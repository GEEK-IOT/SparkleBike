/*
 * mqtt_protocol.h
 *
 *  Created on: 2016/10/28
 *      Author: Cocoonshu
 *
 *  The flags in Fixed header
 *  ===============================================
 *  type              Bit3 Bit2 Bit1 Bit0     Combo
 *  -----------------------------------------------
 *  CONNECT           0    0    0    0        0x10
 *  CONNACK           0    0    0    0        0x20
 *  PUBLISH           DUP  QoS  QoS  Retain > 0x30
 *  PUBACK            0    0    0    0        0x40
 *  PUBREC            0    0    0    0        0x50
 *  PUBREL            0    0    0    0        0x60
 *  PUBCOMP           0    0    0    0        0x70
 *  SUBSCRIBE         0    0    0    0        0x80
 *  SUBACK            0    0    0    0        0x90
 *  UNSUBSCRIBE       0    0    0    0        0xA0
 *  UNSUBACK          0    0    0    0        0xB0
 *  PINGREQ           0    0    0    0        0xC0
 *  PINGRESP          0    0    0    0        0xD0
 *  DISCONNECT        0    0    0    0        0xE0
 *  ===============================================
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
