/*
 * mqtt_protocol.h
 *
 *  Created on: 2016年10月28日
 *      Author: oppo
 */

#ifndef APP_INCLUDE_COSMART_MQTT_MQTT_PROTOCOL_H_
#define APP_INCLUDE_COSMART_MQTT_MQTT_PROTOCOL_H_

#include "c_types.h"

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

uint8 MQTTProcotol_MakeFixedHeader(ProtocolStream* stream, uint8 packetType, uint8 dup, uint8 QoS, uint8 retain, uint32 remainLength);
uint8 MQTTProcotol_GeneratePacketIndenitifier();
uint8 MQTTProcotol_packEncoededStream(ProtocolStream* stream);

uint8 MQTTProcotol_EncodeConnectPacket(ProtocolStream* stream, const char* clientIndentifier, bool cleanSession, const char* userName, const char* password, const char* willTopic, const char* willMessage, uint16 keepAliveTimeout);
uint8 MQTTProcotol_DecodeConnectAckPacket(ProtocolStream* stream, MQTTSession* session);
uint8 MQTTProcotol_CreatePublishPacket(ProtocolStream* stream);
uint8 MQTTProcotol_CreatePublishAclPacket(ProtocolStream* stream);
uint8 MQTTProcotol_CreatePublishReceivedPacket(ProtocolStream* stream);
uint8 MQTTProcotol_CreatePublishReleasePacket(ProtocolStream* stream);
uint8 MQTTProcotol_CreatePublishCompleletedPacket(ProtocolStream* stream);
uint8 MQTTProcotol_CreateSubscribePacket(ProtocolStream* stream);
uint8 MQTTProcotol_CreateSubscribeAckPacket(ProtocolStream* stream);
uint8 MQTTProcotol_CreateUnsubscribePacket(ProtocolStream* stream);
uint8 MQTTProcotol_CreateUnsubscribeAckPacket(ProtocolStream* stream);
uint8 MQTTProcotol_EncodePingPacket(ProtocolStream* stream);
uint8 MQTTProcotol_DecodePingAckPacket(ProtocolStream* stream);
uint8 MQTTProcotol_EncodeDisconnectPacket(ProtocolStream* stream);

#endif /* APP_INCLUDE_COSMART_MQTT_MQTT_PROTOCOL_H_ */
