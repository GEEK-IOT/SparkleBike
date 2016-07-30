/*
 * mpu6050.c
 *
 *  Created on: 2016年4月19日
 *      Author: Cocoonshu
 */

#include "cosmart/device/mpu6050.h"
#include "cosmart/log.h"
#include "cosmart/Timer.h"
#include "driver/i2c_master.h"
#include "mem.h"

LOCAL MPU6050Device* mCurrentDevice                  = NULL;
LOCAL MPU6050Device* mDeviceList[SLAVE_DEVICE_COUNT] = {NULL, NULL};
LOCAL bool           mIsPollingAttitude              = false;
LOCAL Timer          mPollingTimer;

LOCAL bool   readByteFromDevice(MPU6050Device* device, uint8 fieldAddress, uint8* data);
LOCAL bool   writeByteToDevice(MPU6050Device* device, uint8 fieldAddress, uint8 data);
LOCAL bool   readByteFromDeviceWithRetry(MPU6050Device* device, uint8 fieldAddress, uint8* data);
LOCAL bool   writeByteToDeviceWithRetry(MPU6050Device* device, uint8 fieldAddress, uint8 data);
LOCAL uint8  getDeviceReadAddress(MPU6050Device* device);
LOCAL uint8  getDeviceWriteAddress(MPU6050Device* device);
LOCAL bool   testDeviceAddress();
LOCAL uint8  whoAmI(MPU6050Device* device);
LOCAL void   executeDefaultSetup();
LOCAL void   onAttitudeInvalidated();

void ICACHE_FLASH_ATTR MPU6050_initialize() {
	Log_printfln("");
	Log_printfln("[MPU6050] Start MPU6050 driver");
	Log_printfln("[MPU6050] Start I2C manager in master mode");

	mCurrentDevice = NULL;
	int itrator    = 0;
	for (itrator = 0; itrator < SLAVE_DEVICE_COUNT; itrator ++) {
		if (mDeviceList[itrator] != NULL) {
			os_free(mDeviceList[itrator]);
			mDeviceList[itrator] = NULL;
		}
	}

	i2c_master_gpio_init();
	Timer_delayMS(1000);
	if (testDeviceAddress()) {
		executeDefaultSetup();
	}
}

int ICACHE_FLASH_ATTR MPU6050_getSlaveDeviceCount() {
	return SLAVE_DEVICE_COUNT;
}

int ICACHE_FLASH_ATTR MPU6050_getValidSlaveDeviceCount() {
	int validDeviceCount = 0;
	int itrator          = 0;
	for (itrator = 0; itrator < SLAVE_DEVICE_COUNT; itrator ++) {
		if (mDeviceList[itrator] != NULL) {
			validDeviceCount ++;
		}
	}
	return validDeviceCount;
}

bool ICACHE_FLASH_ATTR MPU6050_bindDevice(int index) {
	if (index < 0 || index >= SLAVE_DEVICE_COUNT) {
		return false;
	}
	if (mDeviceList[index] == NULL) {
		return false;
	}
	mCurrentDevice = mDeviceList[index];
	return true;
}

void ICACHE_FLASH_ATTR MPU6050_setSampleRate(uint32 sampleRate) {
	if (mCurrentDevice == NULL) {
		return;
	}

	bool   isSuccess          = false;
	uint32 gyroscopeFrequency = MPU6050_getGyroscopeFrequencyByDLPFConfig(mCurrentDevice->digitalLowPassFilter);
	uint8  sampleRateDivider  = gyroscopeFrequency / sampleRate - 1;
	if (sampleRate <= 0 || sampleRateDivider <= 0) {
		Log_printfln("[MPU6050] Sample rate set failed, %d isn't in range from %d to %d",
				      sampleRate, 1, gyroscopeFrequency);
		return;
	} else {
		isSuccess = writeByteToDeviceWithRetry(
				mCurrentDevice, SAMPLE_RATE_DIVIDER, sampleRateDivider);
		if (isSuccess) {
			mCurrentDevice->sampelRate        = sampleRate;
			mCurrentDevice->sampleRateDivider = sampleRateDivider;
		}
	}
}

void ICACHE_FLASH_ATTR MPU6050_setDigitalLowPassFilter(DLPF config) {
	if (mCurrentDevice == NULL) {
		return;
	}

	bool  isSuccess  = false;
	uint8 extSyncSet = mCurrentDevice->externalSyncSet;
	isSuccess = writeByteToDeviceWithRetry(
					mCurrentDevice, DIGITAL_LOW_PASS_FILTER_CONFIG, ((extSyncSet & 0b0111) << 3 | (config & 0b111)));
	if (isSuccess) {
		mCurrentDevice->digitalLowPassFilter = config;
		uint32 gyroscopeFrequency = MPU6050_getGyroscopeFrequencyByDLPFConfig(config);
		if (mCurrentDevice->sampelRate > gyroscopeFrequency) {
			Log_printfln("[MPU6050] Sample rate has been set to %d because DLPF changed", gyroscopeFrequency);
			MPU6050_setSampleRate(gyroscopeFrequency);
		}
	}
}

void ICACHE_FLASH_ATTR MPU6050_setDigitalHighPassFilter(DHPF config) {
	// TODO unimplemented
}

void ICACHE_FLASH_ATTR MPU6050_setExternalSyncConfig(ExternalSyncSet config) {
	if (mCurrentDevice == NULL) {
		return;
	}

	bool  isSuccess  = false;
	uint8 DLPF       = mCurrentDevice->digitalLowPassFilter;
	isSuccess = writeByteToDeviceWithRetry(
						mCurrentDevice, DIGITAL_LOW_PASS_FILTER_CONFIG, ((config & 0b0111) << 3 | (DLPF & 0b111)));
	if (isSuccess) {
		mCurrentDevice->externalSyncSet = config;
	}
}

void ICACHE_FLASH_ATTR MPU6050_setGyroscopeRange(GyroscopeRange range) {
	if (mCurrentDevice == NULL) {
		return;
	}

	bool isSuccess = false;
	isSuccess = writeByteToDeviceWithRetry(
			mCurrentDevice, GYROSCOPE_CONFIG, ((range & 0b0011) << 3));
	if (isSuccess) {
		mCurrentDevice->gyroscopeRange = range;
		switch (range) {
		case GYRO_250DegreePerSec:  mCurrentDevice->gyroscopeUnit = GYRO_250Deg_Unit;  break;
		case GYRO_500DegreePerSec:  mCurrentDevice->gyroscopeUnit = GYRO_500Deg_Unit;  break;
		case GYRO_1000DegreePerSec: mCurrentDevice->gyroscopeUnit = GYRO_1000Deg_Unit; break;
		case GYRO_2000DegreePerSec: mCurrentDevice->gyroscopeUnit = GYRO_2000Deg_Unit; break;
		}
	}
}

void ICACHE_FLASH_ATTR MPU6050_setAccelerometerRange(AccelerometerRange range) {
	if (mCurrentDevice == NULL) {
		return;
	}

	bool isSuccess = false;
	isSuccess = writeByteToDeviceWithRetry(
			mCurrentDevice, ACCELEROMETER_CONFIG, ((range & 0b0011) << 3));
	if (isSuccess) {
		mCurrentDevice->accelerometerRange = range;
		switch (range) {
		case ACCEL_2Grivity:  mCurrentDevice->accelerometerUnit = ACCEL_2G_Unit;  break;
		case ACCEL_4Grivity:  mCurrentDevice->accelerometerUnit = ACCEL_4G_Unit;  break;
		case ACCEL_8Grivity:  mCurrentDevice->accelerometerUnit = ACCEL_8G_Unit;  break;
		case ACCEL_16Grivity: mCurrentDevice->accelerometerUnit = ACCEL_16G_Unit; break;
		}
	}
}

void ICACHE_FLASH_ATTR MPU6050_setGyroscopeFIFOEnabled(bool enabled) {
	if (mCurrentDevice == NULL) {
		return;
	}

	bool isSuccess           = false;
	bool isAccelerometerFIFO = mCurrentDevice->isAccelerometerFIFO;
	bool isTemperatureFIFO   = mCurrentDevice->isTemperatureFIFO;
	bool isGyroscopeFIFO     = enabled;
	isSuccess = writeByteToDeviceWithRetry(
			mCurrentDevice, GYROSCOPE_FIFO_ENABLE,
			((isTemperatureFIFO   & 0b0001) << 7) |
			((isGyroscopeFIFO     & 0b0001) << 6) |
			((isGyroscopeFIFO     & 0b0001) << 5) |
			((isGyroscopeFIFO     & 0b0001) << 4) |
			((isAccelerometerFIFO & 0b0001) << 3));
	if (isSuccess) {
		mCurrentDevice->isGyroscopeFIFO = enabled;
	}
}

void ICACHE_FLASH_ATTR MPU6050_setAcceleromterFIFOEnabled(bool enabled) {
	if (mCurrentDevice == NULL) {
		return;
	}

	bool isSuccess           = false;
	bool isAccelerometerFIFO = enabled;
	bool isTemperatureFIFO   = mCurrentDevice->isTemperatureFIFO;
	bool isGyroscopeFIFO     = mCurrentDevice->isGyroscopeFIFO;
	isSuccess = writeByteToDeviceWithRetry(
			mCurrentDevice, ACCELEROMETER_FIFO_ENABLE,
			((isTemperatureFIFO   & 0b0001) << 7) |
			((isGyroscopeFIFO     & 0b0001) << 6) |
			((isGyroscopeFIFO     & 0b0001) << 5) |
			((isGyroscopeFIFO     & 0b0001) << 4) |
			((isAccelerometerFIFO & 0b0001) << 3));
	if (isSuccess) {
		mCurrentDevice->isAccelerometerFIFO = enabled;
	}
}

void ICACHE_FLASH_ATTR MPU6050_setTemperatureFIFOEnabled(bool enabled) {
	if (mCurrentDevice == NULL) {
		return;
	}

	bool isSuccess           = false;
	bool isAccelerometerFIFO = mCurrentDevice->isAccelerometerFIFO;
	bool isTemperatureFIFO   = enabled;
	bool isGyroscopeFIFO     = mCurrentDevice->isGyroscopeFIFO;
	isSuccess = writeByteToDeviceWithRetry(
			mCurrentDevice, TEMPERATURE_FIFO_ENABLE,
			((isTemperatureFIFO   & 0b0001) << 7) |
			((isGyroscopeFIFO     & 0b0001) << 6) |
			((isGyroscopeFIFO     & 0b0001) << 5) |
			((isGyroscopeFIFO     & 0b0001) << 4) |
			((isAccelerometerFIFO & 0b0001) << 3));
	if (isSuccess) {
		mCurrentDevice->isTemperatureFIFO = enabled;
	}
}

void ICACHE_FLASH_ATTR MPU6050_resetGyroscopeSignalChannel() {
	if (mCurrentDevice == NULL) {
		return;
	}

	bool isSuccess = false;
	isSuccess = writeByteToDeviceWithRetry(
			mCurrentDevice, GYROSCOPE_RESET, (1 << 2));
	if (isSuccess) {
		Log_printfln("[MPU6050] Gyroscope of Device 0x%x has been reset", mCurrentDevice->address);
	} else {
		Log_printfln("[MPU6050] Gyroscope of Device 0x%x reset failed", mCurrentDevice->address);
	}
}

void ICACHE_FLASH_ATTR MPU6050_resetAcceleromterSignalChannel() {
	if (mCurrentDevice == NULL) {
		return;
	}

	bool isSuccess = false;
	isSuccess = writeByteToDeviceWithRetry(
			mCurrentDevice, ACCELEROMETER_RESET, (1 << 1));
	if (isSuccess) {
		Log_printfln("[MPU6050] Accelerometer of Device 0x%x has been reset", mCurrentDevice->address);
	} else {
		Log_printfln("[MPU6050] Accelerometer of Device 0x%x reset failed", mCurrentDevice->address);
	}
}

void ICACHE_FLASH_ATTR MPU6050_resetTemperatureSignalChannel() {
	if (mCurrentDevice == NULL) {
		return;
	}

	bool isSuccess = false;
	isSuccess = writeByteToDeviceWithRetry(
			mCurrentDevice, TEMPERATURE_RESET, (1 << 0));
	if (isSuccess) {
		Log_printfln("[MPU6050] Temperature of Device 0x%x has been reset", mCurrentDevice->address);
	} else {
		Log_printfln("[MPU6050] Temperature of Device 0x%x reset failed", mCurrentDevice->address);
	}
}

int16 ICACHE_FLASH_ATTR MPU6050_getAccelerometerX() {
	if (mCurrentDevice == NULL) {
		return 0;
	}

	bool  isSuccess           = true;
	uint8 accelerometerXHigh  = 0;
	uint8 accelerometerXLow   = 0;
	int16 accelerometerXValue = 0;

	// TODO Detect Data Ready Interrupt to ensure MPU6050 updated
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, ACCELEROMETER_X_H, &accelerometerXHigh);
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, ACCELEROMETER_X_L, &accelerometerXLow);
	if (isSuccess) {
		if ((accelerometerXHigh & 0b10000000) == 0b10000000) {
			// 负数，取补码
			accelerometerXValue = ((~((accelerometerXHigh << 8) | accelerometerXLow)) | 0x8000) + 1;
		} else {
			// 正数，取原码
			accelerometerXValue = (accelerometerXHigh << 8) | accelerometerXLow;
		}
		mCurrentDevice->accelerometerXLSB = accelerometerXValue;
		return accelerometerXValue;
	} else {
		return 0;
	}
}

int16 ICACHE_FLASH_ATTR MPU6050_getAccelerometerY() {
	if (mCurrentDevice == NULL) {
		return 0;
	}

	bool  isSuccess           = true;
	uint8 accelerometerYHigh  = 0;
	uint8 accelerometerYLow   = 0;
	int16 accelerometerYValue = 0;

	// TODO Detect Data Ready Interrupt to ensure MPU6050 updated
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, ACCELEROMETER_Y_H, &accelerometerYHigh);
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, ACCELEROMETER_Y_L, &accelerometerYLow);
	if (isSuccess) {
		if ((accelerometerYHigh & 0b10000000) == 0b10000000) {
			// 负数，取补码
			accelerometerYValue = ((~((accelerometerYHigh << 8) | accelerometerYLow)) | 0x8000) + 1;
		} else {
			// 正数，取原码
			accelerometerYValue = (accelerometerYHigh << 8) | accelerometerYLow;
		}
		mCurrentDevice->accelerometerYLSB = accelerometerYValue;
		return accelerometerYValue;
	} else {
		return 0;
	}
}

int16 ICACHE_FLASH_ATTR MPU6050_getAccelerometerZ() {
	if (mCurrentDevice == NULL) {
		return 0;
	}

	bool  isSuccess           = true;
	uint8 accelerometerZHigh  = 0;
	uint8 accelerometerZLow   = 0;
	int16 accelerometerZValue = 0;

	// TODO Detect Data Ready Interrupt to ensure MPU6050 updated
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, ACCELEROMETER_Z_H, &accelerometerZHigh);
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, ACCELEROMETER_Z_L, &accelerometerZLow);
	if (isSuccess) {
		if ((accelerometerZHigh & 0b10000000) == 0b10000000) {
			// 负数，取补码
			accelerometerZValue = ((~((accelerometerZHigh << 8) | accelerometerZLow)) | 0x8000) + 1;
		} else {
			// 正数，取原码
			accelerometerZValue = (accelerometerZHigh << 8) | accelerometerZLow;
		}
		mCurrentDevice->accelerometerZLSB = accelerometerZValue;
		return accelerometerZValue;
	} else {
		return 0;
	}
}

int16 ICACHE_FLASH_ATTR MPU6050_getGyroscopeX() {
	if (mCurrentDevice == NULL) {
		return 0;
	}

	bool  isSuccess       = true;
	uint8 gyroscopeXHigh  = 0;
	uint8 gyroscopeXLow   = 0;
	int16 gyroscopeXValue = 0;

	// TODO Detect Data Ready Interrupt to ensure MPU6050 updated
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, GYROSCOPE_X_H, &gyroscopeXHigh);
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, GYROSCOPE_X_L, &gyroscopeXLow);
	if (isSuccess) {
		if ((gyroscopeXHigh & 0b10000000) == 0b10000000) {
			// 负数，取补码
			gyroscopeXValue = ((~((gyroscopeXHigh << 8) | gyroscopeXLow)) | 0x8000) + 1;
		} else {
			// 正数，取原码
			gyroscopeXValue = (gyroscopeXHigh << 8) | gyroscopeXLow;
		}
		mCurrentDevice->gyroscopeXLSB = gyroscopeXValue;
		return gyroscopeXValue;
	} else {
		return 0;
	}
}

int16 ICACHE_FLASH_ATTR MPU6050_getGyroscopeY() {
	if (mCurrentDevice == NULL) {
		return 0;
	}

	bool  isSuccess       = true;
	uint8 gyroscopeYHigh  = 0;
	uint8 gyroscopeYLow   = 0;
	int16 gyroscopeYValue = 0;

	// TODO Detect Data Ready Interrupt to ensure MPU6050 updated
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, GYROSCOPE_Y_H, &gyroscopeYHigh);
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, GYROSCOPE_Y_L, &gyroscopeYLow);
	if (isSuccess) {
		if ((gyroscopeYHigh & 0b10000000) == 0b10000000) {
			// 负数，取补码
			gyroscopeYValue = ((~((gyroscopeYHigh << 8) | gyroscopeYLow)) | 0x8000) + 1;
		} else {
			// 正数，取原码
			gyroscopeYValue = (gyroscopeYHigh << 8) | gyroscopeYLow;
		}
		mCurrentDevice->gyroscopeYLSB = gyroscopeYValue;
		return gyroscopeYValue;
	} else {
		return 0;
	}
}

int16 ICACHE_FLASH_ATTR MPU6050_getGyroscopeZ() {
	if (mCurrentDevice == NULL) {
		return 0;
	}

	bool  isSuccess       = true;
	uint8 gyroscopeZHigh  = 0;
	uint8 gyroscopeZLow   = 0;
	int16 gyroscopeZValue = 0;

	// TODO Detect Data Ready Interrupt to ensure MPU6050 updated
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, GYROSCOPE_Z_H, &gyroscopeZHigh);
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, GYROSCOPE_Z_L, &gyroscopeZLow);
	if (isSuccess) {
		if ((gyroscopeZHigh & 0b10000000) == 0b10000000) {
			// 负数，取补码
			gyroscopeZValue = ((~((gyroscopeZHigh << 8) | gyroscopeZLow)) | 0x8000) + 1;
		} else {
			// 正数，取原码
			gyroscopeZValue = (gyroscopeZHigh << 8) | gyroscopeZLow;
		}
		mCurrentDevice->gyroscopeZLSB = gyroscopeZValue;
		return gyroscopeZValue;
	} else {
		return 0;
	}
}

int16 ICACHE_FLASH_ATTR MPU6050_getTemperature() {
	if (mCurrentDevice == NULL) {
		return 0;
	}

	bool  isSuccess        = true;
	uint8 temperatureHigh  = 0;
	uint8 temperatureLow   = 0;
	int16 temperatureValue = 0;

	// TODO Detect Data Ready Interrupt to ensure MPU6050 updated
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, TEMPERATURE_H, &temperatureHigh);
	isSuccess &= readByteFromDeviceWithRetry(
			mCurrentDevice, TEMPERATURE_L, &temperatureLow);
	if (isSuccess) {
		if ((temperatureHigh & 0b10000000) == 0b10000000) {
			// 负数，取补码
			temperatureValue = ((~((temperatureHigh << 8) | temperatureLow)) | 0x8000) + 1;
		} else {
			// 正数，取原码
			temperatureValue = (temperatureHigh << 8) | temperatureLow;
		}
		mCurrentDevice->gyroscopeZLSB = temperatureValue;
		return temperatureValue;
	} else {
		return 0;
	}
}

uint32 ICACHE_FLASH_ATTR MPU6050_getGyroscopeFrequencyByDLPFConfig(DLPF config) {
	switch (config) {
	case DLPF_ACC_NONE_GYRO_NONE:   return 8000;
	case DLPF_ACC_5Hz_GYRO_5Hz:     return 1000;
	case DLPF_ACC_10Hz_GYRO_10Hz:   return 1000;
	case DLPF_ACC_21Hz_GYRO_20Hz:   return 1000;
	case DLPF_ACC_44Hz_GYRO_42Hz:   return 1000;
	case DLPF_ACC_94Hz_GYRO_98Hz:   return 1000;
	case DLPF_ACC_184Hz_GYRO_188Hz: return 1000;
	case DLPF_ACC_260Hz_GYRO_256Hz: return 8000;
	default: return 0;
	}
}

const char* ICACHE_FLASH_ATTR MPU6050_getStringOfDLPF(DLPF config) {
	switch (config) {
	case DLPF_ACC_NONE_GYRO_NONE:   return "LOW_ACC_NONE_GYRO_NONE";
	case DLPF_ACC_5Hz_GYRO_5Hz:     return "LOW_ACC_5Hz_GYRO_5Hz";
	case DLPF_ACC_10Hz_GYRO_10Hz:   return "LOW_ACC_10Hz_GYRO_10Hz";
	case DLPF_ACC_21Hz_GYRO_20Hz:   return "LOW_ACC_21Hz_GYRO_20Hz";
	case DLPF_ACC_44Hz_GYRO_42Hz:   return "LOW_ACC_44Hz_GYRO_42Hz";
	case DLPF_ACC_94Hz_GYRO_98Hz:   return "LOW_ACC_94Hz_GYRO_98Hz";
	case DLPF_ACC_184Hz_GYRO_188Hz: return "LOW_ACC_184Hz_GYRO_188Hz";
	case DLPF_ACC_260Hz_GYRO_256Hz: return "LOW_ACC_260Hz_GYRO_256Hz";
	default: return "Unknown";
	}
}

const char* ICACHE_FLASH_ATTR MPU6050_getStringOfExternalSyncSet(ExternalSyncSet config) {
	switch (config) {
	case ESS_InputDisabled:     return "ESS_InputDisabled";
	case ESS_TemperatureOut:    return "ESS_TemperatureOut";
	case ESS_GyroscopeXOut:     return "ESS_GyroscopeXOut";
	case ESS_GyroscopeYOut:     return "ESS_GyroscopeYOut";
	case ESS_GyroscopeZOut:     return "ESS_GyroscopeZOut";
	case ESS_AccelerometerXOut: return "ESS_AccelerometerXOut";
	case ESS_AccelerometerYOut: return "ESS_AccelerometerYOut";
	case ESS_AccelerometerZOut: return "ESS_AccelerometerZOut";
	default: return "Unknown";
	}
}

uint32 ICACHE_FLASH_ATTR MPU6050_getValueOfGyroscopeRange(GyroscopeRange range) {
	switch (range) {
	case GYRO_250DegreePerSec:  return 250;
	case GYRO_500DegreePerSec:  return 500;
	case GYRO_1000DegreePerSec: return 1000;
	case GYRO_2000DegreePerSec: return 2000;
	default: return 0;
	}
}

uint32 ICACHE_FLASH_ATTR MPU6050_getValueOfAccelerometerRange(AccelerometerRange range) {
	switch (range) {
	case ACCEL_2Grivity:  return 2;
	case ACCEL_4Grivity:  return 4;
	case ACCEL_8Grivity:  return 8;
	case ACCEL_16Grivity: return 16;
	default: return 0;
	}
}

void ICACHE_FLASH_ATTR MPU6050_startPollingAttitude() {
	if (!mIsPollingAttitude) {
		Timer_delayMS(1000);
		Timer_setCallback(&mPollingTimer, onAttitudeInvalidated);
		Timer_setLoopable(&mPollingTimer, true);
		Timer_setTimerUnit(&mPollingTimer, MSTimer);
		Timer_setInterval(&mPollingTimer, ATTITUDE_POLLING_INTERVAL);
		Timer_start(&mPollingTimer);
		mIsPollingAttitude = true;
	}
}

void ICACHE_FLASH_ATTR MPU6050_stopPollingAttitude() {
	if (mIsPollingAttitude) {
		Timer_stop(&mPollingTimer);
	}
}

LOCAL uint8 ICACHE_FLASH_ATTR whoAmI(MPU6050Device* device) {
	if (device == NULL) {
		return 0;
	}

	uint8 deviceAddress = 0;
	bool  isSuccess     = false;
	isSuccess = readByteFromDeviceWithRetry(device, WHO_AM_I, &deviceAddress);
	if (isSuccess) {
		return deviceAddress;
	} else {
		return 0;
	}
}

LOCAL bool ICACHE_FLASH_ATTR testDeviceAddress() {
	uint8 ack             = I2C_ACK_OK;
	uint8 currentAddress  = MPU6050_SLAVE_NO_ADDRESS;

	if (mDeviceList[0] != NULL) {
		os_free(mDeviceList[0]);
		mDeviceList[0] = NULL;
	}
	if (mDeviceList[1] != NULL) {
		os_free(mDeviceList[1]);
		mDeviceList[1] = NULL;
	}

	// Test High address
	i2c_master_start();
	i2c_master_writeByte(MPU6050_SLAVE_HIGH_ADDRESS);
	if ((ack = i2c_master_getAck()) == I2C_ACK_OK) {
		i2c_master_stop();
		currentAddress = MPU6050_SLAVE_HIGH_ADDRESS;
		mDeviceList[0] = (MPU6050Device*)os_zalloc(sizeof(MPU6050Device));
		os_memset(mDeviceList[0], 0, sizeof(MPU6050Device));
		mDeviceList[0]->address               = MPU6050_SLAVE_HIGH_ADDRESS;
		mDeviceList[0]->sampelRate            = 8000;
		mDeviceList[0]->externalSyncSet       = DEFAULT_EXT_SYNC_SET;
		mDeviceList[0]->digitalLowPassFilter  = DEFAULT_DLPF;
		mDeviceList[0]->digitalHighPassFilter = DHPF_ACC_NONE_GYRO_NONE;
		mDeviceList[0]->gyroscopeRange        = GYRO_250DegreePerSec;
		mDeviceList[0]->gyroscopeUnit         = GYRO_250Deg_Unit;
		mDeviceList[0]->accelerometerRange    = ACCEL_2Grivity;
		mDeviceList[0]->accelerometerUnit     = ACCEL_2G_Unit;
		mDeviceList[0]->sampleRateDivider     = 0;
		mDeviceList[0]->isGyroscopeFIFO       = false;
		mDeviceList[0]->isAccelerometerFIFO   = false;
		mDeviceList[0]->isTemperatureFIFO     = false;
		mCurrentDevice = mDeviceList[0];

		uint8 id = whoAmI(mDeviceList[0]);
		Log_printfln("       - Found MPU6050 slave on I2C Bus port 0x%x, id 0x%x", MPU6050_SLAVE_HIGH_ADDRESS, id);

		return true;
	}

	// Test Low address
	i2c_master_start();
	i2c_master_writeByte(MPU6050_SLAVE_LOW_ADDRESS);
	if ((ack = i2c_master_getAck()) == I2C_ACK_OK) {
		i2c_master_stop();
		currentAddress = MPU6050_SLAVE_LOW_ADDRESS;
		mDeviceList[1] = (MPU6050Device*)os_zalloc(sizeof(MPU6050Device));
		os_memset(mDeviceList[1], 0, sizeof(MPU6050Device));
		mDeviceList[1]->address               = MPU6050_SLAVE_LOW_ADDRESS;
		mDeviceList[1]->sampelRate            = 8000;
		mDeviceList[1]->externalSyncSet       = DEFAULT_EXT_SYNC_SET;
		mDeviceList[1]->digitalLowPassFilter  = DEFAULT_DLPF;
		mDeviceList[1]->digitalHighPassFilter = DHPF_ACC_NONE_GYRO_NONE;
		mDeviceList[1]->gyroscopeRange        = GYRO_250DegreePerSec;
		mDeviceList[1]->gyroscopeUnit         = GYRO_250Deg_Unit;
		mDeviceList[1]->accelerometerRange    = ACCEL_2Grivity;
		mDeviceList[1]->accelerometerUnit     = ACCEL_2G_Unit;
		mDeviceList[1]->sampleRateDivider     = 0;
		mDeviceList[1]->isGyroscopeFIFO       = false;
		mDeviceList[1]->isAccelerometerFIFO   = false;
		mDeviceList[1]->isTemperatureFIFO     = false;
		if (mCurrentDevice == NULL) {
			mCurrentDevice = mDeviceList[1];
		}

		uint8 id = whoAmI(mDeviceList[1]);
		Log_printfln("       - Found MPU6050 slave on I2C Bus port 0x%x, id 0x%x", MPU6050_SLAVE_LOW_ADDRESS, id);
		return true;
	}

	if (currentAddress == MPU6050_SLAVE_NO_ADDRESS) {
		Log_printfln("[MPU6050] No slave device found on I2C bus");
		return false;
	}
}

LOCAL void ICACHE_FLASH_ATTR executeDefaultSetup() {
	int  i         = 0;
	bool isSuccess = false;
	for (i = 0; i < SLAVE_DEVICE_COUNT; i ++) {
		if (mDeviceList[i] != NULL) {
			// 1. Setup MPU6050 CONFIGURATION
			isSuccess = writeByteToDeviceWithRetry(
					mDeviceList[i], CONFIGURATION, DEFAULT_CONFIGURATION_VALUE);
			if (isSuccess) {
				mDeviceList[i]->externalSyncSet      = DEFAULT_EXT_SYNC_SET;
				mDeviceList[i]->digitalLowPassFilter = DEFAULT_DLPF;
				Log_printfln("       - Setup external sync set to %s", MPU6050_getStringOfExternalSyncSet(mDeviceList[i]->externalSyncSet));
				Log_printfln("       - Setup digital low pass filter to %s", MPU6050_getStringOfDLPF(mDeviceList[i]->digitalLowPassFilter));

				uint8 feedback = 0;
				readByteFromDeviceWithRetry(mDeviceList[i], CONFIGURATION, &feedback);
				Log_printfln("       - Feedback of CONFIGURATION is 0x%x", feedback);
			} else {
				Log_printfln("       - Setup external sync set to %s failed", MPU6050_getStringOfExternalSyncSet(DEFAULT_EXT_SYNC_SET));
				Log_printfln("       - Setup digital low pass filter to %s failed", MPU6050_getStringOfDLPF(DEFAULT_DLPF));
			}

			// 2. Setup MPU6050 SAMPLE_RATE_DIVIDER
			isSuccess = writeByteToDeviceWithRetry(
					mDeviceList[i], SAMPLE_RATE_DIVIDER, DEFAULT_SAMPLE_RATE_DIVIDER);
			if (isSuccess) {
				uint32 gyroscopeFrequency = MPU6050_getGyroscopeFrequencyByDLPFConfig(mDeviceList[i]->digitalLowPassFilter);
				mDeviceList[i]->sampelRate = gyroscopeFrequency / DEFAULT_SAMPLE_RATE_DIVIDER;
				Log_printfln("       - Setup sample rate to %dHz", mDeviceList[i]->sampelRate);

				uint8 feedback = 0;
				readByteFromDeviceWithRetry(mDeviceList[i], SAMPLE_RATE_DIVIDER, &feedback);
				Log_printfln("       - Feedback of SAMPLE_RATE_DIVIDER is 0x%x", feedback);
			} else {
				uint32 gyroscopeFrequency = MPU6050_getGyroscopeFrequencyByDLPFConfig(mDeviceList[i]->digitalLowPassFilter);
				uint32 sampelRate = gyroscopeFrequency / DEFAULT_SAMPLE_RATE_DIVIDER;
				Log_printfln("       - Setup sample rate to %dHz failed", sampelRate);
			}

			// 3. Setup MPU6050 GYROSCOPE_CONFIGURATION
			isSuccess = writeByteToDeviceWithRetry(
					mDeviceList[i], GYROSCOPE_CONFIG, DEFAULT_GYROSCOPE_CONFIGURATION);
			if (isSuccess) {
				mDeviceList[i]->gyroscopeRange = GYRO_250DegreePerSec;
				mDeviceList[i]->gyroscopeUnit  = GYRO_250Deg_Unit;
				Log_printfln("       - Setup gyroscope range to %dDeg/s", MPU6050_getValueOfGyroscopeRange(mDeviceList[i]->gyroscopeRange));

				uint8 feedback = 0;
				readByteFromDeviceWithRetry(mDeviceList[i], GYROSCOPE_CONFIG, &feedback);
				Log_printfln("       - Feedback of GYROSCOPE_CONFIG is 0x%x", feedback);
			} else {
				Log_printfln("       - Setup gyroscope range to %dDeg/s failed", MPU6050_getValueOfGyroscopeRange(GYRO_250DegreePerSec));
			}

			// 4. Setup MPU6050 ACCELEROMETER_CONFIGURATION
			isSuccess = writeByteToDeviceWithRetry(
					mDeviceList[i], ACCELEROMETER_CONFIG, DEFAULT_ACCELEROMETER_CONFIGURATION);
			if (isSuccess) {
				mDeviceList[i]->accelerometerRange = ACCEL_16Grivity;
				mDeviceList[i]->accelerometerUnit  = ACCEL_16G_Unit;
				Log_printfln("       - Setup accelerometer range to %dGrivity", MPU6050_getValueOfAccelerometerRange(mDeviceList[i]->accelerometerRange));

				uint8 feedback = 0;
				readByteFromDeviceWithRetry(mDeviceList[i], ACCELEROMETER_CONFIG, &feedback);
				Log_printfln("       - Feedback of ACCELEROMETER_CONFIG is 0x%x", feedback);
			} else {
				Log_printfln("       - Setup accelerometer range to %dGrivity failed", MPU6050_getValueOfAccelerometerRange(ACCEL_16Grivity));
			}

			// 5. Setup MPU6050 POWER_MANAGMENT
			isSuccess = writeByteToDeviceWithRetry(
					mDeviceList[i], PWR_MANAGEMENT_PLAN_A, DEFAULT_MANAGEMENT_PLAN_A);
			if (isSuccess) {
				Log_printfln("       - Setup power management to 0x%x", DEFAULT_MANAGEMENT_PLAN_A);

				uint8 feedback = 0;
				readByteFromDeviceWithRetry(mDeviceList[i], PWR_MANAGEMENT_PLAN_A, &feedback);
				Log_printfln("       - Feedback of PWR_MANAGEMENT_PLAN_A is 0x%x", feedback);
			} else {
				Log_printfln("       - Setup power management to 0x%x failed", DEFAULT_MANAGEMENT_PLAN_A);
			}
		}
	}
}

LOCAL bool ICACHE_FLASH_ATTR readByteFromDeviceWithRetry(MPU6050Device* device, uint8 fieldAddress, uint8* data) {
	int   retryItr  = 0;
	bool  isSuccess = true;
	uint8 address   = getDeviceWriteAddress(device);

	if (address == MPU6050_SLAVE_NO_ADDRESS) {
		Log_printfln("[MPU6050] Device address is invalid");
		return false;
	}
	do {
		isSuccess = readByteFromDevice(device, fieldAddress, data);
		retryItr ++;
	} while (!isSuccess && retryItr <= DEVICE_READ_RETRY_COUNT);
	return isSuccess;
}

LOCAL bool ICACHE_FLASH_ATTR writeByteToDeviceWithRetry(MPU6050Device* device, uint8 fieldAddress, uint8 data) {
	int   retryItr  = 0;
	bool  isSuccess = true;
	uint8 address   = getDeviceWriteAddress(device);

	if (address == MPU6050_SLAVE_NO_ADDRESS) {
		Log_printfln("[MPU6050] Device address is invalid");
		return false;
	}
	do {
		isSuccess = writeByteToDevice(device, fieldAddress, data);
		retryItr ++;
	} while (!isSuccess && retryItr <= DEVICE_WRITE_RETRY_COUNT);
	return isSuccess;
}

LOCAL bool ICACHE_FLASH_ATTR readByteFromDevice(MPU6050Device* device, uint8 fieldAddress, uint8* data) {
	uint8 ack     = I2C_ACK_OK;
	uint8 address = getDeviceWriteAddress(device);

	if (address == MPU6050_SLAVE_NO_ADDRESS) {
		Log_printfln("[MPU6050] Device address is invalid");
		return false;
	}

	i2c_master_start();
	i2c_master_writeByte(address);
	if ((ack = i2c_master_getAck()) != I2C_ACK_OK) {
		i2c_master_stop();
		Log_printfln("[MPU6050] Device isn't ACK after requesting read register 0x%x on 0x%x",
				      fieldAddress, address);
		return false;
	}
	i2c_master_writeByte(fieldAddress);
	if ((ack = i2c_master_getAck()) != I2C_ACK_OK) {
		i2c_master_stop();
		Log_printfln("[MPU6050] Device isn't ACK after sending read register name 0x%x on 0x%x",
				      fieldAddress, address);
		return false;
	}
	i2c_master_start();
	i2c_master_writeByte(getDeviceReadAddress(device));
	if ((ack = i2c_master_getAck()) != I2C_ACK_OK) {
		i2c_master_stop();
		Log_printfln("[MPU6050] Device isn't ACK after waiting read register 0x%x on 0x%x",
				      fieldAddress, address);
		return false;
	}

	*data = i2c_master_readByte();
	i2c_master_send_nack();
	i2c_master_stop();
	return true;
}

LOCAL bool ICACHE_FLASH_ATTR writeByteToDevice(MPU6050Device* device, uint8 fieldAddress, uint8 data) {
	uint8 ack     = I2C_ACK_OK;
	uint8 address = getDeviceWriteAddress(device);

	if (address == MPU6050_SLAVE_NO_ADDRESS) {
		Log_printfln("[MPU6050] Device address is invalid");
		return false;
	}

	i2c_master_start();
	i2c_master_writeByte(address);
	if ((ack = i2c_master_getAck()) != I2C_ACK_OK) {
		i2c_master_stop();
		Log_printfln("[MPU6050] Device isn't ACK after requesting write register 0x%x on 0x%x",
				      fieldAddress, address);
		return false;
	}
	i2c_master_writeByte(fieldAddress);
	if ((ack = i2c_master_getAck()) != I2C_ACK_OK) {
		i2c_master_stop();
		Log_printfln("[MPU6050] Device isn't ACK after sending write register name 0x%x on 0x%x",
				      fieldAddress, address);
		return false;
	}
	i2c_master_writeByte(data);
	if ((ack = i2c_master_getAck()) != I2C_ACK_OK) {
		i2c_master_stop();
		Log_printfln("[MPU6050] Device isn't ACK after writing value to 0x%x on 0x%x",
				      fieldAddress, address);
		return false;
	}
	i2c_master_stop();
	Timer_delayMS(5);
	return true;
}

LOCAL uint8 getDeviceReadAddress(MPU6050Device* device) {
	if (device == NULL) {
		return MPU6050_SLAVE_NO_ADDRESS;
	}
	if (device->address == MPU6050_SLAVE_NO_ADDRESS) {
		return MPU6050_SLAVE_NO_ADDRESS;
	} else {
		return device->address | 1;
	}
}

LOCAL uint8 getDeviceWriteAddress(MPU6050Device* device) {
	if (device == NULL) {
		return MPU6050_SLAVE_NO_ADDRESS;
	}
	if (device->address == MPU6050_SLAVE_NO_ADDRESS) {
		return MPU6050_SLAVE_NO_ADDRESS;
	} else {
		return device->address | 0;
	}
}

LOCAL void ICACHE_FLASH_ATTR onAttitudeInvalidated() {
	if (mCurrentDevice == NULL) {
		Log_printfln("[MPU6050] Device is null, give up polling");
		return;
	}

	Log_printfln("[MPU6050] ========= MPU6050 =========");
	int16 gyroscopeX            = MPU6050_getGyroscopeX();
	int16 gyroscopeY            = MPU6050_getGyroscopeY();
	int16 gyroscopeZ            = MPU6050_getGyroscopeZ();
	int16 accelerometerX        = MPU6050_getAccelerometerX();
	int16 accelerometerY        = MPU6050_getAccelerometerY();
	int16 accelerometerZ        = MPU6050_getAccelerometerZ();
	int16 temperature           = MPU6050_getTemperature();
	uint8 id                    = whoAmI(mCurrentDevice);
	float temperatureCelsius    = TemperatureToCelsius(temperature);
	float gyroscopeXDegPerSec   = gyroscopeX / mCurrentDevice->gyroscopeUnit;
	float gyroscopeYDegPerSec   = gyroscopeY / mCurrentDevice->gyroscopeUnit;
	float gyroscopeZDegPerSec   = gyroscopeZ / mCurrentDevice->gyroscopeUnit;
	float accelerometerXGravity = accelerometerX / mCurrentDevice->accelerometerUnit;
	float accelerometerYGravity = accelerometerY / mCurrentDevice->accelerometerUnit;
	float accelerometerZGravity = accelerometerZ / mCurrentDevice->accelerometerUnit;

	Log_printfln("[MPU6050] Gyroscope:     (%d.%d, %d.%d, %d.%d)Deg/s, unit: %d.%d",
			IntegerOfFloat(gyroscopeXDegPerSec), DecimalOfFloat(gyroscopeXDegPerSec),
			IntegerOfFloat(gyroscopeYDegPerSec), DecimalOfFloat(gyroscopeYDegPerSec),
			IntegerOfFloat(gyroscopeZDegPerSec), DecimalOfFloat(gyroscopeZDegPerSec),
			IntegerOfFloat(mCurrentDevice->gyroscopeUnit), DecimalOfFloat(mCurrentDevice->gyroscopeUnit));
	Log_printfln("[MPU6050] Accelerometer: (%d.%d, %d.%d, %d.%d)G, unit: %d.%d",
			IntegerOfFloat(accelerometerXGravity), DecimalOfFloat(accelerometerXGravity),
			IntegerOfFloat(accelerometerYGravity), DecimalOfFloat(accelerometerYGravity),
			IntegerOfFloat(accelerometerZGravity), DecimalOfFloat(accelerometerZGravity),
			IntegerOfFloat(mCurrentDevice->accelerometerUnit), DecimalOfFloat(mCurrentDevice->accelerometerUnit));
	Log_printfln("[MPU6050] Temperature:    %d.%d C, HEX: 0x%x",
			IntegerOfFloat(temperatureCelsius), DecimalOfFloat(temperatureCelsius), temperature);
	Log_printfln("[MPU6050] ID         :    0x%x"       , id);

//	Log_printfln("[MPU6050] Gyroscope:     (%d, %d, %d)",    gyroscopeX, gyroscopeY, gyroscopeZ);
//	Log_printfln("[MPU6050] Accelerometer: (%d, %d, %d)",    accelerometerX, accelerometerY, accelerometerZ);
//	Log_printfln("[MPU6050] Temperature:    %d.%d C [0x%x]", IntegerOfFloat(temperatureCelsius), DecimalOfFloat(temperatureCelsius), temperature);
//	Log_printfln("[MPU6050] ID         :    0x%x"       ,    id);
}
