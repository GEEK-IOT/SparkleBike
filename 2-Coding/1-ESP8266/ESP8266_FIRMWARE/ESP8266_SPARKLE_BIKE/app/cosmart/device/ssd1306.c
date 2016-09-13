/*
 * ssd1306.c
 *
 *  Created on: 2016年5月14日
 *      Author: Cocoonshu
 */

#include "cosmart/device/ssd1306.h"
#include "cosmart/log.h"
#include "cosmart/Timer.h"
#include "driver/i2c_master.h"
#include "mem.h"

LOCAL SSD1306Device* mCurrentDevice                  = NULL;
LOCAL SSD1306Device* mDeviceList[SLAVE_DEVICE_COUNT] = {NULL, NULL};

LOCAL bool   writeCMD(SSD1306Device* device, uint8 cmd, uint8 params, bool hasParams);
LOCAL bool   writeDATA(SSD1306Device* device, uint8* data, uint8 dataLength);
LOCAL uint8  getDeviceReadAddress(SSD1306Device* device);
LOCAL uint8  getDeviceWriteAddress(SSD1306Device* device);
LOCAL bool   testDeviceAddress();
LOCAL void   executeDefaultSetup();
LOCAL void   resetDevice(SSD1306Device* device);

void ICACHE_FLASH_ATTR SSD1306_initialize() {
	Log_printfln("");
	Log_printfln("[SSD1306] Start SSD1306 driver");
	Log_printfln("[SSD1306] Start I2C manager in master mode");

	i2c_master_gpio_init();
	Timer_delayMS(500);
	if (testDeviceAddress()) {
		Timer_delayMS(100);
		executeDefaultSetup();
	}
	SSD1306_cleanScreen();
}

int ICACHE_FLASH_ATTR SSD1306_getSlaveDeviceCount() {
	return SLAVE_DEVICE_COUNT;
}

int ICACHE_FLASH_ATTR SSD1306_getValidSlaveDeviceCount() {
	int validDeviceCount = 0;
	int itrator          = 0;
	for (itrator = 0; itrator < SLAVE_DEVICE_COUNT; itrator ++) {
		if (mDeviceList[itrator] != NULL) {
			validDeviceCount ++;
		}
	}
	return validDeviceCount;
}

bool ICACHE_FLASH_ATTR SSD1306_bindDevice(int index) {
	if (index < 0 || index >= SLAVE_DEVICE_COUNT) {
		return false;
	}
	if (mDeviceList[index] == NULL) {
		return false;
	}
	mCurrentDevice = mDeviceList[index];
	return true;
}

void ICACHE_FLASH_ATTR SSD1306_drawPixels(uint16 x, uint16 y, uint16 width, uint16 height, uint8* pixels) {
	if (mCurrentDevice == NULL || mCurrentDevice->address == SSD1306_SLAVE_NO_ADDRESS) {
		return;
	}

	uint8  page          = y / DEVICE_PAGE_COUNT;
	uint16 column        = x;
	uint8  pageLoop      = (y + height) / DEVICE_HEIGHT_PER_PAGE;
	uint16 columnLoop    = x + width;
	uint16 bufferAddress = page * DEVICE_COLUMN_COUNT + column;
	uint8* buffer        = mCurrentDevice->frameBuffer + bufferAddress;

	pageLoop   = pageLoop > DEVICE_PAGE_COUNT ? DEVICE_PAGE_COUNT : pageLoop;
	columnLoop = columnLoop > DEVICE_SEGMENT_COUNT ? DEVICE_SEGMENT_COUNT : columnLoop;

	uint16 w = 0;
	uint16 h = 0;
	for (; page < pageLoop; page ++) {
		writeCMD(mCurrentDevice, CMD_SET_PAGE_POINTER(page), NaN, false);
		writeCMD(mCurrentDevice, CMD_COLUMN_POINTER_LOW(column), NaN, false);
		writeCMD(mCurrentDevice, CMD_COLUMN_POINTER_HIGH(column), NaN, false);

		for (; column < columnLoop; column ++) {


		}
	}
}

void ICACHE_FLASH_ATTR SSD1306_cleanScreen() {
	if (mCurrentDevice == NULL || mCurrentDevice->address == SSD1306_SLAVE_NO_ADDRESS) {
		return;
	}

	uint8  page       = 0;
	uint8  pageLoop   = 8;
	uint16 column     = 0;
	uint16 columnLoop = 128;
	uint8  dummyPixel = 0xAA;

	for (; page < pageLoop; page ++) {
		writeCMD(mCurrentDevice, CMD_SET_PAGE_POINTER(page), NaN, false);
		writeCMD(mCurrentDevice, CMD_COLUMN_POINTER_LOW(column), NaN, false);
		writeCMD(mCurrentDevice, CMD_COLUMN_POINTER_HIGH(column), NaN, false);

		for (; column < columnLoop; column ++) {
			writeDATA(mCurrentDevice, &dummyPixel, 1);
		}
	}
}

void ICACHE_FLASH_ATTR SSD1306_reset() {
	if (mCurrentDevice == NULL) {
		Log_printfln("[SSD1306] device is null, reset failed");
		return;
	}
	if (mCurrentDevice->address == SSD1306_SLAVE_NO_ADDRESS) {
		Log_printfln("[SSD1306] device address is SSD1306_SLAVE_NO_ADDRESS, reset failed");
		return;
	}
	resetDevice(mCurrentDevice);
}

LOCAL bool ICACHE_FLASH_ATTR testDeviceAddress() {
	uint8 ack             = I2C_ACK_OK;
	uint8 currentAddress  = SSD1306_SLAVE_NO_ADDRESS;

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
	i2c_master_writeByte(SSD1306_SLAVE_HIGH_ADDRESS);
	if ((ack = i2c_master_getAck()) == I2C_ACK_OK) {
		i2c_master_stop();
		currentAddress = SSD1306_SLAVE_HIGH_ADDRESS;
		mDeviceList[0] = (SSD1306Device*)os_zalloc(sizeof(SSD1306Device));
		os_memset(mDeviceList[0], 0, sizeof(SSD1306Device));
		mDeviceList[0]->address     = SSD1306_SLAVE_HIGH_ADDRESS;
		mDeviceList[0]->colummSize  = DEVICE_COLUMN_COUNT;
		mDeviceList[0]->segmentSize = DEVICE_SEGMENT_COUNT;
		mDeviceList[0]->pageSize    = DEVICE_PAGE_COUNT;
		mDeviceList[0]->comSize     = DEVICE_COM_COUNT;
		mDeviceList[0]->frameBuffer = (uint8* )os_zalloc(sizeof(uint8) * mDeviceList[0]->pageSize * mDeviceList[0]->colummSize);
		if (mDeviceList[0]->frameBuffer != NULL) {
			os_memset(mDeviceList[0]->frameBuffer, 0, sizeof(uint8) * mDeviceList[0]->pageSize * mDeviceList[0]->colummSize);
		}

		mCurrentDevice = mDeviceList[0];
		Log_printfln("       - Found SSD1306 slave on I2C Bus port 0x%x", SSD1306_SLAVE_HIGH_ADDRESS);
		if (mDeviceList[0]->frameBuffer != NULL) {
			Log_printfln("       - SSD1306 device on 0x%x zalloc framebuffer successes", SSD1306_SLAVE_HIGH_ADDRESS);
		} else {
			Log_printfln("       - SSD1306 device on 0x%x failed to zalloc framebuffer", SSD1306_SLAVE_HIGH_ADDRESS);
		}
		return true;
	}

	// Test Low address
	i2c_master_start();
	i2c_master_writeByte(SSD1306_SLAVE_LOW_ADDRESS);
	if ((ack = i2c_master_getAck()) == I2C_ACK_OK) {
		i2c_master_stop();
		currentAddress = SSD1306_SLAVE_LOW_ADDRESS;
		mDeviceList[1] = (SSD1306Device*)os_zalloc(sizeof(SSD1306Device));
		os_memset(mDeviceList[1], 0, sizeof(SSD1306Device));
		mDeviceList[1]->address     = SSD1306_SLAVE_HIGH_ADDRESS;
		mDeviceList[1]->colummSize  = DEVICE_COLUMN_COUNT;
		mDeviceList[1]->segmentSize = DEVICE_SEGMENT_COUNT;
		mDeviceList[1]->pageSize    = DEVICE_PAGE_COUNT;
		mDeviceList[1]->comSize     = DEVICE_COM_COUNT;
		mDeviceList[1]->frameBuffer = (uint8* )os_zalloc(sizeof(uint8) * mDeviceList[1]->pageSize * mDeviceList[1]->colummSize);
		if (mDeviceList[1]->frameBuffer != NULL) {
			os_memset(mDeviceList[1]->frameBuffer, 0, sizeof(uint8) * mDeviceList[1]->pageSize * mDeviceList[1]->colummSize);
		}

		if (mCurrentDevice == NULL) {
			mCurrentDevice = mDeviceList[1];
		}
		Log_printfln("       - Found SSD1306 slave on I2C Bus port 0x%x", SSD1306_SLAVE_LOW_ADDRESS);
		if (mDeviceList[1]->frameBuffer != NULL) {
			Log_printfln("       - SSD1306 device on 0x%x zalloc framebuffer successes", SSD1306_SLAVE_LOW_ADDRESS);
		} else {
			Log_printfln("       - SSD1306 device on 0x%x failed to zalloc framebuffer", SSD1306_SLAVE_LOW_ADDRESS);
		}
		return true;
	}

	if (currentAddress == SSD1306_SLAVE_NO_ADDRESS) {
		Log_printfln("[SSD1306] No slave device found on I2C bus");
		return false;
	}
}

LOCAL void ICACHE_FLASH_ATTR executeDefaultSetup() {
	int  i         = 0;
	bool isSuccess = false;
	for (i = 0; i < SLAVE_DEVICE_COUNT; i ++) {
		if (mDeviceList[i] != NULL) {
			resetDevice(mDeviceList[i]);
		}
	}
}

LOCAL void ICACHE_FLASH_ATTR resetDevice(SSD1306Device* device) {
	Log_printfln("[SSD1306] device is 0x%x reset to default", device->address);
	writeCMD(device, DEFAULT_SCREEN_OFF);
	writeCMD(device, DEFAULT_MEM_ADDRESSING_MODE);
	writeCMD(device, DEFAULT_DISPLAY_START_LINE);
	writeCMD(device, DEFAULT_DISPLAY_START_LINE_OFFSET);
	writeCMD(device, DEFAULT_PAGE_POINTER);
	writeCMD(device, DEFAULT_COLUMN_POINTER_LOW);
	writeCMD(device, DEFAULT_COLUMN_POINTER_HIGH);
	writeCMD(device, DEFAULT_COM_SCAN_DIRECTION);
	writeCMD(device, DEFAULT_SEGMENT_REMAP);
	writeCMD(device, DEFAULT_CONSTRAST);
	writeCMD(device, DEFAULT_DISPLAY_MODE);
	writeCMD(device, DEFAULT_MULTIPLEX_RATIO);
	writeCMD(device, DEFAULT_COM_CONFIG);
	writeCMD(device, DEFAULT_ALL_PIXEL_OFF);
	writeCMD(device, DEFAULT_START_LINE_OFFSET);
	writeCMD(device, DEFAULT_DISPLAY_CLOCK_DIVIDE_RATIO);
	writeCMD(device, DFFAULT_PRECHARGE_INTERVAL);
	writeCMD(device, DEFAULT_COMH_VOLTAGE);
	writeCMD(device, DEFAULT_DC2DC_CONFIG);
	writeCMD(device, DEFAULT_SCREEN_ON);
//	writeCMD(device, DEFAULT_SCREEN_OFF);
//	writeCMD(device, DFFAULT_PRECHARGE_INTERVAL);
//	writeCMD(device, DEFAULT_COMH_VOLTAGE);
//	writeCMD(device, DEFAULT_DC2DC_CONFIG);
//	writeCMD(device, DEFAULT_SCREEN_ON);
//	writeCMD(device, DEFAULT_ALL_PIXEL_ON);
	Log_printfln("[SSD1306] device is 0x%x reset completed", device->address);
}

LOCAL bool ICACHE_FLASH_ATTR writeCMD(SSD1306Device* device, uint8 cmd, uint8 params, bool hasParam) {
	uint8 paramItr = 0;
	uint8 address  = getDeviceWriteAddress(device);

	i2c_master_start();
	i2c_master_writeByte(address);
#if !ENABLE_FAST_TRANSFER
	if (i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] writeCMD(0x%x) failed, address 0x%x no ACK", cmd, address);
		return false;
	}
#endif

	i2c_master_writeByte(FLAG_COMMAND);
#if !ENABLE_FAST_TRANSFER
	if (i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] writeCMD(0x%x) failed, cmd flag no ACK", cmd);
		return false;
	}
#endif

	i2c_master_writeByte(cmd);
#if !ENABLE_FAST_TRANSFER
	if (i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] writeCMD(0x%x) failed, write byte no ACK", cmd);
		return false;
	}
#endif

	if (hasParam) {
		i2c_master_writeByte(params);
#if !ENABLE_FAST_TRANSFER
		if (i2c_master_checkAck()) {
			i2c_master_stop();
			Log_printfln("[SSD1306] writeCMD(0x%x) failed, write param no ACK", cmd);
			return false;
		}
#endif
	}

	i2c_master_stop();

	return true;
}

LOCAL bool ICACHE_FLASH_ATTR writeDATA(SSD1306Device* device, uint8* data, uint8 dataLength) {
	uint8 paramItr = 0;
	uint8 address  = getDeviceWriteAddress(device);

	i2c_master_start();
	i2c_master_writeByte(address);
#if !ENABLE_FAST_TRANSFER
	if (i2c_master_checkAck()) {
		i2c_master_stop();
		return false;
	}
#endif

	for (paramItr = 0; paramItr < dataLength; paramItr++) {
		i2c_master_writeByte(FLAG_DATA);
#if !ENABLE_FAST_TRANSFER
		if (i2c_master_checkAck()) {
			i2c_master_stop();
			return false;
		}
#endif
		i2c_master_writeByte(*(data + paramItr));
#if !ENABLE_FAST_TRANSFER
		if (i2c_master_checkAck()) {
			i2c_master_stop();
			return false;
		}
#endif
	}
	i2c_master_stop();

	return true;
}

LOCAL uint8 getDeviceReadAddress(SSD1306Device* device) {
	if (device == NULL) {
		return SSD1306_SLAVE_NO_ADDRESS;
	}
	if (device->address == SSD1306_SLAVE_NO_ADDRESS) {
		return SSD1306_SLAVE_NO_ADDRESS;
	} else {
		return device->address | 1;
	}
}

LOCAL uint8 getDeviceWriteAddress(SSD1306Device* device) {
	if (device == NULL) {
		return SSD1306_SLAVE_NO_ADDRESS;
	}
	if (device->address == SSD1306_SLAVE_NO_ADDRESS) {
		return SSD1306_SLAVE_NO_ADDRESS;
	} else {
		return device->address | 0;
	}
}
