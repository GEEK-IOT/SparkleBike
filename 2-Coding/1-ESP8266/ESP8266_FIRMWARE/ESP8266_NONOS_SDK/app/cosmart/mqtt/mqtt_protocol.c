/*
 * mqtt_protocol.c
 *
 *  Created on: 2016/10/28
 *      Author: Cocoonshu
 */
#include "cosmart/mqtt/mqtt_protocol.h"
#include "cosmart/mqtt/mqtt.h"
#include "cosmart/textutils.h"
#include "osapi.h"
#include "mem.h"

#ifndef DELETE_POINTER
#define DELETE_POINTER(ptr) if (ptr != NULL) {os_free(ptr); ptr = NULL;}
#endif

LOCAL uint16 sPacketIndentifier = 0;

LOCAL uint8  makeFixedHeaderRemainLengthStream(uint32 length, uint8** stream);

uint8 ICACHE_FLASH_ATTR MQTTProcotol_makeFixedHeader(ProtocolStream* stream, uint8 packetType, uint8 dup, uint8 QoS, uint8 retain, uint32 remainLength) {
	if (stream == NULL) {
		return 0;
	}

	// make remain length stream
	uint8* remainLengthStream     = NULL;
	uint8  remainLengthStreamSize = makeFixedHeaderRemainLengthStream(remainLength, &remainLengthStream);

	// make fixed header stream
	uint8 headerSize = 1;
	uint8 header     = ((packetType << 4) & 0xF0)
				     | ((dup << 3) & 0x08)
				     | ((QoS << 1) & 0x06)
				     | (retain & 0x01);

	// make protocol stream
	int fixedHeaderTotalSize = headerSize + remainLengthStreamSize;
	stream->fixedHeader       = (uint8*) os_malloc(fixedHeaderTotalSize);
	stream->fixedHeaderLength = fixedHeaderTotalSize;
	*stream->fixedHeader      = header;
	do {
		int streamOffset       = fixedHeaderTotalSize - remainLengthStreamSize;
		int remainStreamOffset = remainLengthStreamSize - remainLengthStreamSize;
		*(stream->fixedHeader + streamOffset) = *(remainLengthStream + remainStreamOffset);
		remainLengthStreamSize--;
	} while (remainLengthStreamSize > 0);

	// free resource
	if (remainLengthStream != NULL) {
		os_free(remainLengthStream);
	}

	return fixedHeaderTotalSize;
}

LOCAL uint8 ICACHE_FLASH_ATTR makeFixedHeaderRemainLengthStream(uint32 length, uint8** stream) {
	if (length > MAX_REMAIN_LENGTH) {
		return 0;
	}

	uint8* lengthStream     = NULL;
	uint8  lengthStreamSize = 0;

	// STEP1 remain length
	if (length == 0) {
		lengthStream = (uint8*) os_malloc(1);
		os_memset(lengthStream, 0x00, 1);
		lengthStreamSize = 1;
	} else {
		uint32 content = length;
		do {
			content = content / 0x80;
			lengthStreamSize ++;
		} while (content > 0);

		lengthStream = (uint8*) os_malloc(lengthStreamSize);
		os_memset(lengthStream, 0x00, lengthStreamSize);

		uint8* pointer = lengthStream;
		content = length;
		uint8 byte = 0;
		do {
			byte = (uint8) (content % 0x80);
			content = content / 0x80;
			if (content > 0) {
				byte = byte | 0x80;
			}
			*pointer = byte;
			pointer++;
		} while (content > 0);
	}

	*stream = lengthStream;
	return lengthStreamSize;
}

uint8 ICACHE_FLASH_ATTR MQTTProcotol_generatePacketIndenitifier() {
	return sPacketIndentifier++;
}

uint8 ICACHE_FLASH_ATTR MQTTProcotol_packEncoededStream(ProtocolStream* stream) {
	if (stream == NULL) {
		return 0;
	}

	uint8* pointer   = NULL;
	uint16 totalSize = (stream->fixedHeader != NULL    ? stream->fixedHeaderLength : 0)
			         + (stream->variableHeader != NULL ? stream->variableHeaderLength : 0)
					 + (stream->payload != NULL        ? stream->payloadLength : 0);

	DELETE_POINTER(stream->encodedStream);
	stream->encodedStream = (uint8 *) os_malloc(totalSize);
	stream->encodedStreamLength = totalSize;
	pointer = stream->encodedStream;

	os_memcpy(pointer, stream->fixedHeader,    stream->fixedHeaderLength);    pointer += stream->fixedHeaderLength;
	os_memcpy(pointer, stream->variableHeader, stream->variableHeaderLength); pointer += stream->variableHeaderLength;
	os_memcpy(pointer, stream->payload,        stream->payloadLength);        pointer += stream->payloadLength;

	DELETE_POINTER(stream->fixedHeader);    stream->fixedHeaderLength    = 0;
	DELETE_POINTER(stream->variableHeader); stream->variableHeaderLength = 0;
	DELETE_POINTER(stream->payload);        stream->payloadLength        = 0;

	return totalSize;
}

int ICACHE_FLASH_ATTR MQTTProcotol_encodeConnectPacket(
		ProtocolStream* stream,
        const char*     clientIndentifier,
		const char*     userName,
		const char*     password,
		const char*     willTopic,
		const char*     willMessage,
		bool            cleanSession,
		uint16          keepAliveTimeout) {
	/**
	 * Fixed header
	 *  byte[1]   = packetType[7:4] + dup[3] + QoS[2:1] + retain[0]
	 *  byte[2:5] = remainLength
	 * Variable header
	 *  byte[1]   = protocol name length MSB
	 *  byte[2]   = protocol name length LSB
	 *  byte[3]   = 'M' (0b 0100 1101)
	 *  byte[4]   = 'Q' (0b 0101 0001)
	 *  byte[5]   = 'T' (0b 0101 0100)
	 *  byte[6]   = 'T' (0b 0101 0100)
	 *  byte[7]   = protocol level (4 for 3.1.1)
	 *  byte[8]   = connect flags
	 *              user name flag[7] + password flag[6] + will retain[5] + will QoS[4:3] + will flag[2] + clean session[1] + reserved[0]
	 *  byte[9]   = keep alive MSB
	 *  byte[10]  = keep alive LSB
	 * Pay load
	 *  byte[11]  = client identifier length MSB
	 *  byte[12]  = client identifier length LSB
	 *  byte[13:] = client identifier UTF-8 strings
	 *  byte[14]  = will topic length MSB
	 *  byte[15]  = will topic length LSB
	 *  byte[16:] = will topic UTF-8 strings
	 *  byte[17]  = will message length MSB
	 *  byte[18]  = will message length LSB
	 *  byte[19:] = will message UTF-8 strings
	 *  byte[20]  = user name length MSB
	 *  byte[21]  = user name length LSB
	 *  byte[22:] = user name UTF-8 strings
	 *  byte[23]  = password length MSB
	 *  byte[24]  = password length LSB
	 *  byte[25:] = password UTF-8 strings
	 */
	if (stream == NULL) {
		return 0;
	}

	uint8* utf8_mqtt                 = Text_asciiToUtf8(MQTT_NAME);
	uint8* utf8_clientIdentifier     = Text_asciiToUtf8(clientIndentifier);
	uint8* utf8_willTopic            = Text_asciiToUtf8(willTopic);
	uint8* utf8_willMessage          = Text_asciiToUtf8(willMessage);
	uint8* utf8_userName             = Text_asciiToUtf8(userName);
	uint8* utf8_password             = Text_asciiToUtf8(password);
	uint8  utf8_mqttSize             = Text_utf8Length(utf8_mqtt);
	uint8  utf8_clientIdentifierSize = Text_utf8Length(utf8_clientIdentifier);
	uint8  utf8_willTopicSize        = Text_utf8Length(utf8_willTopic);
	uint8  utf8_willMessageSize      = Text_utf8Length(utf8_willMessage);
	uint8  utf8_userNameSize         = Text_utf8Length(utf8_userName);
	uint8  utf8_passwordSize         = Text_utf8Length(utf8_password);
	uint8  connectFlags              = 0;
	uint16 remainSize                = 0;
	uint16 variableHeaderSize        = 0;
	uint16 payloadSize               = 0;
	uint16 totalSize                 = 0;

	remainSize += Text_utf8Length(utf8_mqtt);
	remainSize += 4; // protocol level(1) + connect flags(1) + keep alive(2)
	variableHeaderSize = remainSize;

	remainSize += utf8_clientIdentifierSize;
	remainSize += utf8_willTopicSize;
	remainSize += utf8_willMessageSize;
	remainSize += utf8_userNameSize;
	remainSize += utf8_passwordSize;
	payloadSize = remainSize - variableHeaderSize;

	/**
	 * user name flag[7] + password flag[6] + will retain[5] + will QoS[4:3] + will flag[2] + clean session[1] + reserved[0]
	 */
	connectFlags = (userName != NULL ? 1 << 7 : 0)
			     | (password != NULL ? 1 << 6 : 0)
				 | (willTopic != NULL ? 1 << 5 : 0)
				 | (willTopic != NULL ? MQTT_QoS_AT_LEAST_ONCE << 3 : 0);

	{// Variable header
		stream->variableHeaderLength = variableHeaderSize;
		DELETE_POINTER(stream->variableHeader);
		stream->variableHeader = (uint8*) os_malloc(stream->variableHeaderLength);
		os_memset(stream->variableHeader, NULL, stream->variableHeaderLength);
		os_memcpy(stream->variableHeader, utf8_mqtt, utf8_mqttSize);
		uint8* pointer = stream->variableHeader + utf8_mqttSize;
		*(pointer + 0) = MQTT_PROTOCOL_LEVEL_3_1_1;
		*(pointer + 1) = connectFlags;
		*(pointer + 2) = keepAliveTimeout >> 8;
		*(pointer + 3) = keepAliveTimeout & 0xFF;
	}

	{// Pay load
		stream->payloadLength = payloadSize;
		DELETE_POINTER(stream->payload);
		stream->payload = (uint8*) os_malloc(stream->payloadLength);
		os_memset(stream->payload, NULL, stream->payloadLength);
		uint8* pointer = stream->payload;
		os_memcpy(pointer, utf8_clientIdentifier, utf8_clientIdentifierSize); pointer += utf8_clientIdentifierSize;
		os_memcpy(pointer, utf8_willTopic,        utf8_willTopicSize);        pointer += utf8_willTopicSize;
		os_memcpy(pointer, utf8_willMessage,      utf8_willMessageSize);      pointer += utf8_willMessageSize;
		os_memcpy(pointer, utf8_userName,         utf8_userNameSize);         pointer += utf8_userNameSize;
		os_memcpy(pointer, utf8_password,         utf8_passwordSize);         pointer += utf8_passwordSize;
	}

	{// Fress resources
		DELETE_POINTER(utf8_mqtt);
		DELETE_POINTER(utf8_clientIdentifier);
		DELETE_POINTER(utf8_willTopic);
		DELETE_POINTER(utf8_willMessage);
		DELETE_POINTER(utf8_userName);
		DELETE_POINTER(utf8_password);
	}

	MQTTProcotol_makeFixedHeader(stream, MQTT_CONNECT, false, MQTT_QoS_AT_MOST_ONCE, false, remainSize);
	totalSize = MQTTProcotol_packEncoededStream(stream);
	return totalSize;
}

int ICACHE_FLASH_ATTR MQTTProcotol_decodeConnectAckPacket(ProtocolStream* stream, MQTTSession* session) {
	/**
	 * Fixed header
	 *  byte[1]   = packetType[7:4] + dup[3] + QoS[2:1] + retain[0]
	 *  byte[2]   = remainLength (2 for ConnectAck)
	 * Variable header
	 *  byte[1]   = reserved[7:1] + session present[0]
	 *  byte[2]   = connect return code
	 *
	 *  return @sessionPresent is the current session state in server
	 *         @connectReturnCode is connection acknowledge code
	 */
	if (stream == NULL) {
		return MQTT_STREAM_INVALID;
	}
	if (stream->encodedStream == NULL) {
		return MQTT_STREAM_INVALID;
	}
	if (stream->encodedStreamLength != 4) {
		return MQTT_PROTOCOL_ILLEGAL;
	}

	DELETE_POINTER(stream->fixedHeader);    stream->fixedHeaderLength    = 0;
	DELETE_POINTER(stream->variableHeader); stream->variableHeaderLength = 0;
	DELETE_POINTER(stream->payload);        stream->payloadLength        = 0;

	uint8 flags      = 0;
	uint8 returnCode = 0;
	stream->fixedHeaderLength = 1;
//	stream->fixedHeader = (FixHeader*) os_malloc(stream->fixedHeaderLength);
//	os_memcpy(stream->fixedHeader, stream->encodedStream, stream->fixedHeaderLength);
	flags      = *(stream->encodedStream + stream->fixedHeaderLength);
	returnCode = *(stream->encodedStream + stream->fixedHeaderLength + 1);

	DELETE_POINTER(stream->encodedStream); stream->encodedStreamLength = 0;

	if ((flags & 0b11111110) == 0) {
		session->sessionPresent = flags & 0b00000001;
		session->connectAck     = returnCode;
	} else {
		return MQTT_PROTOCOL_ILLEGAL;
	}
}

int ICACHE_FLASH_ATTR MQTTProcotol_encodeDisconnectPacket(ProtocolStream* stream) {
	/**
	 * Fixed header
	 *  byte[1]   = packetType[7:4] + dup[3] + QoS[2:1] + retain[0]
	 *            = {
	 *                  packetType = 14
	 *                  dup        = 0
	 *                  QoS        = 0
	 *                  retain     = 0
	 *               }
	 *  byte[2]   = remainLength (0 for Disconnect)
	 */
}

int ICACHE_FLASH_ATTR MQTTProcotol_encodePingPacket(ProtocolStream* stream) {
	/**
	 * Fixed header
	 *  byte[1]   = packetType[7:4] + dup[3] + QoS[2:1] + retain[0]
	 *            = {
	 *                  packetType = 12
	 *                  dup        = 0
	 *                  QoS        = 0
	 *                  retain     = 0
	 *               }
	 *  byte[2]   = remainLength (0 for Disconnect)
	 */
}

int ICACHE_FLASH_ATTR MQTTProcotol_decodePingAckPacket(ProtocolStream* stream) {
	/**
	 * Fixed header
	 *  byte[1]   = packetType[7:4] + dup[3] + QoS[2:1] + retain[0]
	 *            = {
	 *                  packetType = 13
	 *                  dup        = 0
	 *                  QoS        = 0
	 *                  retain     = 0
	 *               }
	 *  byte[2]   = remainLength (0 for Disconnect)
	 */
}
