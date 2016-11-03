/*
 * led.c
 *
 *  Created on: 2016年3月20日
 *      Author: Cocoonshu
 */

#ifndef APP_COSMART_DEVICE_LED_C_
#define APP_COSMART_DEVICE_LED_C_

#include "cosmart/device/led.h"
#include "cosmart/log.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include <math.h>

os_timer_t mLedTimer;
uint32*    mLedIOPortList         = NULL;
bool*      mLedBreathEnabledList  = NULL;
uint32*    mLedBreathDurationList = NULL;
uint32*    mLedBreathTimeList     = NULL;
int        mLedSize               = 0;
uint64     mLedBreathTime         = 0;

void ICACHE_FLASH_ATTR LED_processLEDBreathing(void* data);

void ICACHE_FLASH_ATTR LED_initialize(const uint32 ledIOPort[], int length) {
	LED_initializeAdvance(ledIOPort, 0, length);
}

void ICACHE_FLASH_ATTR LED_initializeAdvance(const uint32 ledIOPort[], int offset, int length) {
	Log_printfln("");
	Log_printfln("[LED] Start LED device service");

	int i = 0;
	mLedIOPortList         = (uint32*)os_zalloc(sizeof(uint32) * length);
	mLedBreathDurationList = (uint32*)os_zalloc(sizeof(uint32) * length);
	mLedBreathTimeList     = (uint32*)os_zalloc(sizeof(uint32) * length);
	mLedBreathEnabledList  = (bool*)  os_zalloc(sizeof(bool) * length);
	os_memset(mLedIOPortList,         0,     sizeof(uint32) * length);
	os_memset(mLedBreathDurationList, 0,     sizeof(uint32) * length);
	os_memset(mLedBreathTimeList,     0,     sizeof(uint32) * length);
	os_memset(mLedBreathEnabledList,  false, sizeof(bool) * length);

	// Copy input parameters to internal
	int loop = 0;
	for (i = 0, loop = offset; i < length && loop < offset + length; i++, loop++) {
		mLedIOPortList[i] = ledIOPort[loop];
		Log_printfln("       - Setup GPIO_%d as LED_%d port", ledIOPort[loop], i);
	}
	mLedSize = length;

	// Register Led Port list to GPIO controller
	for (i = 0; i < mLedSize; i++) {
		PWM_registerPWMPort(mLedIOPortList[i]);
	}
}

void ICACHE_FLASH_ATTR LED_setBrightness(int index, uint32 brightness) {
	if (index < 0 || index >= mLedSize) {
		return;
	} else {
		PWM_setPWMDutyLevel(mLedIOPortList[index], brightness);
	}
}

uint32 ICACHE_FLASH_ATTR LED_getBrightness(int index) {
	if (index < 0 || index >= mLedSize) {
		return -1;
	} else {
		return PWM_getPWMDutyLevel(mLedIOPortList[index]);
	}
}

int ICACHE_FLASH_ATTR LED_getSize() {
	return mLedSize;
}

uint32 ICACHE_FLASH_ATTR LED_getIOPort(int index) {
	if (index < 0 || index >= mLedSize) {
		return -1;
	} else {
		return mLedIOPortList[index];
	}
}

void ICACHE_FLASH_ATTR LED_turnOn(int index) {
	LED_setBrightness(index, 1);
}

void ICACHE_FLASH_ATTR LED_turnOff(int index) {
	LED_setBrightness(index, 0);
}

void ICACHE_FLASH_ATTR LED_turnOnAll() {
	int i = 0;
	for (i = 0; i < mLedSize; i++) {
		PWM_setPWMDutyLevel(mLedIOPortList[i], 1);
	}
}

void ICACHE_FLASH_ATTR LED_turnOffAll() {
	int i = 0;
	for (i = 0; i < mLedSize; i++) {
		PWM_setPWMDutyLevel(mLedIOPortList[i], 0);
	}
}

void ICACHE_FLASH_ATTR LED_startBreathing(int index) {
	if (index < 0 || index >= mLedSize) {
		return;
	} else {
		Log_printfln("[LED] Start LED_%d(GPIO_%d) breathing, period is %dms",
				index, mLedIOPortList[index], DEFAULT_LED_PERIOD);
		mLedBreathDurationList[index] = DEFAULT_LED_PERIOD;
		mLedBreathEnabledList[index] = true;
		LED_processLEDBreathing(NULL);
		// TODO
	}
}

void ICACHE_FLASH_ATTR LED_startBreathingWithDuration(int index, int duration) {
	if (index < 0 || index >= mLedSize) {
		return;
	} else {
		Log_printfln("[LED] Start LED_%d(GPIO_%d) breathing, period is %dms",
				index, mLedIOPortList[index], duration);
		mLedBreathDurationList[index] = duration;
		mLedBreathTimeList[index] = 0;
		mLedBreathEnabledList[index] = true;
		LED_processLEDBreathing(NULL);
	}
}

void ICACHE_FLASH_ATTR LED_startBreathingWithDurationAndDelay(int index, int duration, int delay) {
	if (index < 0 || index >= mLedSize) {
		return;
	} else {
		Log_printfln("[LED] Start LED_%d(GPIO_%d) breathing, period is %dms, delay %dms",
				index, mLedIOPortList[index], duration, delay);
		mLedBreathDurationList[index] = duration;
		mLedBreathTimeList[index] = delay;
		mLedBreathEnabledList[index] = true;
		LED_processLEDBreathing(NULL);
	}
}

void ICACHE_FLASH_ATTR LED_stopBreathing(int index) {
	if (index < 0 || index >= mLedSize) {
		return;
	} else {
		Log_printfln("[LED] Stop LED_%d(GPIO_%d) breathing", index, mLedIOPortList[index]);
		mLedBreathDurationList[index] = 0;
		mLedBreathTimeList[index] = 0;
		mLedBreathEnabledList[index] = false;
		LED_processLEDBreathing(NULL);
	}
}

void ICACHE_FLASH_ATTR LED_startBurstFlashing(int redLedIndex, int blueLedIndex) {
	Log_printfln("[LED] Start burst flashing in RedLED_%d(GPIO_%d) and BlueLED_%d(GPIO_%d)",
			redLedIndex, mLedIOPortList[redLedIndex], blueLedIndex, mLedIOPortList[blueLedIndex]);
	Log_printfln("[LED] Burst flashing is not implemented yet!!!");
}

void ICACHE_FLASH_ATTR LED_stopBurstFlashing() {
	Log_printfln("[LED] Stop burst flashing");
	Log_printfln("[LED] Burst flashing is not implemented yet!!!");
}

void ICACHE_FLASH_ATTR LED_processLEDBreathing(void* data) {
	int  i             = 0;
	bool hasMoreFrames = true;
	for (i = 0; i < mLedSize; i++) {
		if (mLedBreathEnabledList[i]) {
			mLedBreathTimeList[i] += DEFAULT_LED_UPDATE_INTERVAL;
			float  time      = mLedBreathTimeList[i];
			float  duration  = mLedBreathDurationList[i];
			float  modTime   = mLedBreathTimeList[i] % mLedBreathDurationList[i];
			float  sinFactor = modTime / duration;
			float  sinDuty   = 0.5f * (-cosf(sinFactor * M_TWOPI) + 1.0f);
			uint32 realDuty  = sinDuty * 1024;
			PWM_setPWMDutyLevel(mLedIOPortList[i], realDuty);
		} else {
			PWM_setPWMDutyLevel(mLedIOPortList[i], 0);
		}
	}
	PWM_apply();

	// Re-setup timer
	system_soft_wdt_feed();
	os_timer_disarm(&mLedTimer);
	if (hasMoreFrames) {
		mLedBreathTime += DEFAULT_LED_UPDATE_INTERVAL;
		os_timer_setfn(&mLedTimer, (os_timer_func_t *)LED_processLEDBreathing, NULL);
		os_timer_arm(&mLedTimer, DEFAULT_LED_UPDATE_INTERVAL, false);
	} else {
		mLedBreathTime = 0;
	}
}

#endif /* APP_COSMART_DEVICE_LED_C_ */
