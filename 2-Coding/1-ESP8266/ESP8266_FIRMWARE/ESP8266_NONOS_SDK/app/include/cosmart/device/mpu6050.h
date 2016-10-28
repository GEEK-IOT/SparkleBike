/*
 * mpu6050.h
 *
 *  Created on: 2016年4月19日
 *      Author: 80074591
 */
#include "c_types.h"
#include "math.h"

#ifndef APP_INCLUDE_COSMART_DEVICE_MPU6050_H_
#define APP_INCLUDE_COSMART_DEVICE_MPU6050_H_

#define MPU6050_SLAVE_NO_ADDRESS            0x00 // 无效IIC从机地址
#define MPU6050_SLAVE_HIGH_ADDRESS          0xD0 // AD0拉高后，作为IIC从机的地址, 1101 000(rw)
#define MPU6050_SLAVE_LOW_ADDRESS           0xD2 // AD0拉低后，作为IIC从机的地址, 1101 001(rw)

#define CONFIGURATION                       0x1A // 基本配置，包含帧同步采样配置和低通滤波频率设置
#define SAMPLE_RATE_DIVIDER                 0x19 // 采样率分频，用于设置6050陀螺仪的采样输出频率，{[7:0]SMPRT_DIV[7:0]}
#define DIGITAL_LOW_PASS_FILTER_CONFIG      0x1A // 低通滤波频率，{[7]-, [6]-, [5:3]EXT_SYNC_SET[2:0], [2:0]DLPF_CFG[2:0]}
#define DIGITAL_HIGH_PASS_FILTER_CONFIG     0x1C // 高通滤波频率, {[7]XA_ST, [6]YA_ST, [5]ZA_ST, [4:3]AFS_SEL[1:0], [2:0]DHPF[2:0]}
#define EXT_SYNC_CONFIG                     0x1A // 帧同步采样配置，{[7]-, [6]-, [5:3]EXT_SYNC_SET[2:0], [2:0]DLPF_CFG[2:0]}
#define GYROSCOPE_CONFIG                    0x1B // 陀螺仪自检及测量范围，{[7]XG_ST, [6]YG_ST, [5]ZG_ST, [4:3]FS_SEL[1:0], [2:0]-}
#define ACCELEROMETER_CONFIG                0x1C // 加速计自检及测量范围，{[7]XA_ST, [6]YA_ST, [5]ZA_ST, [4:3]AFS_SEL[1:0], [2:0]DHPF[2:0]}
#define TEMPERATURE_FIFO_ENABLE             0x23 // 温度计先入先出缓冲区启用，{[7]TEMP_FIFO_EN, [6]XG_FIFO_EN, [5]YG_FIFO_EN, [4]ZG_FIFO_EN, [3]ACCEL_FIFO_EN, [2:0]SLV(2/1/0)_FIFO_ENABLE}
#define GYROSCOPE_FIFO_ENABLE               0x23 // 陀螺仪先入先出缓冲区启用，{[7]TEMP_FIFO_EN, [6]XG_FIFO_EN, [5]YG_FIFO_EN, [4]ZG_FIFO_EN, [3]ACCEL_FIFO_EN, [2:0]SLV(2/1/0)_FIFO_ENABLE}
#define ACCELEROMETER_FIFO_ENABLE           0x23 // 加速计先入先出缓冲区启用，{[7]TEMP_FIFO_EN, [6]XG_FIFO_EN, [5]YG_FIFO_EN, [4]ZG_FIFO_EN, [3]ACCEL_FIFO_EN, [2:0]SLV(2/1/0)_FIFO_ENABLE}

#define ACCELEROMETER_X_H                   0x3B // 存储最近的X轴加速度感应器的测量值(高位)
#define ACCELEROMETER_X_L                   0x3C // 存储最近的X轴加速度感应器的测量值(低位)
#define ACCELEROMETER_Y_H                   0x3D // 存储最近的Y轴加速度感应器的测量值(高位)
#define ACCELEROMETER_Y_L                   0x3E // 存储最近的Y轴加速度感应器的测量值(低位)
#define ACCELEROMETER_Z_H                   0x3F // 存储最近的Z轴加速度感应器的测量值(高位)
#define ACCELEROMETER_Z_L                   0x40 // 存储最近的Z轴加速度感应器的测量值(低位)
#define TEMPERATURE_H                       0x41 // 存储的最近温度传感器的测量值(高位)
#define TEMPERATURE_L                       0x42 // 存储的最近温度传感器的测量值(低位)
#define GYROSCOPE_X_H                       0x43 // 存储最近的X轴陀螺仪感应器的测量值(高位)
#define GYROSCOPE_X_L                       0x44 // 存储最近的X轴陀螺仪感应器的测量值(低位)
#define GYROSCOPE_Y_H                       0x45 // 存储最近的Y轴陀螺仪感应器的测量值(高位)
#define GYROSCOPE_Y_L                       0x46 // 存储最近的Y轴陀螺仪感应器的测量值(低位)
#define GYROSCOPE_Z_H                       0x47 // 存储最近的Z轴陀螺仪感应器的测量值(高位)
#define GYROSCOPE_Z_L                       0x48 // 存储最近的Z轴陀螺仪感应器的测量值(低位)

#define ACCELEROMETER_RESET                 0x68 // 加速计信道复位，{[7:3]-, [2]GYRO_REST, [1]ACCEL_REST, [0]TEMP_REST}
#define TEMPERATURE_RESET                   0x68 // 陀螺仪信道复位，{[7:3]-, [2]GYRO_REST, [1]ACCEL_REST, [0]TEMP_REST}
#define GYROSCOPE_RESET                     0x68 // 温度计信道复位，{[7:3]-, [2]GYRO_REST, [1]ACCEL_REST, [0]TEMP_REST}
#define PWR_MANAGEMENT_PLAN_A               0x6B // 电源管理方案A，典型值：0x00(正常启用)
#define PWR_MANAGEMENT_PLAN_B               0x6C // 电源管理方案B，典型值：0x00(正常启用)
#define WHO_AM_I                            0x75 // IIC地址寄存器(AD0拉高为0x68，AD0拉低为0xD0，只读)

#define DEVICE_READ_RETRY_COUNT             5    // 读取IIC从机设备重试次数
#define DEVICE_WRITE_RETRY_COUNT            3    // 写入IIC从机设备重试次数
#define SLAVE_DEVICE_COUNT                  2    // 额定设备挂在数量
#define ATTITUDE_POLLING_INTERVAL           1000 // 姿态轮询时长

#define DEFAULT_DLPF                        DLPF_ACC_5Hz_GYRO_5Hz
#define DEFAULT_EXT_SYNC_SET                ESS_InputDisabled
#define DEFAULT_CONFIGURATION_VALUE         ((DEFAULT_EXT_SYNC_SET << 3) | DEFAULT_DLPF)
#define DEFAULT_SAMPLE_RATE_DIVIDER         (0b0111)
#define DEFAULT_GYROSCOPE_CONFIGURATION     ((0b0000 << 7) | (0b0000 << 6) | (0b0000 << 5) | (GYRO_250DegreePerSec << 3))
#define DEFAULT_ACCELEROMETER_CONFIGURATION ((0b0000 << 7) | (0b0000 << 6) | (0b0000 << 5) | (ACCEL_2Grivity << 3))
#define DEFAULT_MANAGEMENT_PLAN_A           (0b0000)

#define ACCEL_2G_Unit                       16384.0 // 加速计2G量程下分辨率，double
#define ACCEL_4G_Unit                       8192.0  // 加速计4G量程下分辨率，double
#define ACCEL_8G_Unit                       4096.0  // 加速计8G量程下分辨率，double
#define ACCEL_16G_Unit                      2048.0  // 加速计16G量程下分辨率，double
#define GYRO_250Deg_Unit                    131.0   // 陀螺仪250Deg/s量程下分辨率，double
#define GYRO_500Deg_Unit                    65.5    // 陀螺仪500Deg/s量程下分辨率，double
#define GYRO_1000Deg_Unit                   32.8    // 陀螺仪1000Deg/s量程下分辨率，double
#define GYRO_2000Deg_Unit                   16.4    // 陀螺仪2000Deg/s量程下分辨率，double

#define TemperatureToCelsius(temp) (temp / 340.0 + 36.53)
#define IntegerOfFloat(value)      ((int)value)
#define DecimalOfFloat(value)      (abs((int)((value - (int)value) * 100)))
// =========================================================================================================================================================
//  Read byte to MPU6050
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
//  Master | [Start] - [SlaveAddress | Write bit] --- [RegisterAddress] --- [Value] --- [Start] - [SlaveAddress | Read bit] ------------- [NACK] - [Stop] |
//  Slave  | ----------------------------------- [ACK] --------------- [ACK] ----- [ACK] --------------------------------- [ACK] - [Value] -------------- |
// =========================================================================================================================================================

// ==============================================================================================
//  Write byte to MPU6050
// ----------------------------------------------------------------------------------------------
//  Master | [Start] - [SlaveAddress | Write bit] --- [RegisterAddress] --- [Value] --- [Stop] |
//  Slave  | ----------------------------------- [ACK] --------------- [ACK] ----- [ACK] ----- |
// ==============================================================================================

typedef enum {
	DLPF_ACC_NONE_GYRO_NONE   = 7,
	DLPF_ACC_5Hz_GYRO_5Hz     = 6,
	DLPF_ACC_10Hz_GYRO_10Hz   = 5,
	DLPF_ACC_21Hz_GYRO_20Hz   = 4,
	DLPF_ACC_44Hz_GYRO_42Hz   = 3,
	DLPF_ACC_94Hz_GYRO_98Hz   = 2,
	DLPF_ACC_184Hz_GYRO_188Hz = 1,
	DLPF_ACC_260Hz_GYRO_256Hz = 0
} DLPF;

typedef enum {
	DHPF_ACC_NONE_GYRO_NONE   = 7
} DHPF;

typedef enum {
	ESS_InputDisabled     = 0,
	ESS_TemperatureOut    = 1,
	ESS_GyroscopeXOut     = 2,
	ESS_GyroscopeYOut     = 3,
	ESS_GyroscopeZOut     = 4,
	ESS_AccelerometerXOut = 5,
	ESS_AccelerometerYOut = 6,
	ESS_AccelerometerZOut = 7
} ExternalSyncSet;

typedef enum {
	GYRO_250DegreePerSec  = 0,
	GYRO_500DegreePerSec  = 1,
	GYRO_1000DegreePerSec = 2,
	GYRO_2000DegreePerSec = 3
} GyroscopeRange;

typedef enum {
	ACCEL_2Grivity  = 0,
	ACCEL_4Grivity  = 1,
	ACCEL_8Grivity  = 2,
	ACCEL_16Grivity = 3
} AccelerometerRange;

typedef struct {
	uint8              address;
	uint32             sampelRate;
	ExternalSyncSet    externalSyncSet;
	DLPF               digitalLowPassFilter;
	DHPF               digitalHighPassFilter;
	GyroscopeRange     gyroscopeRange;
	AccelerometerRange accelerometerRange;
	uint8              sampleRateDivider;
	bool               isGyroscopeFIFO;
	bool               isAccelerometerFIFO;
	bool               isTemperatureFIFO;

	int32              accelerometerXLSB;
	int32              accelerometerYLSB;
	int32              accelerometerZLSB;
	int32              gyroscopeXLSB;
	int32              gyroscopeYLSB;
	int32              gyroscopeZLSB;
	int32              temperatueLSB;

	float              gyroscopeUnit;
	float              accelerometerUnit;
} MPU6050Device;

void        MPU6050_initialize();
int         MPU6050_getSlaveDeviceCount();
int         MPU6050_getValidSlaveDeviceCount();
bool        MPU6050_bindDevice(int index);
void        MPU6050_setSampleRate(uint32 sampleRate);
void        MPU6050_setDigitalLowPassFilter(DLPF config);
void        MPU6050_setDigitalHighPassFilter(DHPF config);
void        MPU6050_setExternalSyncConfig(ExternalSyncSet config);
void        MPU6050_setGyroscopeRange(GyroscopeRange range);
void        MPU6050_setAccelerometerRange(AccelerometerRange range);
void        MPU6050_setGyroscopeFIFOEnabled(bool enabled);
void        MPU6050_setAcceleromterFIFOEnabled(bool enabled);
void        MPU6050_setTemperatureFIFOEnabled(bool enabled);
void        MPU6050_resetGyroscopeSignalChannel();
void        MPU6050_resetAcceleromterSignalChannel();
void        MPU6050_resetTemperatureSignalChannel();
int16       MPU6050_getAccelerometerX();
int16       MPU6050_getAccelerometerY();
int16       MPU6050_getAccelerometerZ();
int16       MPU6050_getGyroscopeX();
int16       MPU6050_getGyroscopeY();
int16       MPU6050_getGyroscopeZ();
int16       MPU6050_getTemperature();
uint32      MPU6050_getGyroscopeFrequencyByDLPFConfig(DLPF config);
const char* MPU6050_getStringOfDLPF(DLPF config);
const char* MPU6050_getStringOfExternalSyncSet(ExternalSyncSet config);
uint32      MPU6050_getValueOfGyroscopeRange(GyroscopeRange range);
uint32      MPU6050_getValueOfAccelerometerRange(AccelerometerRange range);
void        MPU6050_startPollingAttitude();
void        MPU6050_stopPollingAttitude();

#endif /* APP_INCLUDE_COSMART_DEVICE_MPU6050_H_ */
