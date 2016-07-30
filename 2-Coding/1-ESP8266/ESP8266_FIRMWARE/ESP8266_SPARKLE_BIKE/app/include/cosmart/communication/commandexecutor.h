/*
 * commandexecutor.h
 *
 *  Created on: 2016年7月3日
 *      Author: Cocoonshu
 */

#ifndef APP_INCLUDE_COSMART_COMMUNICATION_COMMANDEXECUTOR_H_
#define APP_INCLUDE_COSMART_COMMUNICATION_COMMANDEXECUTOR_H_

#include "cosmart/communication/cmdprotocol.h"
#include "cosmart/communication/commandserver.h"

#ifndef MEMZERO
	#define MEMZERO(ptr, len) {if (ptr != NULL) {os_bzero(ptr, len);}}
#endif

#ifndef FREEMEM
	#define FREEMEM(ptr) {if (ptr != NULL) {os_free(ptr);}}
#endif

typedef bool  (Decoder) (const char* command, Request* inRequest, Response* outResponse);
typedef char* (Encoder) (Request* inRequest, Response* outResponse);
typedef void  (Executor)(Request* inRequest, Response* outResponse);

typedef struct {
	const char* name;
	Executor*   executor;
	Decoder*    decoder;
	Encoder*    encoder;
} Method;

/**
 * 解析Command为Request和Response
 */
bool          CMDDecoder_command(const char* command, Request** inRequest, Response** outResponse);

bool          CMDDecoder_conect(const char* command, Request* inRequest, Response* outResponse);
bool          CMDDecoder_disconnect(const char* command, Request* inRequest, Response* outResponse);
bool          CMDDecoder_getSystemInfo(const char* command, Request* inRequest, Response* outResponse);
bool          CMDDecoder_joinNetwork(const char* command, Request* inRequest, Response* outResponse);
bool          CMDDecoder_reboot(const char* command, Request* inRequest, Response* outResponse);

char*          CMDEncoder_conect(Request* inRequest, Response* outResponse);
char*          CMDEncoder_disconnect(Request* inRequest, Response* outResponse);
char*          CMDEncoder_getSystemInfo(Request* inRequest, Response* outResponse);
char*          CMDEncoder_joinNetwork(Request* inRequest, Response* outResponse);
char*          CMDEncoder_reboot(Request* inRequest, Response* outResponse);

void          CMDExecutor_conect(Request* inRequest, Response* outResponse);
void          CMDExecutor_disconnect(Request* inRequest, Response* outResponse);
void          CMDExecutor_getSystemInfo(Request* inRequest, Response* outResponse);
void          CMDExecutor_joinNetwork(Request* inRequest, Response* outResponse);
void          CMDExecutor_reboot(Request* inRequest, Response* outResponse);

const int     CMDExecutor_getMethodSize();
const Method* CMDExecutor_getMethods();

#endif /* APP_INCLUDE_COSMART_COMMUNICATION_COMMANDEXECUTOR_H_ */
