/*
 * gpiomanager.c
 *
 *  Created on: 2016年3月20日
 *      Author: Cocoonshu
 */
#include "cosmart/pwmmanager.h"
#include "cosmart/log.h"
#include "os_type.h"
#include "mem.h"

#define PWM_FILL_MAP_ITEM_(var, x) {(var)[0] = PWM_REG_(x);(var)[0] = PWM_MUX_(x);(var)[0] = PWM_PIN_(x);}

bool   mPWMPortRegisterList[GPIO_PIN_COUNT];
uint32 mPWMDutyMap[GPIO_PIN_COUNT];
uint32 (*mPWMSetupMap)[PWM_SETUP_ITEM_COUNT];
uint32 mPWMChannelSize = 0;
bool   mHasSetuped = false;

void ICACHE_FLASH_ATTR PWM_initialize() {
	Log_printfln("");
	Log_printfln("[PWM] Start PMW manager");
	Log_printfln("       - Setup PWM period to %d us", DEFAULT_PERIOD);

	// Initialize
	os_memset(mPWMPortRegisterList, false, GPIO_PIN_COUNT);
	os_memset(mPWMDutyMap, 0, GPIO_PIN_COUNT * sizeof(uint32));
}

bool ICACHE_FLASH_ATTR PWM_isInitialized() {
	return mHasSetuped;
}

void ICACHE_FLASH_ATTR PWM_registerPWMPort(int ioPort) {
	if (mHasSetuped || ioPort < 0 || ioPort >= GPIO_PIN_COUNT) {
		return;
	} else {
		mPWMPortRegisterList[ioPort] = true;
		mPWMChannelSize = PWM_getPWMSize();
	}
}

void ICACHE_FLASH_ATTR PWM_unregisterPWMPort(int ioPort) {
	if (mHasSetuped || ioPort < 0 || ioPort >= GPIO_PIN_COUNT) {
		return;
	} else {
		mPWMPortRegisterList[ioPort] = false;
		mPWMChannelSize = PWM_getPWMSize();
	}
}

uint32* ICACHE_FLASH_ATTR PWM_getPWMMapItem(int gpioPort) {
	uint32* item = (uint32*)os_zalloc(sizeof(uint32) * PWM_SETUP_ITEM_COUNT);
	switch (gpioPort) {
		case 0x0:{ item[0] = PWM_REG_00; item[1] = PWM_MUX_00; item[2] = PWM_PIN_00; } break;
		case 0x1:{ item[0] = PWM_REG_01; item[1] = PWM_MUX_01; item[2] = PWM_PIN_01; } break;
		case 0x2:{ item[0] = PWM_REG_02; item[1] = PWM_MUX_02; item[2] = PWM_PIN_02; } break;
		case 0x3:{ item[0] = PWM_REG_03; item[1] = PWM_MUX_03; item[2] = PWM_PIN_03; } break;
		case 0x4:{ item[0] = PWM_REG_04; item[1] = PWM_MUX_04; item[2] = PWM_PIN_04; } break;
		case 0x5:{ item[0] = PWM_REG_05; item[1] = PWM_MUX_05; item[2] = PWM_PIN_05; } break;
		case 0x6:{ item[0] = PWM_REG_06; item[1] = PWM_MUX_06; item[2] = PWM_PIN_06; } break;
		case 0x7:{ item[0] = PWM_REG_07; item[1] = PWM_MUX_07; item[2] = PWM_PIN_07; } break;
		case 0x8:{ item[0] = PWM_REG_08; item[1] = PWM_MUX_08; item[2] = PWM_PIN_08; } break;
		case 0x9:{ item[0] = PWM_REG_09; item[1] = PWM_MUX_09; item[2] = PWM_PIN_09; } break;
		case 0xA:{ item[0] = PWM_REG_10; item[1] = PWM_MUX_10; item[2] = PWM_PIN_10; } break;
		case 0xB:{ item[0] = PWM_REG_11; item[1] = PWM_MUX_11; item[2] = PWM_PIN_11; } break;
		case 0xC:{ item[0] = PWM_REG_12; item[1] = PWM_MUX_12; item[2] = PWM_PIN_12; } break;
		case 0xD:{ item[0] = PWM_REG_13; item[1] = PWM_MUX_13; item[2] = PWM_PIN_13; } break;
		case 0xE:{ item[0] = PWM_REG_14; item[1] = PWM_MUX_14; item[2] = PWM_PIN_14; } break;
		case 0xF:{ item[0] = PWM_REG_15; item[1] = PWM_MUX_15; item[2] = PWM_PIN_15; } break;
	}
	return item;
}

void ICACHE_FLASH_ATTR PWM_setup()  {
	mHasSetuped  = true;
	int pinItr   = 0;
	int validItr = 0;

	// Generate Setup Map
	mPWMChannelSize = PWM_getPWMSize();
	mPWMSetupMap = (uint32(*)[PWM_SETUP_ITEM_COUNT])os_zalloc(mPWMChannelSize * PWM_SETUP_ITEM_COUNT * sizeof(uint32));
	os_memset(mPWMSetupMap, 0, mPWMChannelSize * PWM_SETUP_ITEM_COUNT * sizeof(uint32));
	for (pinItr = 0, validItr = 0; pinItr < GPIO_PIN_COUNT && validItr < mPWMChannelSize; pinItr++) {
		if (mPWMPortRegisterList[pinItr]) {
			uint32* item = PWM_getPWMMapItem(pinItr);
			int elemItr = 0;
			for (elemItr = 0; elemItr < PWM_SETUP_ITEM_COUNT; elemItr++) {
				mPWMSetupMap[validItr][elemItr] = item[elemItr];
			}
			validItr++;
		}
	}

	// Initialize system PWM
	set_pwm_debug_en(false);
	pwm_init(DEFAULT_PWM_PERIOD, mPWMPortRegisterList, mPWMChannelSize, mPWMSetupMap);

	// Setup PWM initialization
	for (pinItr = 0, validItr = 0; pinItr < GPIO_PIN_COUNT && validItr < mPWMChannelSize; pinItr++) {
		if (mPWMPortRegisterList[pinItr]) {
			pwm_set_duty(mPWMDutyMap[pinItr], validItr);
			Log_printfln("       - Initialize GPIO_%d as PWM, channel #%d, value %d",
					      pinItr, validItr, mPWMDutyMap[pinItr]);
			validItr++;
		}
	}

	// Launch PWM
	pwm_start();
}

void ICACHE_FLASH_ATTR PWM_apply()  {
	pwm_start();
}

void ICACHE_FLASH_ATTR PWM_setPWMDutyLevel(int gpioPin, uint32 dutyLevel) {
	uint32 channel     = -1;
	uint32 itr         = 0;
	uint32 valueLength = DEFAULT_PWM_MAX_DUTY - DEFAULT_PWM_MIN_DUTY;
	float  factor      = dutyLevel / 1024.0f;
	uint32 realDuty    = factor * valueLength + DEFAULT_PWM_MIN_DUTY;
	for (itr = 0; itr < mPWMChannelSize; itr++) {
		if (gpioPin == mPWMSetupMap[itr][GPIO_PIN]) {
			channel = itr;
			break;
		}
	}
	pwm_set_duty(realDuty, channel);
}

uint32 ICACHE_FLASH_ATTR PWM_getPWMDutyLevel(int gpioPin) {
	uint32 channel     = -1;
	uint32 itr         = 0;
	uint32 valueLength = DEFAULT_PWM_MAX_DUTY - DEFAULT_PWM_MIN_DUTY;
	for (itr = 0; itr < mPWMChannelSize; itr++) {
		if (gpioPin == mPWMSetupMap[itr][GPIO_PIN]) {
			channel = itr;
			break;
		}
	}
	if (channel == -1) {
		return -1;
	} else {
		uint32 realDuty = pwm_get_duty(channel);
		uint32 duty     = (realDuty - DEFAULT_PWM_MIN_DUTY) / valueLength;
		return duty * 1024;
	}
}

uint32 ICACHE_FLASH_ATTR PWM_getPWMSize() {
	int i = 0;
	uint32 channelSize = 0;
	for (i = 0; i < GPIO_PIN_COUNT; i++) {
		if (mPWMPortRegisterList[i]) {
			channelSize++;
		}
	}
	return channelSize;
}

uint32 ICACHE_FLASH_ATTR PWM_getMaxPWMDuty() {
	return DEFAULT_PWM_MAX_DUTY;
}

uint32 ICACHE_FLASH_ATTR PWM_getMinPWMDuty() {
	return DEFAULT_PWM_MIN_DUTY;
}

uint32 ICACHE_FLASH_ATTR PWM_getPeriod() {
	return DEFAULT_PWM_PERIOD;
}
