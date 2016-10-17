/*
 * wifimanager.h
 *
 *  Created on: 2015-08-06 00:28:29
 *      Author: Cocoonshu
 */

#ifndef APP_INCLUDE_COSMART_WIFIMANAGER_H_
#define APP_INCLUDE_COSMART_WIFIMANAGER_H_

#include "cosmart/cosmart.h"

void        WiFi_initialize();
void        WiFi_setupAPAndSTA();
void        WiFi_setupAPRecevier();
void        WiFi_setupProtocolBridge();
void        WiFi_connectAP();
void        WiFi_disconnectAP();
char*       WiFi_generateSTAIdentify();
void        WiFi_freeSTAIdentify(char** identify);
void        WiFi_updateNTPTime();

#endif /* APP_INCLUDE_COSMART_WIFIMANAGER_H_ */
