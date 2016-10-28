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
#define QoS_AT_MOST_ONCE     0x00
#define QoS_AT_LEAST_ONCE    0x01
#define QoS_ONLY_ONCE        0x02
#define KeepSession          0x00
#define CleanSession         0x01
#define MAX_REMAIN_LENGTH    268435455

enum ResponseCode {
	RES_CODE_UNSUPPORTED_PROTOCOL_LEVEL = 0x01
};

enum PacketType {
	CONNECT           = 1,
	CONNECT_ACK       = 2,
	PUBLISH           = 3,  // packet identifier
	PUBLISH_ACK       = 4,  // packet identifier
	PUBLISH_RECEVIED  = 5,  // packet identifier
	PUBLISH_RELEASE   = 6,  // packet identifier
	PUBLISH_COMPLETED = 7,  // packet identifier
	SUBSCRIBE         = 8,  // packet identifier
	SUBSCRIBE_ACK     = 9,  // packet identifier
	UNSUBSCRIBE       = 10, // packet identifier
	SUBSCRIBE_ACK     = 11, // packet identifier
	PING              = 12,
	PING_ACK          = 13,
	DISCONNECT        = 14
};

typedef struct {
	uint8 protocolLevel;
} MQTTClient;

#endif /* APP_INCLUDE_COSMART_MQTT_MQTT_H_ */
