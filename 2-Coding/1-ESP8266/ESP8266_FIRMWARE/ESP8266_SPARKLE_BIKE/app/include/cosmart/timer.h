/*
 * timer.h
 *
 *  Created on: 2016年4月1日
 *      Author: 80074591
 */

#ifndef APP_INCLUDE_COSMART_TIMER_H_
#define APP_INCLUDE_COSMART_TIMER_H_

#include "os_type.h"
#include "cosmart/config.h"

#define TIMER_MAX_DALAY_US 1000000

typedef os_timer_t      NativeTimer;
typedef os_timer_func_t TimerCallback;
typedef void*           TimerCallbackParam;

typedef enum {
#ifdef USE_US_TIMER
	USTimer = 1,
#endif
	MSTimer = 0
} TimerUnit;

typedef struct {
	NativeTimer        timer;
	TimerCallback      *callback;
	TimerCallbackParam *callbackParam;
	TimerUnit          unit;
	uint32             interval;
	bool               isLoop;
} Timer;

void   Timer_initialize();
void   Timer_setCallback(Timer* timer, TimerCallback callback);
void   Timer_setCallbackWithParam(Timer* timer, TimerCallback callback, TimerCallbackParam param);
void   Timer_setTimerUnit(Timer* timer, TimerUnit unit);
void   Timer_setInterval(Timer* timer, uint32 interval);
void   Timer_setLoopable(Timer* timer, bool isLoop);
void   Timer_delayMS(uint32 delay);
void   Timer_delayUS(uint32 delay);
void   Timer_start(Timer* timer);
void   Timer_stop(Timer* timer);
uint32 Timer_getTime();

#endif /* APP_INCLUDE_COSMART_TIMER_H_ */
