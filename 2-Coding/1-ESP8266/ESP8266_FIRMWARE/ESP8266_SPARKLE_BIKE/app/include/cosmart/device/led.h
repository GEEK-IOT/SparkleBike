/*
 * led.h
 *
 *  Created on: 2016年3月20日
 *      Author: Cocoonshu
 */

#ifndef APP_INCLUDE_COSMART_DEVICE_LED_H_
#define APP_INCLUDE_COSMART_DEVICE_LED_H_

#include "c_types.h"

#define DEFAULT_LED_PERIOD          1000 // ms
#define DEFAULT_LED_UPDATE_INTERVAL 16   // ms

void   LED_initialize(const uint32 ledIOPort[], int length);
void   LED_initializeAdvance(const uint32 ledIOPort[], int offset, int length);
void   LED_setBrightness(int index, uint32 brightness);
uint32 LED_getBrightness(int index);
int    LED_getSize();
uint32 LED_getIOPort(int index);
void   LED_turnOn(int index);
void   LED_turnOff(int index);
void   LED_turnOnAll();
void   LED_turnOffAll();
void   LED_startBreathing(int index);
void   LED_startBreathingWithDuration(int index, int duration);
void   LED_startBreathingWithDurationAndDelay(int index, int duration, int delay);
void   LED_stopBreathing(int index);
void   LED_startBurstFlashing(int redLedIndex, int blueLedIndex);
void   LED_stopBurstFlashing();

#endif /* APP_INCLUDE_COSMART_DEVICE_LED_H_ */
