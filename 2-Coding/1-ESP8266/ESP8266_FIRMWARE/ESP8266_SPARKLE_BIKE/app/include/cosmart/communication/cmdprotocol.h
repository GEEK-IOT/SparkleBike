/*
 * cmdprotocol.h
 *
 *  Created on: 2016年6月16日
 *      Author: Cocoonshu
 */

#ifndef APP_INCLUDE_COSMART_COMMUNICATION_CMDPROTOCOL_H_
#define APP_INCLUDE_COSMART_COMMUNICATION_CMDPROTOCOL_H_

#include "c_types.h"

enum StateCode {
	State_OK                           = 0,
	State_IncompletedRequestException  = -100,
	State_IllegalParameterException    = -101,
	State_UnimplementedMethodException = -102,
	State_OutOfMemoryError             = -103
};

typedef struct {
	uint32      broadcastIP;
	uint32      broadcastPort;
	uint32      commandPort;
	const char* platform;
	const char* securityCode;
} ConnectRequest;

typedef struct {
	bool result;
} ConnectResponse;

typedef struct {
	const char* requestID;
	const char* method;
	union {
		ConnectRequest* request;
	}* parameters;
} Request;


typedef struct {
	int         stateCode;
	const char* stateMessage;
	const char* stateDescription;
} RequestState;
typedef struct {
	const  char*   responseID;
	RequestState*  requestState;
	union {
		ConnectResponse* response;
	}* result;
} Response;

#endif /* APP_INCLUDE_COSMART_COMMUNICATION_CMDPROTOCOL_H_ */
