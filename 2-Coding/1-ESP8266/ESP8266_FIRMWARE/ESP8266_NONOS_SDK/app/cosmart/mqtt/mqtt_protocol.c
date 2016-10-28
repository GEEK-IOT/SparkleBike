/*
 * mqtt_protocol.c
 *
 *  Created on: 2016年10月28日
 *      Author: oppo
 */
#include "cosmart/mqtt/mqtt_protocol.h"
#include "cosmart/mqtt/mqtt.h"
#include "mem.h"

LOCAL uint8 makeFixedHeaderRemainLengthStream(uint32 length, uint8** stream);

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

LOCAL uint8 ICACHE_FLASH_ATTR MQTTProcotol_GeneratePacketIndenitifier() {

}
