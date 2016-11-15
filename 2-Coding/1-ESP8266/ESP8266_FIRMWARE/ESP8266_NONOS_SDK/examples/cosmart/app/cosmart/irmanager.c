/*
 * irmanager.c
 *
 *  Created on: 2016年1月21日
 *      Author: 80074591
 */

#include "cosmart/irmanager.h"
#include "cosmart/log.h"
#include "cosmart/config.h"
#include "cosmart/interruptmanager.h"
#include "c_types.h"

// Function declaration
static void receiveHandle();
// Variable declaration
static struct GPIOCallback receiveCallback;

void ICACHE_FLASH_ATTR IR_initialize() {
	Log_printfln("");
	Log_printfln("[IR] Start IR Manager");

#ifdef ENABLE_IR_RECEIVER
	Log_printfln("   - Enable IR sending function");
	Log_printfln("   - Setup GPIO_14 as IR_SEND on 38kHz");
#endif

#ifdef ENABLE_IR_SENDER
	Log_printfln("   - Enable IR receive function");
	Log_printfln("   - Setup GPIO_%d as IR_RECEIVE", IR_RECEIVE_GPIO_PIN);
	Log_printfln("   - Register GPIO_%d to interrupt handle", IR_RECEIVE_GPIO_PIN);

	receiveCallback.callback_handle = &receiveHandle;
	receiveCallback.gpio_pin = IR_RECEIVE_GPIO_PIN;
	InterruptManager_registerInterruptCallback(&receiveCallback);
#endif
}

//#define IR_NEC_TIME_ERROR            200
//#define IR_NEC_HEADER_H_DURATION     9000
//#define IR_NEC_GUIDE_L_DURATION      4500
//#define IR_NEC_REPEAT_L_DURATION     2250
//#define IR_NEC_DATA_H_DURATION       560
//#define IR_NEC_DATA_L_0_DURATION     560
//#define IR_NEC_DATA_L_1_DURATION     1690
//#define IR_NEC_DATA_L_0_RE_DURATION  560
//#define IR_NEC_DATA_L_1_RE_DURATION  1690
//#define IR_NEC_FRAME_DURATION        110000
// GUIDE + ADDR + !ADDR + COMMAND + !COMMAND + END
// 1bit  + 8bit + 8bit  + 8bit    + 8bit     + 1bit
// u8    + u8   + u8    + u8      + u8       + u8
static void receiveHandle() {
//	// 读取GPIO状态
//	static bool   isDuringFrame  = false;
//	static bool   isGuided       = false;
//	static uint32 frameStartTime = 0;
//	static uint32 highStartTime  = 0;
//	static uint32 lowStartTime   = 0;
//
//	uint32 gpioStatus        = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
//	uint32 gpioLevel         = GPIO_INPUT_GET(GPIO_ID_PIN(IR_RECEIVE_GPIO_PIN));
//	uint32 currentTime       = system_get_time();
//
//	if (gpioLevel == GPIO_LEVEL_HIGH) {
//		if (!isDuringFrame) {
//			frameStartTime = currentTime;
//			highStartTime = currentTime;
//			isDuringFrame = true;
//			isGuided = false;
//		} else {
//			// 先处理上一个码
//			{
//				uint32 highDuration = lowStartTime - highStartTime;
//				highDuration = highDuration < 0 ? 0 : highDuration;
//				uint32 lowDuration = currentTime - lowStartTime;
//				lowDuration = lowDuration < 0 ? 0 : lowDuration;
//				uint32 totalDuration = highDuration + lowDuration;
//
//				if (highDuration > IR_NEC_HEADER_H_DURATION - IR_NEC_TIME_ERROR
//				 && highDuration < IR_NEC_HEADER_H_DURATION + IR_NEC_TIME_ERROR) {
//					// 引导码或者重复码
//					if (lowDuration > IR_NEC_GUIDE_L_DURATION - IR_NEC_TIME_ERROR
//					 && lowDuration < IR_NEC_GUIDE_L_DURATION + IR_NEC_TIME_ERROR) {
//						// 引导码
//						// TODO
//					} else if (lowDuration > IR_NEC_REPEAT_L_DURATION - IR_NEC_TIME_ERROR
//							&& lowDuration < IR_NEC_REPEAT_L_DURATION + IR_NEC_TIME_ERROR){
//						// 重复码
//						// TODO
//					}
//				}
//				else
//				if (highDuration > IR_NEC_DATA_H_DURATION - IR_NEC_TIME_ERROR
//			     && highDuration < IR_NEC_DATA_H_DURATION + IR_NEC_TIME_ERROR) {
//					// 数据码或者结束码
//					if (lowDuration > IR_NEC_DATA_L_0_DURATION - IR_NEC_TIME_ERROR
//					 && lowDuration < IR_NEC_DATA_L_0_DURATION + IR_NEC_TIME_ERROR) {
//						// 0码或者~1码
//						// TODO
//					} else {
//						// 结束码
//						// TODO
//						isDuringFrame = false;
//					}
//				}
//			}
//			// 记录下一个码的起始点
//			{
//				highStartTime = currentTime;
//			}
//		}
//	} else {
//
//	}
}
//
//static void receiveHandle() {
//	static uint8  sRepeatCode      = 0;
//	static uint32 sLastTime        = system_get_time();
//	static uint32 sFrameStartTime  = 0;
//	static uint32 sFrameMiddleTime = 0;
//	static uint32 sFrameEndTime    = 0;
//	static uint32 sIRState         = IR_NEC_STATE_IDLE;
//
//	// 读取GPIO状态
//	uint32 gpioStatus        = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
//	uint32 gpioLevel         = GPIO_INPUT_GET(GPIO_ID_PIN(IR_RECEIVE_GPIO_PIN));
//	uint32 currentTime       = system_get_time();
//	uint32 duration          = currentTime - sLastTime;
//	uint32 preFrameDuration  = 0;
//	uint32 postFrameDuration = 0;
//
//	// 记录NEC帧的时刻点
//	//   ＿|￣  FrameStart or FrameEnd
//	//
//	//   ￣|＿  FrameMiddle
//	if (gpioLevel == GPIO_LEVEL_HIGH) {
//		sFrameEndTime   = sLastTime;
//		sFrameStartTime = currentTime;
//	} else {
//		sFrameMiddleTime = currentTime;
//	}
//
//	// 帧判别
//	switch (sIRState) {
//	case IR_NEC_STATE_IDLE:
//		{
//			if (gpioLevel == GPIO_LEVEL_HIGH) {
//				preFrameDuration  = sFrameMiddleTime - sFrameStartTime;
//				postFrameDuration = sFrameEndTime - sFrameMiddleTime;
//				if (preFrameDuration <= 0) {
//					// 当前为帧起始点，等待下一次中断信号
//					sFrameMiddleTime = 0;
//					sFrameEndTime    = 0;
//					goto END_FRAME_DETECT;
//				} else if (postFrameDuration <= 0) {
//					// 帧错误，重置
//					goto RESET_FRAME_DETECT;
//				} else {
//					// 帧尾部，下一帧的起始点
//					goto DETECT_FRAME;
//				}
//			} else {
//				// 当前为帧中点，等待下一次中断信号
//				preFrameDuration  = sFrameMiddleTime - sFrameStartTime;
//				postFrameDuration = 0;
//				goto END_FRAME_DETECT;
//			}
//		}
//		break;
//	}
//
//	/**
//	 * 当前已经存在有效帧，检测此帧
//	 */
//	DETECT_FRAME : {
//		// 引导帧判别
//		if ((preFrameDuration >= IR_NEC_GUIDE_PRE - IR_NEC_DURATION_ERROR)
//		 && (preFrameDuration <= IR_NEC_GUIDE_PRE + IR_NEC_DURATION_ERROR)) {
//			// 可能为引导帧：NEC_GUIDE
//			if ((postFrameDuration >= IR_NEC_GUIDE_POST - IR_NEC_DURATION_ERROR)
//			&& (postFrameDuration <= IR_NEC_GUIDE_POST + IR_NEC_DURATION_ERROR)) {
//				// 为引导帧：NEC_GUIDE
//				// 改变帧判别状态
//				sIRState = IR_NEC_STATE_GUIDE;
//			}
//		} else
//		// 地址帧判别
//		if (sIRState == IR_NEC_STATE_GUIDE) {
//			// 开始累积8bit正向地址帧
//		} else if (sIRState == IR_NEC_STATE_ADDR) {
//			// 开始累积8bit反向地址帧
//		} else
//        //
//		if (sIRState == IR_NEC_STATE_ADDR_REVERT) {
//
//		}
//	}
//
//	/**
//	 * 当前帧已处理完毕，重置所有检测状态
//	 */
//	RESET_FRAME_DETECT : {
//		sFrameStartTime  = 0;
//		sFrameMiddleTime = 0;
//		sFrameEndTime    = 0;
//		sIRState         = IR_NEC_STATE_IDLE;
//	}
//
//	/**
//	 * 结束帧判别
//	 */
//	END_FRAME_DETECT: {
//		return;
//	}
//
//	// 记录本次事件的时间
//	// TODO
//}
