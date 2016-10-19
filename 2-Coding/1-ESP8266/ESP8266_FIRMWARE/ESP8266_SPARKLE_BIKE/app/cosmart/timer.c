/*
 * timer.c
 *
 *  Created on: 2016年4月1日
 *      Author: 80074591
 */

#include "cosmart/cosmart.h"
#include "cosmart/timer.h"
#include "cosmart/log.h"
#include "mem.h"
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

uint32 ICACHE_FLASH_ATTR Timer_getSystemTime() {
	return system_get_time();
}

DateTime* ICACHE_FLASH_ATTR Timer_getDateTime() {
	/**
	 *  format: Tue Oct 18 14:07:07 2016
	 *          %s %s %02d %02d:%02d:%02d %02d
	 */
	static const char day_name[7][4] = {
			"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	static const char month_name[12][4] = {
			"Jan", "Feb", "Mar", "Apr", "May", "Jun",
			"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	long      timeStamp = sntp_get_current_timestamp();
	char*     timeStr   = sntp_get_real_time(timeStamp);
	DateTime* dateTime  = (DateTime*) os_malloc(sizeof(DateTime));
	int i, j;

	os_memset(dateTime, NULL, sizeof(DateTime));

	// Days of week
	for (j = 0; j < 7; j++) {
		bool isMatch = true;
		for (i = 0; i < 3; i++) {
			if (day_name[j][i] != timeStr[i]) {
				isMatch = false;
				break;
			}
		}
		if (isMatch) {
			dateTime->dayOfWeek = j;
		}
	}

	// Month
	for (j = 0; j < 12; j++) {
		bool isMatch = true;
		for (i = 4; i < 7; i++) {
			if (month_name[j][i - 4] != timeStr[i]) {
				isMatch = false;
				break;
			}
		}
		if (isMatch) {
			dateTime->month = j + 1;
		}
	}

	// Day
	char dayStr[3] = {timeStr[8], timeStr[9], 0};
	dateTime->day = atoi(dayStr);

	// Hour
	char hourStr[3] = {timeStr[11], timeStr[12], 0};
	dateTime->hour = atoi(hourStr);

	// Minute
	char minuteStr[3] = {timeStr[14], timeStr[15], 0};
	dateTime->minute = atoi(minuteStr);

	// Second
	char secondStr[3] = {timeStr[17], timeStr[18], 0};
	dateTime->second = atoi(secondStr);

	// Year
	char yearStr[5] = {timeStr[20], timeStr[21], timeStr[22], timeStr[23], 0};
	dateTime->year = atoi(yearStr);

	return dateTime;
}

char* ICACHE_FLASH_ATTR Timer_getDateTimeString() {
	return sntp_get_real_time(sntp_get_current_timestamp());
}
