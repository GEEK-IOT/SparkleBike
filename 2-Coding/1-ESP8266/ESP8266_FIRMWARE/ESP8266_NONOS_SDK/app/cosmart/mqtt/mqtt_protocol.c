/*
 * mqtt_protocol.c
 *
 *  Created on: 2016年10月28日
 *      Author: oppo
 */
#include "cosmart/mqtt/mqtt_protocol.h"
#include "cosmart/mqtt/mqtt.h"
#include "cosmart/textutils.h"
#include "mem.h"

#define DELETE_POINTER(ptr) if (ptr != NULL) {os_free(ptr); ptr == NULL;}

LOCAL uint16 sPacketIndentifier = 0;

LOCAL uint8  makeFixedHeaderRemainLengthStream(uint32 length, uint8** stream);

uint8 ICACHE_FLASH_ATTR MQTTProcotol_MakeFixedHeader(ProtocolStream* stream, uint8 packetType, uint8 dup, uint8 QoS, uint8 retain, uint32 remainLength) {
	if (stream == NULL) {
		return 0;
	}

	// make remain length stream
	uint8* remainLengthStream     = NULL;
	uint8  remainLengthStreamSize = makeFixedHeaderRemainLengthStream(remainLength, &remainLengthStream);

	// make fixed header stream
	int        headerSize = sizeof(FixHeader);
	FixHeader* header     = (FixHeader *) os_malloc(headerSize);
	os_memset(header, NULL, headerSize);
	header->packetType = packetType;
	header->dup        = dup;
	header->QoS        = QoS;
	header->retain     = retain;

	// make protocol stream
	int fixedHeaderTotalSize = headerSize + remainLengthStreamSize;
	stream->fixedHeader       = (uint8*) os_malloc(fixedHeaderTotalSize);
	stream->fixedHeaderLength = fixedHeaderTotalSize;
	os_memcpy(stream->fixedHeader, header, headerSize);
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
	if (header != NULL) {
		os_free(header);
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

uint8 ICACHE_FLASH_ATTR MQTTProcotol_GeneratePacketIndenitifier() {
	return sPacketIndentifier++;
}

uint8 ICACHE_FLASH_ATTR MQTTProcotol_EncodeConnectPacket(
		ProtocolStream* stream,
		bool            cleanSession,
        const char*     clientIndentifier,
		const char*     userName,
		const char*     password,
		const char*     willTopic,
		const char*     willMessage,
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

	uint8* utf8_mqtt             = Text_asciiToUtf8(MQTT_NAME);
	uint8* utf8_clientIdentifier = Text_asciiToUtf8(clientIndentifier);
	uint8* utf8_willTopic        = Text_asciiToUtf8(willTopic);
	uint8* utf8_willMessage      = Text_asciiToUtf8(willMessage);
	uint8* utf8_userName         = Text_asciiToUtf8(userName);
	uint8* utf8_password         = Text_asciiToUtf8(password);
	uint16 remainSize            = 0;
	uint16 variableHeaderSize    = 0;
	uint16 payloadSize           = 0;
	uint8  connectFlags          = 0;

	remainSize += Text_utf8Length(utf8_mqtt);
	remainSize += 4; // protocol level(1) + connect flags(1) + keep alive(2)
	variableHeaderSize = remainSize;

	remainSize += Text_utf8Length(utf8_clientIdentifier);
	remainSize += Text_utf8Length(utf8_willTopic);
	remainSize += Text_utf8Length(utf8_willMessage);
	remainSize += Text_utf8Length(utf8_userName);
	remainSize += Text_utf8Length(utf8_password);
	payloadSize = remainSize - variableHeaderSize;

	stream->variableHeaderLength = variableHeaderSize;
	DELETE_POINTER(stream->variableHeader);
	stream->variableHeader = (uint8*) os_malloc(stream->variableHeaderLength);
	os_memset(stream->variableHeader, NULL, stream->variableHeaderLength);
	os_memcpy(stream->variableHeader, utf8_clientIdentifier, Text_utf8Length(utf8_mqtt));
	uint8* pointer = stream->variableHeader + Text_utf8Length(utf8_mqtt);
	*(pointer + 0) = PROTOCOL_LEVEL_3_1_1;
	*(pointer + 1) = PROTOCOL_LEVEL_3_1_1; // user name flag[7] + password flag[6] + will retain[5] + will QoS[4:3] + will flag[2] + clean session[1] + reserved[0]
	os_memcpy(pointer + 2, &keepAliveTimeout, 2);

	MQTTProcotol_MakeFixedHeader(stream, MQTT_CONNECT, true, MQTT_QoS_AT_MOST_ONCE, cleanSession, remainSize);
}

uint8 ICACHE_FLASH_ATTR MQTTProcotol_DecodeConnectAckPacket(ProtocolStream* stream, MQTTSession* session) {
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
}

uint8 ICACHE_FLASH_ATTR MQTTProcotol_EncodeDisconnectPacket(ProtocolStream* stream) {
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

uint8 ICACHE_FLASH_ATTR MQTTProcotol_EncodePingPacket(ProtocolStream* stream) {
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

uint8 ICACHE_FLASH_ATTR MQTTProcotol_DecodePingAckPacket(ProtocolStream* stream) {
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
