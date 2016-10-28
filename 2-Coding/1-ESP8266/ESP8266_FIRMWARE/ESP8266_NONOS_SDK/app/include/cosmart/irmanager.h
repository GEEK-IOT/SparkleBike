/*
 * irmanager.h
 *
 *  Created on: 2016年1月21日
 *      Author: 80074591
 */

#ifndef APP_INCLUDE_COSMART_IRMANAGER_H_
#define APP_INCLUDE_COSMART_IRMANAGER_H_
#include "cosmart/gpiomanager.h"

/**
 * IR接口
 *    - GPIO14 - IR_SEND,    I2SI_WS
 *    - GPIO05 - IR_RECEIVE, GPIO_05
 *    IR Remote Control 接口由软件实现，接口使用NEC编码及调制解调，采用
 *    I2S的WS端或BC端产生的38kHz载波用于调制发射
 */
#define IR_SEND_GPIO_PIN    GPIO_PIN_14
#define IR_RECEIVE_GPIO_PIN GPIO_PIN_05

/**
 * INFRARED REMOTE PROTOCOL DEFINITION
 */
#define IR_PROTOCOL_NEC         0
#define IR_PROTOCOL_RC5         1

/**
 * NEC CODE DEFINITION
 */
#define IR_NEC_STATE_IDLE        0
#define IR_NEC_STATE_GUIDE       1
#define IR_NEC_STATE_ADDR        2
#define IR_NEC_STATE_ADDR_REVERT 3
#define IR_NEC_STATE_CMD         4
#define IR_NEC_STATE_CMD_REVERT  5
#define IR_NEC_STATE_REPEAT      6

#define IR_NEC_DURATION_ERROR   200
#define IR_NEC_GUIDE_PRE        9000
#define IR_NEC_GUIDE_POST       4500



#define IR_NEC_BIT_NUM 		    8
#define IR_NEC_MAGIN_US 		200
#define IR_NEC_TM_PRE_US 		13500
#define IR_NEC_D1_TM_US 		2250
#define IR_NEC_D0_TM_US 		1120
#define IR_NEC_REP_TM1_US 	    20000
#define IR_NEC_REP_TM2_US 	    11250
#define IR_NEC_REP_LOW_US 	    2250
#define IR_NEC_REP_CYCLE    	110000
#define IR_NEC_HEADER_HIGH_US   9000
#define IR_NEC_HEADER_LOW_US 	4500
#define IR_NEC_DATA_HIGH_US 	560
#define IR_NEC_DATA_LOW_1_US 	1690
#define IR_NEC_DATA_LOW_0_US 	560

void IR_initialize();

#endif /* APP_INCLUDE_COSMART_IRMANAGER_H_ */
