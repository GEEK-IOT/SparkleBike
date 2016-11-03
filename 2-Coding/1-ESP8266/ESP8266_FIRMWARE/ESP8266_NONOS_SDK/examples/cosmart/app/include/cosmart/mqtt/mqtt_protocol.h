/*
 * mqtt_protocol.h
 *
 *  Created on: 2016年10月28日
 *      Author: oppo
 */

#ifndef APP_INCLUDE_COSMART_MQTT_MQTT_PROTOCOL_H_
#define APP_INCLUDE_COSMART_MQTT_MQTT_PROTOCOL_H_

#include "c_types.h"

#define MQTT_STREAM_INVALID    0 // ignore
#define MQTT_PROTOCOL_ILLEGAL -1 // should disconnect

typedef struct {
	uint8  protocolLevel;
	uint16 keepAlive;
	char*  clientIdentifier;
	char*  userName;
	char*  password;

	uint16 connectAck;
	bool   sessionPresent;
} MQTTSession;

#pragma pack (1)

/**
 * FixedHeader {
 *   uint8 packetType   : 4;
 *   uint8 dup          : 1;
 *   uint8 QoS          : 2;
 *   uint8 retain       : 1;
 *   uint8 remainLength : 8 ~ 32;
 * }
 */
typedef struct {
	uint8 packetType : 4;
	uint8 dup        : 1;
	uint8 QoS        : 2;
	uint8 retain     : 1;
} FixHeader;

typedef struct {
	uint8*  fixedHeader;
	uint8   fixedHeaderLength;

	uint8*  variableHeader;
	uint8   variableHeaderLength;

	uint8*  payload;
	uint32  payloadLength;

	uint8*  encodedStream;
	uint32  encodedStreamLength;
} ProtocolStream;

#pragma pack ()

uint8 MQTTProcotol_makeFixedHeader(ProtocolStream* stream, uint8 packetType, uint8 dup, uint8 QoS, uint8 retain, uint32 remainLength);
uint8 MQTTProcotol_generatePacketIndenitifier();
uint8 MQTTProcotol_packEncoededStream(ProtocolStream* stream);

int   MQTTProcotol_encodeConnectPacket(ProtocolStream* stream, const char* clientIndentifier, const char* userName, const char* password, const char* willTopic, const char* willMessage, bool cleanSession, uint16 keepAliveTimeout);
int   MQTTProcotol_decodeConnectAckPacket(ProtocolStream* stream, MQTTSession* session);
int   MQTTProcotol_createPublishPacket(ProtocolStream* stream);
int   MQTTProcotol_createPublishAclPacket(ProtocolStream* stream);
int   MQTTProcotol_createPublishReceivedPacket(ProtocolStream* stream);
int   MQTTProcotol_createPublishReleasePacket(ProtocolStream* stream);
int   MQTTProcotol_createPublishCompleletedPacket(ProtocolStream* stream);
int   MQTTProcotol_createSubscribePacket(ProtocolStream* stream);
int   MQTTProcotol_createSubscribeAckPacket(ProtocolStream* stream);
int   MQTTProcotol_createUnsubscribePacket(ProtocolStream* stream);
int   MQTTProcotol_createUnsubscribeAckPacket(ProtocolStream* stream);
int   MQTTProcotol_encodePingPacket(ProtocolStream* stream);
int   MQTTProcotol_decodePingAckPacket(ProtocolStream* stream);
int   MQTTProcotol_encodeDisconnectPacket(ProtocolStream* stream);

#endif /* APP_INCLUDE_COSMART_MQTT_MQTT_PROTOCOL_H_ */
