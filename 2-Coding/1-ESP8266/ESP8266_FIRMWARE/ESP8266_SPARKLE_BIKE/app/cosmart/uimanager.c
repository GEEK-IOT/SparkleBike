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

LOCAL void onTimeCount(void* msg);

static const int MSG_DISPLAY_MAIN_UI = 0x0001;
static const int MSG_UPDATE_TIME     = 0x0002;

LOCAL Timer mUITimer;

void ICACHE_FLASH_ATTR UI_initialize() {
	Log_printfln("");
	Log_printfln("[GUI] Start UI manager");

	SSD1306_initialize();
	SSD1306_bindDevice(0);

	Log_printfln("[GUI] Display LOGO for 5s");
	SSD1306_drawBuffer(0, 127, 0, 7, IMG_LOGO);
	Timer_setCallbackWithParam(&mUITimer, onTimeCount, (void*)MSG_DISPLAY_MAIN_UI);
	Timer_setLoopable(&mUITimer, false);
	Timer_setInterval(&mUITimer, 5000);
	Timer_start(&mUITimer);
}

LOCAL void ICACHE_FLASH_ATTR onTimeCount(void* msg) {
	int* what = (int*)msg;
	switch (*what) {
	case MSG_DISPLAY_MAIN_UI:
	{
		Timer_stop(&mUITimer);
		SSD1306_cleanScreen();
		SSD1306_drawText("00:00", 0, 0);

		Timer_setCallbackWithParam(&mUITimer, onTimeCount, (void*)MSG_UPDATE_TIME);
		Timer_setLoopable(&mUITimer, true);
		Timer_setInterval(&mUITimer, 500);
		Timer_start(&mUITimer);
	}
		break;

	case MSG_UPDATE_TIME:
	{
		// TODO
	}
		break;
	}
}
