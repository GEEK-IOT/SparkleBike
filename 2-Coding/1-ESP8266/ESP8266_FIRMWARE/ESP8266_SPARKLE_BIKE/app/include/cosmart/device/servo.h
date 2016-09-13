/*
 * servo.h
 *
 *  Created on: 2016年3月20日
 *      Author: Cocoonshu
 */

#ifndef APP_INCLUDE_COSMART_DEVICE_SERVO_H_
#define APP_INCLUDE_COSMART_DEVICE_SERVO_H_

#include "cosmart/config.h"

/**
 * Servo driver
 *
 * ----------------------------------------------------
 *  PWM period : 20ms (50Hz)
 * ----------------------------------------------------
 *  Servo angle: 0°------45°-----90°-----135°----180°
 *  PWM duty   : 0.5ms---1.0ms---1.5ms---2.0ms---2.5ms
 * ----------------------------------------------------
 *
 * Because The PWM period range is 1,000μs (1kHz) to 10,000μs (100Hz),
 * the Servo need to keep period as 20,000μs (50Hz), so we use timer to
 * drive Servo on.
 *
 * ## All time uint is millisecond in this driver library. ##
 */

#define DEFAULT_SERVO_PEROID                  20000
#define DEFAULT_SERVO_MIN_ANGLE_DUTY_DURATION 500
#define DEFAULT_SERVO_MAX_ANGLE_DUTY_DURATION 2500
#define DEFAULT_SERVO_MIN_ANGLE               0
#define DEFAULT_SERVO_MAX_ANGLE               180

#define SERVO_SG90_CONFIG 20000, 500, 2500, 0, 180

typedef struct {
	uint32 GPIOPort;     // GPIO id
	uint32 period;       // μs
	uint32 minAngleDuty; // μs
	uint32 maxAngleDuty; // μs
	int32  minAngle;     // degree
	int32  maxAngle;     // degree
	int32  currentAngle; // degree
} ServoDevice;

void   Servo_initialize(const uint32 servoIOPort[], int length);
void   Servo_initializeAdvance(const uint32 servoIOPort[], int offset, int length);
void   Servo_setupIOPort(int index, uint32 period, uint32 minAngleDutyDuration, uint32 maxAngleDutyDuration, int32 minAngle, int32 maxAngle);
void   Servo_setAngle(int index, int32 angle);
int32  Servo_getAngle(int index);
int    Servo_getSize();
uint32 Servo_getIOPort(int index);
void   Servo_startMotion(int index);
void   Servo_stopMotion(int index);

#endif /* APP_INCLUDE_COSMART_DEVICE_SERVO_H_ */
