/*
 * gpiocontroller.c
 *
 *  Created on: 2016年1月18日
 *      Author: 80074591
 */

#include "cosmart/gpiomanager.h"
#include "cosmart/log.h"
#include "osapi.h"
#include "os_type.h"
#include <math.h>

/**
 * Reference from http://www.esp8266.com/wiki/doku.php?id=esp8266_gpio_registers
 */
extern volatile uint32_t PIN_IN;
extern volatile uint32_t PIN_OUT;
extern volatile uint32_t PIN_OUT_SET;
extern volatile uint32_t PIN_OUT_CLEAR;
extern volatile uint32_t PIN_DIR;
extern volatile uint32_t PIN_DIR_OUTPUT;
extern volatile uint32_t PIN_DIR_INPUT;

uint32 mGPIOInputBitsSet     = 0x00000000;
uint32 mGPIOOutputBitsSet    = 0x00000000;
uint32 mGPIOHighLevelBitsSet = 0x00000000;
uint32 mGPIOLowLevelBitsSet  = 0x00000000;

uint32 ICACHE_FLASH_ATTR convertToGPIOBitValue(uint32 gpioPin);

void ICACHE_FLASH_ATTR GPIO_initialize() {
	Log_printfln("");
	Log_printfln("[GPIO] Start GPIO manager");

	gpio_init();
}

void ICACHE_FLASH_ATTR GPIO_setLevel(uint32 gpioPin, GPIOLevel level) {
	if (!GPIO_ID_IS_PIN_REGISTER(gpioPin)) {
		return;
	}

	// Rebuild gpio configurations
	uint32 gpioBitValue = convertToGPIOBitValue(gpioPin);
	if (level == LowLevel) {
		mGPIOLowLevelBitsSet  |= gpioBitValue;
		mGPIOHighLevelBitsSet ^= gpioBitValue;
	} else if (level == HighLevel) {
		mGPIOLowLevelBitsSet  ^= gpioBitValue;
		mGPIOHighLevelBitsSet |= gpioBitValue;
	} else {
		mGPIOLowLevelBitsSet  ^= gpioBitValue;
		mGPIOHighLevelBitsSet ^= gpioBitValue;
	}

	// Update gpio configurations
	gpio_output_set(
			mGPIOHighLevelBitsSet, mGPIOLowLevelBitsSet,
			mGPIOOutputBitsSet, mGPIOInputBitsSet);
}

void ICACHE_FLASH_ATTR GPIO_setIOMode(uint32 gpioPin, GPIOIOMode ioMode) {
	if (!GPIO_ID_IS_PIN_REGISTER(gpioPin)) {
		return;
	}

	// Rebuild gpio configurations
	uint32 gpioBitValue = convertToGPIOBitValue(gpioPin);
	if (ioMode == InputMode) {
		mGPIOLowLevelBitsSet  ^= gpioBitValue;
		mGPIOHighLevelBitsSet ^= gpioBitValue;
		mGPIOInputBitsSet     |= gpioBitValue;
		mGPIOOutputBitsSet    ^= gpioBitValue;
	} else if (ioMode == OutputMode) {
		mGPIOInputBitsSet     ^= gpioBitValue;
		mGPIOOutputBitsSet    |= gpioBitValue;
	} else if (ioMode == InputAndOutputMode) {
		mGPIOInputBitsSet     |= gpioBitValue;
		mGPIOOutputBitsSet    |= gpioBitValue;
	} else {
		mGPIOLowLevelBitsSet  ^= gpioBitValue;
		mGPIOHighLevelBitsSet ^= gpioBitValue;
		mGPIOInputBitsSet     ^= gpioBitValue;
		mGPIOOutputBitsSet    ^= gpioBitValue;
	}

	// Update gpio configurations
	gpio_output_set(
			mGPIOHighLevelBitsSet, mGPIOLowLevelBitsSet,
			mGPIOOutputBitsSet, mGPIOInputBitsSet);
}

GPIOIOMode ICACHE_FLASH_ATTR GPIO_getIOMode(uint32 gpioPin) {
	if (!GPIO_ID_IS_PIN_REGISTER(gpioPin)) {
		return UnknownIOMode;
	}

	uint32 gpioBitValue = convertToGPIOBitValue(gpioPin);
	bool   hasInput     = (mGPIOInputBitsSet & gpioBitValue) == gpioBitValue;
	bool   hasOutput    = (mGPIOOutputBitsSet & gpioBitValue) == gpioBitValue;
	if (hasInput || hasOutput) {
		if (hasInput && hasOutput) {
			return InputAndOutputMode;
		} else if (hasInput) {
			return InputMode;
		} else {
			return OutputMode;
		}
	} else {
		return UnknownIOMode;
	}
}

GPIOLevel ICACHE_FLASH_ATTR GPIO_getLevel(uint32 gpioPin) {
	if (!GPIO_ID_IS_PIN_REGISTER(gpioPin)) {
		return UnknownIOMode;
	}

	uint32 gpioBitValue = convertToGPIOBitValue(gpioPin);
	bool   isLowLevel   = (mGPIOLowLevelBitsSet & gpioBitValue) == gpioBitValue;
	bool   isHighLevel  = (mGPIOHighLevelBitsSet & gpioBitValue) == gpioBitValue;
	if (isLowLevel || isHighLevel) {
		// if isLowLevel && isHighLevel, the result is high level
		// so just judge isHighLevel is enough.
		if (isHighLevel) {
			return HighLevel;
		} else {
			return LowLevel;
		}
	} else {
		return UnknownLevel;
	}
	return 0;
}

bool ICACHE_FLASH_ATTR GPIO_isInputIO(uint32 gpioPin) {
	if (!GPIO_ID_IS_PIN_REGISTER(gpioPin)) {
		return UnknownIOMode;
	}

	uint32 gpioBitValue = convertToGPIOBitValue(gpioPin);
	return (mGPIOInputBitsSet & gpioBitValue) == gpioBitValue;
}

bool ICACHE_FLASH_ATTR GPIO_isOutputIO(uint32 gpioPin) {
	if (!GPIO_ID_IS_PIN_REGISTER(gpioPin)) {
		return UnknownIOMode;
	}

	uint32 gpioBitValue = convertToGPIOBitValue(gpioPin);
	return (mGPIOOutputBitsSet & gpioBitValue) == gpioBitValue;
}

bool ICACHE_FLASH_ATTR GPIO_isHighLevel(uint32 gpioPin) {
	if (!GPIO_ID_IS_PIN_REGISTER(gpioPin)) {
		return UnknownIOMode;
	}

	uint32 gpioBitValue = convertToGPIOBitValue(gpioPin);
	return (mGPIOHighLevelBitsSet & gpioBitValue) == gpioBitValue;
}

bool ICACHE_FLASH_ATTR GPIO_isLowLevel(uint32 gpioPin) {
	if (!GPIO_ID_IS_PIN_REGISTER(gpioPin)) {
		return UnknownIOMode;
	}

	uint32 gpioBitValue = convertToGPIOBitValue(gpioPin);
	return (mGPIOLowLevelBitsSet & gpioBitValue) == gpioBitValue;
}

uint32 ICACHE_FLASH_ATTR convertToGPIOBitValue(uint32 gpioPin) {
	return 1 << gpioPin;
}
