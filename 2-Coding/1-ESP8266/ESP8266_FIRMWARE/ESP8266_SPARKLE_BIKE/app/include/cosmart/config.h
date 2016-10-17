/*
 * config.h
 *
 *  Created on: 2016年1月21日
 *      Author: 80074591
 */

#ifndef APP_INCLUDE_COSMART_CONFIG_H_
#define APP_INCLUDE_COSMART_CONFIG_H_

#define ENABLE_LED_DEBUG     0
#define ENABLE_SERVO_DEBUG   0
#define ENABLE_MPU6050_DEBUG 0
#define ENABLE_SSD1306_DEBUG 0

/**
 * Broadcast receive port
 */
#define DEFAULT_BROADCAST_PORT 7629

/**
 * command receive port
 */
#define DEFAULT_CMD_PORT 7630

/**
 * Station command multicast group address
 * in (239.0.0.0～239.255.255.255)
 */
#define STATION_CMD_MULTICAST_GROUP_IP   "239.100.15.32"
#define STATION_CMD_MULTICAST_GROUP_PORT 7631

#define UDP_MESSAGE_QUEUE_SIZE 5

/**
 * Function definition
 */
#define ENABLE_IR_RECEIVER 1
#define ENABLE_IR_SENDER   0
#define ENABLE_SERVO       1

/**
 *  In order to support IR sending,
 *  define USE_US_TIMER to use the
 *  os_timer_arm_us() api to modulate
 *  the 38kHz carriers.
 */
#if ENABLE_IR_SENDER
#ifndef USE_US_TIMER
#define USE_US_TIMER
#endif
#endif

#if ENABLE_SERVO
#ifndef USE_US_TIMER
#define USE_US_TIMER
#endif
#endif

#define ENABLE_GPIO_INTERRUPT_HANDLE  1
#define ENABLE_SPI_INTERRUPT_HANDLE   0
#define ENABLE_UART_INTERRUPT_HANDLE  0

#endif /* APP_INCLUDE_COSMART_CONFIG_H_ */
