/*
 * uimanager.c
 *
 *  Created on: 2016年10月17日
 *      Author: oppo
 */

#include "cosmart/uimanager.h"
#include "cosmart/Log.h"
#include "cosmart/Timer.h"
#include "user_interface.h"
#include "mem.h"

LOCAL void onTimeCount(void* msg);

#define MSG_DISPLAY_MAIN_UI  0x0001
#define MSG_UPDATE_TIME      0x0002

LOCAL Timer mUiTimer;

void ICACHE_FLASH_ATTR UI_initialize() {
	Log_printfln("");
	Log_printfln("[GUI] Start UI manager");

	SSD1306_initialize();
	SSD1306_bindDevice(0);

	Log_printfln("[GUI] Display LOGO for 5s");
	SSD1306_drawBuffer(0, 127, 0, 7, IMG_LOGO);
	int msg = MSG_DISPLAY_MAIN_UI;
	Timer_setCallbackWithParam(&mUiTimer, onTimeCount, (void*)MSG_DISPLAY_MAIN_UI);
	Timer_setLoopable(&mUiTimer, false);
	Timer_setInterval(&mUiTimer, 5000);
	Timer_start(&mUiTimer);
}

LOCAL void ICACHE_FLASH_ATTR onTimeCount(void* msg) {
	int what = (int)msg;

	switch (what) {
	case MSG_DISPLAY_MAIN_UI:
	{
		Timer_stop(&mUiTimer);
		//SSD1306_cleanScreen();
		SSD1306_drawText("--:--:--", 43, 0);

		Timer_setCallbackWithParam(&mUiTimer, onTimeCount, (void*)MSG_UPDATE_TIME);
		Timer_setLoopable(&mUiTimer, true);
		Timer_setInterval(&mUiTimer, 1000);
		Timer_start(&mUiTimer);
	}
		break;

	case MSG_UPDATE_TIME:
	{
		// TODO
		char timeText[16];
		DateTime* time = Timer_getDateTime();
//		Log_printfln("[UI] Time = %04d-%02d-%02d %02d:%02d:%02d",
//				time->year, time->month, time->day,
//				time->hour, time->minute, time->second);
		os_bzero(timeText, 16),
		os_sprintf(timeText, "%02d:%02d:%02d", time->hour, time->minute, time->second);
		os_free(time);
		time = NULL;

		SSD1306_drawText(timeText, 43, 0);
	}
		break;
	}
}
