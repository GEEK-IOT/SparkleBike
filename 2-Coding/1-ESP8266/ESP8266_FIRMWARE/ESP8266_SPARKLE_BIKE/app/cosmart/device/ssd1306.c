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
#include "math.h"

//uint8 ASCII [14][7] = {
//		{3, 8, 0x08, 0x08, 0x08}, // 2D -
//		{1, 8, 0x40}, // 2E .
//		{0, 0}, // 2F /
//		{5, 8, 0x3E, 0x41, 0x41, 0x41, 0x3E}, // 30 0
//		{5, 8, 0x00, 0x42, 0x7F, 0x40, 0x00}, // 31 1
//		{5, 8, 0x42, 0x61, 0x51, 0x49, 0x46}, // 32 2
//		{5, 8, 0x22, 0x41, 0x49, 0x49, 0x36}, // 33 3
//		{5, 8, 0x18, 0x14, 0x12, 0x7F, 0x10}, // 34 4
//		{5, 8, 0x27, 0x49, 0x49, 0x49, 0x31}, // 35 5
//		{5, 8, 0x38, 0x4C, 0x4A, 0x49, 0x30}, // 36 6
//		{5, 8, 0x01, 0x01, 0x71, 0x0D, 0x03}, // 37 7
//		{5, 8, 0x36, 0x49, 0x49, 0x49, 0x36}, // 38 8
//		{5, 8, 0x06, 0x49, 0x29, 0x19, 0x0E}, // 39 9
//		{1, 8, 0x24} // 3A :
//};

LOCAL SSD1306Device* mCurrentDevice                  = NULL;
LOCAL SSD1306Device* mDeviceList[SLAVE_DEVICE_COUNT] = {NULL, NULL};

LOCAL bool   beginCMD(SSD1306Device* device);
LOCAL bool   writeCMD_1(SSD1306Device* device, uint8 cmdA);
LOCAL bool   writeCMD_2(SSD1306Device* device, uint8 cmdA, uint8 cmdB);
LOCAL bool   writeCMD_3(SSD1306Device* device, uint8 cmdA, uint8 cmdB, uint8 cmdC);
LOCAL bool   endCMD(SSD1306Device* device);
LOCAL bool   beginRAM(SSD1306Device* device, uint8 colStart, uint8 colEnd, uint8 pageStart, uint8 pageEnd);
LOCAL bool   writeRAM(SSD1306Device* device, uint8* data, uint16 dataLength);
LOCAL bool   endRAM(SSD1306Device* device);
LOCAL bool   flushFrameBuffer(SSD1306Device* device);
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
	Timer_delayMS(50);
	if (testDeviceAddress()) {
		executeDefaultSetup();
	}
	Timer_delayMS(50);
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

void ICACHE_FLASH_ATTR SSD1306_drawPixels(uint16 x, uint16 y, uint16 width, uint16 height, const uint8* pixels) {
	if (mCurrentDevice == NULL || mCurrentDevice->address == SSD1306_SLAVE_NO_ADDRESS) {
		return;
	}

	uint16 startColumn = x;
	uint16 endColumn   = x + width - 1;
	uint16 startPage   = y / DEVICE_HEIGHT_PER_PAGE;
	uint16 endPage     = (y + height - 1) / DEVICE_HEIGHT_PER_PAGE;


	// TODO
}

void ICACHE_FLASH_ATTR SSD1306_drawBuffer(uint16 startColumn, uint16 endColumn, uint16 startPage, uint16 endPage, const uint8* buffer) {
	if (mCurrentDevice == NULL || mCurrentDevice->address == SSD1306_SLAVE_NO_ADDRESS) {
		return;
	}

	uint16 page;
	for (page = startPage; page <= endPage; page++) {
		uint16 strip = endColumn - startColumn + 1;
		uint8*       dest  = mCurrentDevice->frameBuffer + (startColumn + page * strip);
		const uint8* src   = buffer + (page - startPage) * strip;
		os_memcpy(dest, src, strip);
	}
	flushFrameBuffer(mCurrentDevice);
}

void ICACHE_FLASH_ATTR SSD1306_drawText(char* text, uint16 x, uint16 y) {
	if (mCurrentDevice == NULL || mCurrentDevice->address == SSD1306_SLAVE_NO_ADDRESS) {
		return;
	}

	bool   isDirty        = false;
	char*  letter         = text;
	uint16 currentAddress = x + (y / DEVICE_HEIGHT_PER_PAGE) * DEVICE_COLUMN_COUNT;

	while (letter != NULL && *letter != '\0') {
		char  ascii    = *letter;
		uint8 asciiPos = ascii - 0x00;//0x2D;
		if (asciiPos < 0 || asciiPos >= 128) {
			letter++;
			continue;
		}

		uint8 pixelWidth  = ASCII[asciiPos][0];
		uint8 pixelHeight = ASCII[asciiPos][1];

		system_soft_wdt_feed();
		if (pixelWidth != 0 && pixelHeight != 0) {
			int i, j;
			int column    = pixelWidth > (DEVICE_COLUMN_COUNT - x) ? (DEVICE_COLUMN_COUNT - x): pixelWidth;
			int page      = floor(pixelHeight / DEVICE_COM_COUNT);
			int startPage = y / DEVICE_HEIGHT_PER_PAGE;
			for (j = 0; j < page; j++) {
				for (i = 0; i < column; i++) {
					int bufferIndex = (startPage + j) * DEVICE_COLUMN_COUNT + i;
					int asciiIndex  = 2 + j * pixelWidth + i;
					mCurrentDevice->frameBuffer[currentAddress + bufferIndex] = ASCII[ascii][asciiIndex];
					isDirty = true;
				}
			}

			currentAddress += column * page + 1;
		}

		letter++;
	}

	if (isDirty) {
		system_soft_wdt_feed();
		flushFrameBuffer(mCurrentDevice);
	}
}

void ICACHE_FLASH_ATTR SSD1306_cleanScreen() {
	if (mCurrentDevice == NULL || mCurrentDevice->address == SSD1306_SLAVE_NO_ADDRESS) {
		return;
	}

	uint8 pixel = 0x00;
	Log_printfln("[SSD1306] device is 0x%x begin clean screen", mCurrentDevice->address);
	os_memset(mCurrentDevice->frameBuffer, pixel, DEVICE_COLUMN_COUNT * DEVICE_PAGE_COUNT);
	flushFrameBuffer(mCurrentDevice);
	Log_printfln("[SSD1306] device is 0x%x clean screen finished", mCurrentDevice->address);
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
		mDeviceList[0] = (SSD1306Device*)os_malloc(sizeof(SSD1306Device));
		os_memset(mDeviceList[0], 0, sizeof(SSD1306Device));
		mDeviceList[0]->address     = SSD1306_SLAVE_HIGH_ADDRESS;
		mDeviceList[0]->colummSize  = DEVICE_COLUMN_COUNT;
		mDeviceList[0]->segmentSize = DEVICE_SEGMENT_COUNT;
		mDeviceList[0]->pageSize    = DEVICE_PAGE_COUNT;
		mDeviceList[0]->comSize     = DEVICE_COM_COUNT;
		mDeviceList[0]->frameBuffer = (uint8*)os_malloc(sizeof(uint8) * mDeviceList[0]->pageSize * mDeviceList[0]->colummSize);
		if (mDeviceList[0]->frameBuffer != NULL) {
			os_memset(mDeviceList[0]->frameBuffer, 0, sizeof(uint8) * mDeviceList[0]->pageSize * mDeviceList[0]->colummSize);
		}

		mCurrentDevice = mDeviceList[0];
		Log_printfln("       - Found SSD1306 slave on I2C Bus port 0x%x", SSD1306_SLAVE_HIGH_ADDRESS);
		if (mDeviceList[0]->frameBuffer != NULL) {
			Log_printfln("       - SSD1306 device on 0x%x malloc framebuffer successes", SSD1306_SLAVE_HIGH_ADDRESS);
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
		mDeviceList[1] = (SSD1306Device*)os_malloc(sizeof(SSD1306Device));
		os_memset(mDeviceList[1], 0, sizeof(SSD1306Device));
		mDeviceList[1]->address     = SSD1306_SLAVE_HIGH_ADDRESS;
		mDeviceList[1]->colummSize  = DEVICE_COLUMN_COUNT;
		mDeviceList[1]->segmentSize = DEVICE_SEGMENT_COUNT;
		mDeviceList[1]->pageSize    = DEVICE_PAGE_COUNT;
		mDeviceList[1]->comSize     = DEVICE_COM_COUNT;
		mDeviceList[1]->frameBuffer = (uint8* )os_malloc(sizeof(uint8) * mDeviceList[1]->pageSize * mDeviceList[1]->colummSize);
		if (mDeviceList[1]->frameBuffer != NULL) {
			os_memset(mDeviceList[1]->frameBuffer, 0, sizeof(uint8) * mDeviceList[1]->pageSize * mDeviceList[1]->colummSize);
		}

		if (mCurrentDevice == NULL) {
			mCurrentDevice = mDeviceList[1];
		}
		Log_printfln("       - Found SSD1306 slave on I2C Bus port 0x%x", SSD1306_SLAVE_LOW_ADDRESS);
		if (mDeviceList[1]->frameBuffer != NULL) {
			Log_printfln("       - SSD1306 device on 0x%x malloc framebuffer successes", SSD1306_SLAVE_LOW_ADDRESS);
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
	beginCMD(device);
	writeCMD_1(device, SSD1306_DISPLAYOFF);
	writeCMD_2(device, SSD1306_SETDISPLAYCLOCKDIV, 0x80);
	writeCMD_2(device, SSD1306_SETMULTIPLEX,       0x3F);
	writeCMD_2(device, SSD1306_SETDISPLAYOFFSET,   0x00);
	writeCMD_1(device, SSD1306_STARTLINE(0x00));
	writeCMD_2(device, SSD1306_CHARGEPUMP,         SSD1306_CHARGEPUMP_ENABLE);
	writeCMD_2(device, SSD1306_MEMORYMODE,         SSD1306_MEMORYMODE_HORIZONTAL);
	writeCMD_1(device, SSD1306_INVERTSEGREMAP);
	writeCMD_1(device, SSD1306_COMSCANDEC);
	writeCMD_2(device, SSD1306_SETCOMPINS,         0x12);
	writeCMD_2(device, SSD1306_SETCONTRAST,        0xCF);
	writeCMD_2(device, SSD1306_SETPRECHARGE,       0xF1);
	writeCMD_2(device, SSD1306_SETVCOMDETECT,      0x40);
	writeCMD_1(device, SSD1306_DEACTIVESCROLL);
	writeCMD_1(device, SSD1306_PIXELOFF);
	writeCMD_1(device, SSD1306_NORMALDISPLAY);
	writeCMD_1(device, SSD1306_DISPLAYON);
	endCMD(device);
	Log_printfln("[SSD1306] device is 0x%x reset completed", device->address);
}

LOCAL bool ICACHE_FLASH_ATTR beginCMD(SSD1306Device* device) {
	uint8 address = getDeviceWriteAddress(device);

	i2c_master_start();
	i2c_master_writeByte(address);
#if !ENABLE_FAST_TRANSFER
	if (!i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] device 0x%x no ACK", device->address);
		return false;
	}
#endif

	i2c_master_writeByte(FLAG_COMMAND);
#if !ENABLE_FAST_TRANSFER
	if (!i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] device 0x%x no ACK", device->address);
		return false;
	}
#endif
}

LOCAL bool ICACHE_FLASH_ATTR writeCMD_1(SSD1306Device* device, uint8 cmdA) {
	i2c_master_writeByte(cmdA);
#if !ENABLE_FAST_TRANSFER
	if (!i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] device 0x%x no ACK", device->address);
		return false;
	}
#endif
	return true;
}

LOCAL bool ICACHE_FLASH_ATTR writeCMD_2(SSD1306Device* device, uint8 cmdA, uint8 cmdB) {
	i2c_master_writeByte(cmdA);
#if !ENABLE_FAST_TRANSFER
	if (!i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] device 0x%x no ACK", device->address);
		return false;
	}
#endif

	i2c_master_writeByte(cmdB);
#if !ENABLE_FAST_TRANSFER
	if (!i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] device 0x%x no ACK", device->address);
		return false;
	}
#endif

	return true;
}

LOCAL bool ICACHE_FLASH_ATTR writeCMD_3(SSD1306Device* device, uint8 cmdA, uint8 cmdB, uint8 cmdC) {
	i2c_master_writeByte(cmdA);
#if !ENABLE_FAST_TRANSFER
	if (!i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] device 0x%x no ACK", device->address);
		return false;
	}
#endif

	i2c_master_writeByte(cmdB);
#if !ENABLE_FAST_TRANSFER
	if (!i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] device 0x%x no ACK", device->address);
		return false;
	}
#endif

	i2c_master_writeByte(cmdC);
#if !ENABLE_FAST_TRANSFER
	if (!i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] device 0x%x no ACK", device->address);
		return false;
	}
#endif

	return true;
}

LOCAL bool ICACHE_FLASH_ATTR endCMD(SSD1306Device* device) {
	i2c_master_stop();
	return true;
}

LOCAL bool ICACHE_FLASH_ATTR beginRAM(SSD1306Device* device, uint8 colStart, uint8 colEnd, uint8 pageStart, uint8 pageEnd) {
	beginCMD(device);
	writeCMD_3(device, SSD1306_COLUMNRANGE, colStart,  colEnd);
	writeCMD_3(device, SSD1306_PAGERANGE,   pageStart, pageEnd);
	endCMD(device);
	Timer_delayMS(10);

	uint8 address = getDeviceWriteAddress(device);

	i2c_master_start();
	i2c_master_writeByte(address);
#if !ENABLE_FAST_TRANSFER
	if (!i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] device 0x%x no ACK", device->address);
		return false;
	}
#endif

	i2c_master_writeByte(FLAG_DATA);
#if !ENABLE_FAST_TRANSFER
	if (!i2c_master_checkAck()) {
		i2c_master_stop();
		Log_printfln("[SSD1306] device 0x%x no ACK", device->address);
		return false;
	}
#endif
}

LOCAL bool ICACHE_FLASH_ATTR writeRAM(SSD1306Device* device, uint8* data, uint16 dataLength) {
	uint16 paramItr = 0;
	uint8  address  = getDeviceWriteAddress(device);

	for (paramItr = 0; paramItr < dataLength; paramItr++) {
		i2c_master_writeByte(*(data + paramItr));
#if !ENABLE_FAST_TRANSFER
		if (!i2c_master_checkAck()) {
			i2c_master_stop();
			Log_printfln("[SSD1306] device 0x%x no ACK", device->address);
			return false;
		}
#endif
	}

	return true;
}

LOCAL bool ICACHE_FLASH_ATTR endRAM(SSD1306Device* device) {
	i2c_master_stop();
}

LOCAL bool ICACHE_FLASH_ATTR flushFrameBuffer(SSD1306Device* device) {
	beginRAM(mCurrentDevice, 0x00, DEVICE_COLUMN_COUNT - 1, 0x00, DEVICE_PAGE_COUNT - 1);
	writeRAM(mCurrentDevice, mCurrentDevice->frameBuffer, DEVICE_COLUMN_COUNT * DEVICE_PAGE_COUNT);
	endRAM(mCurrentDevice);
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
