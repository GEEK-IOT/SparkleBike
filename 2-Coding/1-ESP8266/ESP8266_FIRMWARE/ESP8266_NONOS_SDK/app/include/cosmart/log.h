/*
 * log.h
 *
 *  Created on: 2015��8��5��
 *      Author: Cocoonshu
 */

#ifndef APP_INCLUDE_COSMART_LOG_H_
#define APP_INCLUDE_COSMART_LOG_H_

#include <stdarg.h>
#include "cosmart/cosmart.h"
#include "driver/uart.h"
#include "osapi.h"

#define LOG_BY_UART 1   // 0 for os_printf, 1 for uart
#define LOG_BUFFER_SIZE UART_TX_BUFFER_SIZE

/**
 * 初始化Log组件
 */
void Log_initialize();

/**
 * 打印Log，默认从UART0输出
 */
#if LOG_BY_UART
void Log_printf(const char *fmt, ...);
void Log_printfln(const char *fmt, ...);
//	#define Log_printf(fmt, ...) {\
//		char buffer[LOG_BUFFER_SIZE];\
//		os_sprintf(buffer, fmt, ##__VA_ARGS__);\
//		uart0_sendStr(buffer);\
//	}
//	#define Log_printfln(fmt, ...) {\
//		char buffer[LOG_BUFFER_SIZE];\
//		os_sprintf(buffer, fmt, ##__VA_ARGS__);\
//		uart0_sendStr(buffer);\
//		uart0_sendStr("\r\n");\
//	}
#else
	#define Log_printf(fmt, ...) {\
		os_printf(fmt, ##__VA_ARGS__);\
	}
	#define Log_printfln(fmt, ...) {\
		os_printf(fmt, ##__VA_ARGS__);\
		os_printf(LOG_ONE_LINE);\
	}
#endif

/**
 * 打印版权信息
 */
void Log_printCopyright();

#endif /* APP_INCLUDE_COSMART_LOG_H_ */
