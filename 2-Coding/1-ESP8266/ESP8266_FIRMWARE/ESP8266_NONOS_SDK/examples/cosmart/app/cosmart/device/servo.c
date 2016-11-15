/*
 * servo.c
 *
 *  Created on: 2016年3月20日
 *      Author: Cocoonshu
 */
#include "cosmart/log.h"
#include "cosmart/gpiomanager.h"
#include "cosmart/timer.h"
#include "cosmart/device/servo.h"
#include "gpio.h"
#include "mem.h"

#include "eagle_soc.h"

Timer*       mServoTimer      = NULL;
ServoDevice* mServoDeviceList = NULL;
int          mServoDeviceSize = 0;

void ICACHE_FLASH_ATTR Servo_processServoMotion(void* data);

void ICACHE_FLASH_ATTR Servo_initialize(const uint32 servoIOPort[], int length) {
	Servo_initializeAdvance(servoIOPort, 0, length);
}

void ICACHE_FLASH_ATTR Servo_initializeAdvance(const uint32 servoIOPort[], int offset, int length) {
	Log_printfln("");
	Log_printfln("[Servo] Start Servo device service");

	mServoTimer      = (Timer*)      os_zalloc(sizeof(Timer) * length);
	mServoDeviceList = (ServoDevice*)os_zalloc(sizeof(ServoDevice) * length);
	os_memset(mServoTimer,      0, sizeof(Timer) * length);
	os_memset(mServoDeviceList, 0, sizeof(ServoDevice) * length);

	int i    = 0;
	int loop = 0;
	for (i = 0, loop = offset; i < length && loop < offset + length; i++, loop++) {
		mServoDeviceList[i].GPIOPort     = servoIOPort[loop];
		mServoDeviceList[i].period       = DEFAULT_SERVO_PEROID;
		mServoDeviceList[i].minAngleDuty = DEFAULT_SERVO_MIN_ANGLE_DUTY_DURATION;
		mServoDeviceList[i].maxAngleDuty = DEFAULT_SERVO_MAX_ANGLE_DUTY_DURATION;
		mServoDeviceList[i].minAngle     = DEFAULT_SERVO_MIN_ANGLE;
		mServoDeviceList[i].maxAngle     = DEFAULT_SERVO_MAX_ANGLE;
		mServoDeviceList[i].currentAngle = 0;

		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(14), 0);
		//GPIO_setIOMode(mServoDeviceList[i].GPIOPort, OutputMode);
		//GPIO_setLevel(mServoDeviceList[i].GPIOPort, LowLevel);

		Log_printfln("       - Setup GPIO_%d as Servo_%d port", servoIOPort[loop], i);
	}
	mServoDeviceSize = length;
}

void ICACHE_FLASH_ATTR Servo_setupIOPort(int index, uint32 period, uint32 minAngleDutyDuration, uint32 maxAngleDutyDuration, int32 minAngle, int32 maxAngle) {
	if (mServoDeviceList == NULL || index < 0 || index >= mServoDeviceSize) {
		return;
	}

	mServoDeviceList[index].period       = period;
	mServoDeviceList[index].minAngleDuty = minAngleDutyDuration;
	mServoDeviceList[index].maxAngleDuty = maxAngleDutyDuration;
	mServoDeviceList[index].minAngle     = minAngle;
	mServoDeviceList[index].maxAngle     = maxAngle;
	mServoDeviceList[index].currentAngle = minAngle;
	Log_printfln("       - Setup Servo_%d(GPIO_%d) for peroid in %dus(%dus~%dus) and angle in %d'(%d'~%d')",
			index, mServoDeviceList[index].GPIOPort,
			period, minAngleDutyDuration, maxAngleDutyDuration,
			minAngle, minAngle, maxAngle);
}

void ICACHE_FLASH_ATTR Servo_setAngle(int index, int32 angle) {
	if (mServoDeviceList == NULL || index < 0 || index >= mServoDeviceSize) {
		return;
	}

	mServoDeviceList[index].currentAngle = angle;
	Log_printfln("[Servo] Set Servo_%d(GPIO_%d) to %d", index, mServoDeviceList[index].GPIOPort, angle);
}

int32 ICACHE_FLASH_ATTR Servo_getAngle(int index) {
	if (mServoDeviceList == NULL || index < 0 || index >= mServoDeviceSize) {
		return 0;
	}

	return mServoDeviceList[index].currentAngle;
}

int ICACHE_FLASH_ATTR Servo_getSize() {
	return mServoDeviceSize;
}

uint32 ICACHE_FLASH_ATTR Servo_getIOPort(int index) {
	if (mServoDeviceList == NULL || index < 0 || index >= mServoDeviceSize) {
		return 0;
	}

	return mServoDeviceList[index].GPIOPort;
}

void ICACHE_FLASH_ATTR Servo_startMotion(int index) {
	if (mServoDeviceList == NULL || index < 0 || index >= mServoDeviceSize) {
		return;
	}

	int* timerID = (int *)os_zalloc(sizeof(int));
	*timerID = index;
	Timer_setCallbackWithParam(&mServoTimer[index], (TimerCallback*)Servo_processServoMotion, (void *)timerID);
	Timer_setInterval(&mServoTimer[index], mServoDeviceList[index].period);
	Timer_setLoopable(&mServoTimer[index], true);
	Timer_setTimerUnit(&mServoTimer[index], USTimer);
	Timer_start(&mServoTimer[index]);
	Log_printfln("[Servo] Servo_%d(GPIO_%d) start motion", index, mServoDeviceList[index].GPIOPort);
}

void ICACHE_FLASH_ATTR Servo_stopMotion(int index) {
	if (mServoDeviceList == NULL || index < 0 || index >= mServoDeviceSize) {
		return;
	}

	Timer_stop(&mServoTimer[index]);
	Log_printfln("[Servo] Servo_%d(GPIO_%d) stop motion", index, mServoDeviceList[index].GPIOPort);
}

void ICACHE_FLASH_ATTR Servo_processServoMotion(void* data) {
	if (data == NULL) {
		return;
	}

	int timerIndex = *((int *)data);

	ServoDevice* device      = &mServoDeviceList[timerIndex];
	float        delayFactor = (float)(device->currentAngle - device->minAngle) / (float)(device->maxAngle - device->minAngle);
	uint32       delayUS     = delayFactor * (device->maxAngleDuty - device->minAngleDuty) + device->minAngleDuty;

	delayUS = delayUS > device->period ?  device->period : delayUS;
	GPIO_OUTPUT_SET(GPIO_ID_PIN(14), 1);
	//GPIO_setLevel(device->GPIOPort, HighLevel);
	Timer_delayUS(delayUS);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(14), 0);
	//GPIO_setLevel(device->GPIOPort, LowLevel);
//	Log_printfln("[Servo][Servo_processServoMotion] GPIO_%d, delay = %dus, angle = %d", device->GPIOPort, delayUS, device->currentAngle);
}

