/*
 * commandqueue.h
 *
 *  Created on: 2016年6月28日
 *      Author: Cocoonshu
 */

#ifndef APP_INCLUDE_COSMART_COMMUNICATION_COMMANDQUEUE_H_
#define APP_INCLUDE_COSMART_COMMUNICATION_COMMANDQUEUE_H_

#include "cosmart/communication/cmdprotocol.h"
#include "cosmart/communication/commandserver.h"

#define SIG_PROCESS_COMMAND    0x10000001
#define SIG_EVENT_QUEUE_LENGTH 8

struct InnerCommand {
	const char*          command;
	Terminal*            terminal;
	struct InnerCommand* prev;
	struct InnerCommand* next;
};

typedef struct InnerCommand Command;
typedef void (*CommandListener)(Terminal* terminal, const char* command);

void         CMDQueue_initialize();
void         CMDQueue_setCommandListener(CommandListener listener);
void         CMDQueue_startLoop();
void         CMDQueue_stopLoop();
void         CMDQueue_push(Terminal* terminal, const char* command);
void         CMDQueue_pushAtFront(Terminal* terminal, const char* command);
Command*     CMDQueue_pop();
Command*     CMDQueue_popHead();
Command*     CMDQueue_peak();
Command*     CMDQueue_peakHead();
bool         CMDQueue_remove(Terminal* terminal, const char* command);
void         CMDQueue_clear();
unsigned int CMDQueue_size();

#endif /* APP_INCLUDE_COSMART_COMMUNICATION_COMMANDQUEUE_H_ */
