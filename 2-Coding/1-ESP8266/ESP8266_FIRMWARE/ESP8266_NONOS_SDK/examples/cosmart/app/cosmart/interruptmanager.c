/*
 * interruptmanager.c
 *
 *  Created on: 2016-01-22
 *      Author: Cocoonshu
 */

#include "cosmart/interruptmanager.h"
#include "cosmart/log.h"
#include "cosmart/config.h"
#include "c_types.h"
#include "ets_sys.h"

// Function declaration
#if ENABLE_GPIO_INTERRUPT_HANDLE
static void handleGPIOInterrupt();
#endif
#if ENABLE_SPI_INTERRUPT_HANDLE
static void handleSPIInterrupt();
#endif
#if ENABLE_UART_INTERRUPT_HANDLE
static void handleUARTInterrupt();
#endif

// Variable declaration
size_t callbackItr = -1;
struct GPIOCallback* pCallbackList[CALLBACK_LIST_SIZE];

void ICACHE_FLASH_ATTR InterruptManager_initialize() {
	Log_printfln("");
	Log_printfln("[Interrupt] Start Interrupt Manager");

	// 初始化回调函数指针数组
	size_t i = 0;
	for (i = 0; i < CALLBACK_LIST_SIZE; i++) {
		pCallbackList[i] = NULL;
	}

	// 启用GPIO中断
#if ENABLE_GPIO_INTERRUPT_HANDLE
	Log_printfln("           - Enable GPIO interrupt handle");
	ETS_GPIO_INTR_ATTACH(handleGPIOInterrupt, NULL);
	ETS_GPIO_INTR_ENABLE();
#endif

	// 启用SPI中断
#if ENABLE_SPI_INTERRUPT_HANDLE
	Log_printfln("           - Enable SPI interrupt handle");
	ETS_SPI_INTR_ATTACH(handleSPIInterrupt, NULL);
	ETS_SPI_INTR_ENABLE();
#endif

	// 启用UART中断
#if ENABLE_UART_INTERRUPT_HANDLE
	Log_printfln("           - Enable UART interrupt handle");
	ETS_SPI_INTR_ATTACH(handleSPIInterrupt, NULL);
	ETS_SPI_INTR_ENABLE();
#endif
}

bool ICACHE_FLASH_ATTR InterruptManager_registerInterruptCallback(struct GPIOCallback *callback) {
	// STEP1 判空检查
	if (callback == NULL || callback->callback_handle == NULL) {
		return false;
	}

	// STEP2 检查在CallbackList中是否有重复的callback
	size_t i = 0;
	for (i = 0; i < CALLBACK_LIST_SIZE; i++) {
		if (callback == pCallbackList[i]) {
			// STEP3.1 有，则不做处理
			return true;
		}
	}

	// STEP3.2 无，则添加callback到CallbackList中
	if (callbackItr < CALLBACK_LIST_SIZE - 1) {
		callbackItr += 1;
		pCallbackList[callbackItr] = callback;
		return true;
	} else {
		return false;
	}
}

void ICACHE_FLASH_ATTR InterruptManager_unregisterInterruptCallback(struct GPIOCallback *callback) {
	// STEP1 判空检查
	if (callback == NULL) {
		return;
	}
	// STEP2 从CallbackList中移除callback
	size_t i = 0;
	for (i = 0; i < CALLBACK_LIST_SIZE; i++) {
		if (callback == pCallbackList[i]) {
			size_t nextItr = i + 1;
			if (nextItr < CALLBACK_LIST_SIZE - 1) {
				pCallbackList[i] = pCallbackList[nextItr];
			} else {
				pCallbackList[i] = NULL;
			}
		}
	}
}

#if ENABLE_GPIO_INTERRUPT_HANDLE
static void handleGPIOInterrupt() {
	uint32 gpioStatus = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	size_t i = 0;
	// Dispatch GPIO interrupt event
	for (i = 0; i <= callbackItr; i++) {
		struct GPIOCallback* currentCallback = pCallbackList[i];
		if (currentCallback != NULL) {
			if ((gpioStatus >> currentCallback->gpio_pin) & BIT0) {
				if (currentCallback->callback_handle != NULL) {
					currentCallback->callback_handle();
				}
			}
		}
	}

	// Clear this interrupt event in case no one handle it.
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpioStatus);
}
#endif

#if ENABLE_SPI_INTERRUPT_HANDLE
static void handleSPIInterrupt() {

}
#endif

#if ENABLE_UART_INTERRUPT_HANDLE
static void handleUARTInterrupt() {

}
#endif
