/*
 * mqtt.h
 *
 *  Created on: 2016年10月28日
 *      Author: oppo
 */

#ifndef APP_INCLUDE_COSMART_MQTT_MQTT_H_
#define APP_INCLUDE_COSMART_MQTT_MQTT_H_

#include "c_types.h"

#define PROTOCOL_LEVEL_3_1_1 0x04
#define MAX_REMAIN_LENGTH    268435455
#define MQTT_NAME            "MQTT"

enum QoS {
	MQTT_QoS_AT_MOST_ONCE  = 0x00,
	MQTT_QoS_AT_LEAST_ONCE = 0x01,
	MQTT_QoS_ONLY_ONCE     = 0x02
};

enum ResponseCode {
	RES_CODE_CONNECT_COMFIRMED            = 0x00,
	RES_CODE_UNSUPPORTED_PROTOCOL_LEVEL   = 0x01,
	RES_CODE_ILLEGAL_CLIENT_IDENTIFIER    = 0x02,
	RES_CODE_SERVER_INVALIDATED           = 0x03,
	RES_CODE_INVALID_USERNAME_OR_PASSWORD = 0x04,
	RES_CODE_UNAUTHORIZATED               = 0x05
};

enum PacketType {
	MQTT_CONNECT           = 1,
	MQTT_CONNECT_ACK       = 2,
	MQTT_PUBLISH           = 3,  // packet identifier
	MQTT_PUBLISH_ACK       = 4,  // packet identifier
	MQTT_PUBLISH_RECEVIED  = 5,  // packet identifier
	MQTT_PUBLISH_RELEASE   = 6,  // packet identifier
	MQTT_PUBLISH_COMPLETED = 7,  // packet identifier
	MQTT_SUBSCRIBE         = 8,  // packet identifier
	MQTT_SUBSCRIBE_ACK     = 9,  // packet identifier
	MQTT_UNSUBSCRIBE       = 10, // packet identifier
	MQTT_UNSUBSCRIBE_ACK   = 11, // packet identifier
	MQTT_PING              = 12,
	MQTT_PING_ACK          = 13,
	MQTT_DISCONNECT        = 14
};

typedef struct {
	uint8 protocolLevel;
} MQTTClient;

const char* MQTT_GetClientIndentifier();

#endif /* APP_INCLUDE_COSMART_MQTT_MQTT_H_ */
