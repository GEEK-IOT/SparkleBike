/*
 * wifimanager.h
 *
 *  Created on: 2015-08-06 00:28:29
 *      Author: Cocoonshu
 */

#ifndef APP_INCLUDE_COSMART_WIFIMANAGER_H_
#define APP_INCLUDE_COSMART_WIFIMANAGER_H_

#include "cosmart/cosmart.h"

/**
 * 初始化WiFi组件
 */
void WiFi_initialize();

/**
 * 设置为AP & STA混合模式
 */
void WiFi_setupAPAndSTA();

/**
 * 设置AP模式接入监听
 */
void WiFi_setupAPRecevier();

/**
 * 设置控制协议桥
 */
void WiFi_setupProtocolBridge();

void WiFi_connectAP();

void WiFi_disconnectAP();

#endif /* APP_INCLUDE_COSMART_WIFIMANAGER_H_ */
