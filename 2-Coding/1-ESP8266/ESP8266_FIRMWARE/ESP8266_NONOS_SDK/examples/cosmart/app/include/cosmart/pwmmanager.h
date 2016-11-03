/*
 * gpiomanager.h
 *
 *  Created on: 2016年3月20日
 *      Author: Cocoonshu
 */

#ifndef APP_INCLUDE_COSMART_PWMMANAGER_H_
#define APP_INCLUDE_COSMART_PWMMANAGER_H_

#include "c_types.h"
#include "eagle_soc.h"
#include "gpio.h"
#include "cosmart/gpiomanager.h"

#define PWM_MAP_ITEM_00 {PWM_REG_00, PWM_MUX_00, PWM_PIN_00}
#define PWM_MAP_ITEM_01 {PWM_REG_01, PWM_MUX_01, PWM_PIN_01}
#define PWM_MAP_ITEM_02 {PWM_REG_02, PWM_MUX_02, PWM_PIN_02}
#define PWM_MAP_ITEM_03 {PWM_REG_03, PWM_MUX_03, PWM_PIN_03}
#define PWM_MAP_ITEM_04 {PWM_REG_04, PWM_MUX_04, PWM_PIN_04}
#define PWM_MAP_ITEM_05 {PWM_REG_05, PWM_MUX_05, PWM_PIN_05}
#define PWM_MAP_ITEM_06 {PWM_REG_06, PWM_MUX_06, PWM_PIN_06}
#define PWM_MAP_ITEM_07 {PWM_REG_07, PWM_MUX_07, PWM_PIN_07}
#define PWM_MAP_ITEM_08 {PWM_REG_08, PWM_MUX_08, PWM_PIN_08}
#define PWM_MAP_ITEM_09 {PWM_REG_09, PWM_MUX_09, PWM_PIN_09}
#define PWM_MAP_ITEM_10 {PWM_REG_10, PWM_MUX_10, PWM_PIN_10}
#define PWM_MAP_ITEM_11 {PWM_REG_11, PWM_MUX_11, PWM_PIN_11}
#define PWM_MAP_ITEM_12 {PWM_REG_12, PWM_MUX_12, PWM_PIN_12}
#define PWM_MAP_ITEM_13 {PWM_REG_13, PWM_MUX_13, PWM_PIN_13}
#define PWM_MAP_ITEM_14 {PWM_REG_14, PWM_MUX_14, PWM_PIN_14}
#define PWM_MAP_ITEM_15 {PWM_REG_15, PWM_MUX_15, PWM_PIN_15}

#define PWM_SETUP_ITEM_COUNT 3
#define DEFAULT_PWM_PERIOD   1000 // 1kHz
#define DEFAULT_PWM_MIN_DUTY 0
#define DEFAULT_PWM_MAX_DUTY (DEFAULT_PWM_PERIOD * 1000 / 45)

void   PWM_initialize();
bool   PWM_isInitialized();
void   PWM_registerPWMPort(int ioPort);
void   PWM_unregisterPWMPort(int ioPort);
void   PWM_setup();
void   PWM_apply();

void   PWM_setPWMDutyLevel(int gpioPin, uint32 dutyLevel);
uint32 PWM_getPWMDutyLevel(int gpioPin);
uint32 PWM_getPWMSize();
uint32 PWM_getMaxPWMDuty();
uint32 PWM_getMinPWMDuty();
uint32 PWM_getPeriod();

#endif /* APP_INCLUDE_COSMART_PWMMANAGER_H_ */
