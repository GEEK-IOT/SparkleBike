/*
 * commandparser.c
 *
 *  Created on: 2016年7月4日
 *      Author: Cocoonshu
 */

#include "cosmart/communication/commandexecutor.h"
#include "osapi.h"
#include "c_types.h"
#include "cosmart/json/cJSON.h"
#include "mem.h"
#include "cosmart/textutils.h"
#include "cosmart/cosmart.h"

bool ICACHE_FLASH_ATTR CMDDecoder_command(const char* command, Request** inRequest, Response** outResponse) {
	/*
	 *    request ->
	 *     {
	 *         requstID      : "xxxx",
	 *         method        : "xxxx",
	 *         parameters    : {}
	 *     }
	 */
//	bool   result = true;
//	cJSON* jsRoot = cJSON_Parse(command);
//	if (jsRoot != NULL) {
//		(*inRequest)   = (Request *) os_zalloc(sizeof(Request));
//		if ((*inRequest) != NULL) {
//			MEMZERO(*inRequest, sizeof(Request));
//			result &= true;
//		} else {
//			result &= false;
//		}
//
//		(*outResponse) = (Response *) os_zalloc(sizeof(Response));
//		if ((*outResponse) != NULL) {
//			MEMZERO(*outResponse, sizeof(Response));
//			result &= true;
//		} else {
//			result &= false;
//		}
//
//		cJSON* jsRequestID = cJSON_GetObjectItem(jsRoot, KEY_REQUEST_ID);
//		if (jsRequestID != NULL) {
//			(*inRequest)->requestID    = jsRequestID->valuestring;
//			(*outResponse)->responseID = jsRequestID->valuestring;
//			cJSON_Delete(jsRequestID);
//			result &= true;
//		} else {
//			result &= false;
//		}
//
//		cJSON* jsMethod = cJSON_GetObjectItem(jsRoot, KEY_METHOD);
//		if (jsMethod != NULL) {
//			(*inRequest)->method = jsMethod->valuestring;
//			cJSON_Delete(jsMethod);
//			result &= true;
//		} else {
//			result &= false;
//		}
//
//		cJSON_Delete(jsRoot);
//	} else {
//		result &= false;
//	}
//
//	return result;
	return false;
}

bool ICACHE_FLASH_ATTR CMDDecoder_conect(const char* command, Request* inRequest, Response* outResponse) {
//	bool   result = true;
//	cJSON* jsRoot = cJSON_Parse(command);
//	if (jsRoot != NULL) {
//		/*     request ->
//		 *     {
//		 *         requstID      : "0001",
//		 *         method        : "login",
//		 *         parameters    : {
//		 *             broadcastIP   : "192.168.4.2",
//		 *             broadcastPort : 7629,
//		 *             platform      : "cosmart.android.commander",
//		 *             commandPort   : 7630,
//		 *             securityCode  : "j1c81jslf"
//		 *         }
//		 *     }
//		 */
//		// Fill Response->result
//		outResponse->result->response = (ConnectResponse *) os_zalloc(sizeof(ConnectResponse));
//		MEMZERO(outResponse->result->response, sizeof(ConnectResponse));
//
//		// Fill Request->parameters
//		cJSON* jsParameters = cJSON_GetObjectItem(jsRoot, KEY_PARAMETERS);
//		if (jsParameters != NULL) {
//			ConnectRequest* request = (ConnectRequest*) os_zalloc(sizeof(ConnectRequest));
//			inRequest->parameters->request = request;
//			if (request != NULL) {
//				MEMZERO(inRequest->parameters, sizeof(ConnectRequest));
//				result &= true;
//			} else {
//				result &= false;
//			}
//
//			cJSON* jsBroadcastIP = cJSON_GetObjectItem(jsParameters, KEY_BROADCAST_IP);
//			if (jsBroadcastIP != NULL) {
//				char* ipString       = jsBroadcastIP->valuestring;
//				int   ipStringLenght = os_strlen(ipString);
//				request->broadcastIP = Text_parseIPAddressString(ipString, 0, ipStringLenght);
//				FREEMEM(ipString);
//				cJSON_Delete(jsBroadcastIP);
//				result &= true;
//			} else {
//				result &= false;
//			}
//
//			cJSON* jsBroadcastPort = cJSON_GetObjectItem(jsParameters, KEY_BROADCAST_PORT);
//			if (jsBroadcastPort != NULL) {
//				request->broadcastPort = jsBroadcastPort->valueint;
//				cJSON_Delete(jsBroadcastPort);
//				result &= true;
//			} else {
//				result &= false;
//			}
//
//			cJSON* jsPlatform = cJSON_GetObjectItem(jsParameters, KEY_PLATFORM);
//			if (jsPlatform != NULL) {
//				request->platform = jsPlatform->valuestring;
//				cJSON_Delete(jsPlatform);
//				result &= true;
//			} else {
//				result &= false;
//			}
//
//			cJSON* jsCommandPort = cJSON_GetObjectItem(jsParameters, KEY_COMMAND_PORT);
//			if (jsCommandPort != NULL) {
//				request->commandPort = jsCommandPort->valueint;
//				cJSON_Delete(jsCommandPort);
//				result &= true;
//			} else {
//				result &= false;
//			}
//
//			cJSON* jsSecurityCode = cJSON_GetObjectItem(jsParameters, KEY_SECURITY_CODE);
//			if (jsSecurityCode != NULL) {
//				request->securityCode = jsSecurityCode->valuestring;
//				cJSON_Delete(jsSecurityCode);
//				result &= true;
//			} else {
//				result &= false;
//			}
//
//			cJSON_Delete(jsParameters);
//		}
//
//		cJSON_Delete(jsRoot);
//	} else {
//		result &= false;
//	}
//
//	return result;
	return false;
}

char* ICACHE_FLASH_ATTR CMDEncoder_conect(Request* inRequest, Response* outResponse) {
	/*
	 *     response ->
	 *     {
	 *         responseID    : "0001",
	 *         return        : true,
	 *         responseState : {
	 *             stateCode        : 0,
	 *             stateMessage     : "IllegalParameterException",
	 *             stateDescription : "at parameter broadcastPort: 7629"
	 *         }
	 *     }
	 */
	if (outResponse == NULL) {
		return NULL;
	}

	char* json;
	cJSON* jsRoot = cJSON_CreateObject();
	if (jsRoot != NULL) {
		cJSON* jsResponseState = cJSON_CreateObject();
		cJSON_AddNumberToObject(jsResponseState, KEY_STATE_CODE, outResponse->requestState->stateCode);
		cJSON_AddStringToObject(jsResponseState, KEY_STATE_MESSAGE, outResponse->requestState->stateMessage);
		cJSON_AddStringToObject(jsResponseState, KEY_STATE_DESCRIPTION, outResponse->requestState->stateDescription);

		cJSON_AddStringToObject(jsRoot, KEY_RESPONSE_ID, outResponse->responseID);
		cJSON_AddBoolToObject(jsRoot, KEY_RETURN, outResponse->result->response->result);
		cJSON_AddItemReferenceToObject(jsRoot, KEY_RESPONSE_STATE, jsResponseState);

		json = cJSON_Print(jsRoot);
		cJSON_Delete(jsRoot);
		cJSON_Delete(jsResponseState);
	} else {
		json = NULL;
	}

	return json;
}

bool ICACHE_FLASH_ATTR CMDDecoder_disconnect(const char* command, Request* inRequest, Response* outResponse) {
	return false;
}

char* ICACHE_FLASH_ATTR CMDEncoder_disconnect(Request* inRequest, Response* outResponse) {
	return NULL;
}

bool ICACHE_FLASH_ATTR CMDDecoder_getSystemInfo(const char* command, Request* inRequest, Response* outResponse) {
	return false;
}

char* ICACHE_FLASH_ATTR CMDEncoder_getSystemInfo(Request* inRequest, Response* outResponse) {
	return NULL;
}

bool ICACHE_FLASH_ATTR CMDDecoder_joinNetwork(const char* command, Request* inRequest, Response* outResponse) {
	return false;
}

char* ICACHE_FLASH_ATTR CMDEncoder_joinNetwork(Request* inRequest, Response* outResponse) {
	return NULL;
}

bool ICACHE_FLASH_ATTR CMDDecoder_reboot(const char* command, Request* inRequest, Response* outResponse) {
	return false;
}

char* ICACHE_FLASH_ATTR CMDEncoder_reboot(Request* inRequest, Response* outResponse) {
	return NULL;
}
