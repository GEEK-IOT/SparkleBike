/*
 * uimanager.c
 *
 *  Created on: 2016/10/17
 *      Author: Cocoonshu
 */

#include "cosmart/uimanager.h"
#include "cosmart/Log.h"
#include "cosmart/Timer.h"
#include "cosmart/device/ssd1306.h"
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

	Log_printfln("[GUI] Display LOGO for 3s");
	SSD1306_drawBuffer(0, 127, 0, 7, IMG_LOGO);
	SSD1306_flush();

	int msg = MSG_DISPLAY_MAIN_UI;
	Timer_setCallbackWithParam(&mUiTimer, onTimeCount, (void*)MSG_DISPLAY_MAIN_UI);
	Timer_setLoopable(&mUiTimer, false);
	Timer_setInterval(&mUiTimer, 3000);
	Timer_start(&mUiTimer);
}

LOCAL void ICACHE_FLASH_ATTR onTimeCount(void* msg) {
	int what = (int)msg;

	switch (what) {
	case MSG_DISPLAY_MAIN_UI:
	{
		Timer_stop(&mUiTimer);

		SSD1306_drawText("--:--:--", 48, 0);
		SSD1306_flush();

		Timer_setCallbackWithParam(&mUiTimer, onTimeCount, (void*)MSG_UPDATE_TIME);
		Timer_setLoopable(&mUiTimer, true);
		Timer_setInterval(&mUiTimer, 1000);
		Timer_start(&mUiTimer);
	}
		break;

	case MSG_UPDATE_TIME:
	{
		uint32 timestamp = sntp_get_current_timestamp();
		if (timestamp == 0) {
			sint8 rssi = WiFi_getSTARssi();

			SSD1306_erase(0, 0, 128, 8);
			if (rssi < 0) {
				/**
				 * -90.0dBm = 1
				 * -82.5dBm = 2
				 * -75.0dBm = 3
				 * -67.5dBm = 4
				 * -60.0dBm = 5
				 */
				uint8 level = 0;
				if (rssi > -60.0f)      level = 5;
				else if (rssi > -67.5f) level = 4;
				else if (rssi > -75.0f) level = 3;
				else if (rssi > -82.5f) level = 2;
				else if (rssi > -90.0f) level = 1;
				else                    level = 0;

				SSD1306_drawBuffer(0, level * 2, 0, 0, ICON_WIFI);
			} else {
				SSD1306_drawBuffer(0, 6, 0, 0, ICON_NO_WIFI);
			}
			SSD1306_drawText("--:--:--", 48, 0);
			SSD1306_drawBuffer(117, 127, 0, 0, ICON_BATTERY);
			SSD1306_flush();
		} else {
			char timeText[16];
			DateTime* time = Timer_getDateTime();
			sint8     rssi = WiFi_getSTARssi();
			os_bzero(timeText, 16),
			os_sprintf(timeText, "%02d:%02d:%02d", time->hour, time->minute, time->second);
			os_free(time);

			SSD1306_erase(0, 0, 128, 8);
			if (rssi < 0) {
				/**
				 * -90.0dBm = 1
				 * -82.5dBm = 2
				 * -75.0dBm = 3
				 * -67.5dBm = 4
				 * -60.0dBm = 5
				 */
				uint8 level = 0;
				if (rssi > -60.0f)      level = 5;
				else if (rssi > -67.5f) level = 4;
				else if (rssi > -75.0f) level = 3;
				else if (rssi > -82.5f) level = 2;
				else if (rssi > -90.0f) level = 1;
				else                    level = 0;

				SSD1306_drawBuffer(0, level * 2, 0, 0, ICON_WIFI);
			} else {
				SSD1306_drawBuffer(0, 6, 0, 0, ICON_NO_WIFI);
			}
			SSD1306_drawText(timeText, 44, 0);
			SSD1306_drawBuffer(117, 127, 0, 0, ICON_BATTERY);
			SSD1306_flush();
		}
	}
		break;
	}
}
