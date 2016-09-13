/*
 * timer.c
 *
 *  Created on: 2016年4月1日
 *      Author: 80074591
 */

#include "cosmart/cosmart.h"
#include "cosmart/timer.h"
#include "cosmart/log.h"
#include "osapi.h"

void ICACHE_FLASH_ATTR Timer_initialize() {
	Log_printfln(LOG_EMPTY);
	Log_printfln(LOG_TIMER_INITIALIZED);
#ifdef USE_US_TIMER
	Log_printfln(LOG_TIMER_SETUP_US_SCALE);
#endif
}

void ICACHE_FLASH_ATTR Timer_setCallback(Timer* timer, TimerCallback callback) {
	Timer_setCallbackWithParam(timer, callback, NULL);
}

void ICACHE_FLASH_ATTR Timer_setCallbackWithParam(Timer* timer, TimerCallback callback, TimerCallbackParam param) {
	if (timer == NULL) {
		return;
	}
	timer->callback = callback;
	timer->callbackParam = param;
}

void ICACHE_FLASH_ATTR Timer_setTimerUnit(Timer* timer, TimerUnit unit) {
	if (timer == NULL) {
		return;
	}
	timer->unit = unit;
}

void ICACHE_FLASH_ATTR Timer_setInterval(Timer* timer, uint32 interval) {
	if (timer == NULL) {
		return;
	}
	timer->interval = interval;
}

void ICACHE_FLASH_ATTR Timer_setLoopable(Timer* timer, bool isLoop) {
	if (timer == NULL) {
		return;
	}
	timer->isLoop = isLoop;
}

void ICACHE_FLASH_ATTR Timer_delayMS(uint32 delay) {
	Timer_delayUS(delay * 1000);
}

void ICACHE_FLASH_ATTR Timer_delayUS(uint32 delay) {
	if (delay <= 0) {
		return;
	} else {
		long remainDelay = delay;
		long deltaDelay  = TIMER_MAX_DALAY_US;
		do {
			deltaDelay = remainDelay - TIMER_MAX_DALAY_US;
			if (deltaDelay > 0) {
				os_delay_us(TIMER_MAX_DALAY_US);
				system_soft_wdt_feed();
				remainDelay = deltaDelay;
			} else {
				os_delay_us(remainDelay);
				system_soft_wdt_feed();
				remainDelay = 0;
			}
		} while (remainDelay > 0);
	}
}

void ICACHE_FLASH_ATTR Timer_start(Timer* timer) {
	if (timer == NULL) {
		return;
	}

	switch (timer->unit) {
		case MSTimer: {
			ets_timer_setfn(&(timer->timer), timer->callback, timer->callbackParam);
			ets_timer_arm_new(&(timer->timer), timer->interval, timer->isLoop, 1);
		} break;

#ifdef USE_US_TIMER
		case USTimer: {
			ets_timer_setfn(&(timer->timer), timer->callback, timer->callbackParam);
			ets_timer_arm_new(&(timer->timer), timer->interval, timer->isLoop, 0);
		} break;
#endif
	}
}

void ICACHE_FLASH_ATTR Timer_stop(Timer* timer) {
	if (timer == NULL) {
		return;
	}
	os_timer_disarm(&(timer->timer));
}
