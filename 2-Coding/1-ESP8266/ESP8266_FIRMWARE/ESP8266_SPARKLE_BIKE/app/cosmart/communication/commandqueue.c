/*
 * commandqueue.c
 *
 *  Created on: 2016年6月28日
 *      Author: Cocoonshu
 */

#include "cosmart/communication/commandqueue.h"
#include "c_types.h"
#include "user_interface.h"
#include "mem.h"
#include "osapi.h"

#ifndef COMPARE
	#define COMPARE(a, b) (a == b)  // (os_strcmp(a, b) == 0)
#endif

LOCAL void onSystemTask(os_event_t* event);

LOCAL Command*        mQueueHead     = NULL;
LOCAL Command*        mQueueTail     = NULL;
LOCAL unsigned int    mQueueSize     = 0;
LOCAL CommandListener mListener      = NULL;
LOCAL os_event_t*     mSIGEventQueue = NULL;
LOCAL bool            mIsLooping     = false;
LOCAL bool            mIsIdling      = false;

void ICACHE_FLASH_ATTR CMDQueue_initialize() {
	Log_printfln("[CMD] Setup command queue");
	if (mSIGEventQueue != NULL) {
		FREEMEM(mSIGEventQueue);
	}
	mSIGEventQueue = (os_event_t*) os_zalloc(sizeof(os_event_t) * SIG_EVENT_QUEUE_LENGTH);
	system_os_task(onSystemTask, USER_TASK_PRIO_0, mSIGEventQueue, SIG_EVENT_QUEUE_LENGTH);
}

void ICACHE_FLASH_ATTR CMDQueue_setCommandListener(CommandListener listener) {
	mListener = listener;
}

void ICACHE_FLASH_ATTR CMDQueue_startLoop() {
	Log_printfln("[CMD] Command queue start loop...");
	mIsLooping = true;
	if (CMDQueue_size() > 0) {
		system_os_post(USER_TASK_PRIO_0, SIG_PROCESS_COMMAND, (os_param_t)NULL);
	}
}

void ICACHE_FLASH_ATTR CMDQueue_stopLoop() {
	Log_printfln("[CMD] Command queue stop loop");
	mIsLooping = false;
}

void ICACHE_FLASH_ATTR CMDQueue_push(Terminal* terminal, const char* command) {
	Command* node = (Command*) os_zalloc(sizeof(Command));
	if (node != NULL) {
		node->command  = command;
		node->terminal = terminal;
		if (mQueueHead == NULL) {
			mQueueSize = 0;
			mQueueHead = mQueueTail = node;
			node->prev = NULL;
			node->next = NULL;
		} else {
			mQueueTail->next = node;
			node->prev       = mQueueTail;
			node->next       = NULL;
			mQueueTail       = node;
		}
		mQueueSize ++;

		if (mIsLooping && mIsIdling) {
			system_os_post(USER_TASK_PRIO_0, SIG_PROCESS_COMMAND, (os_param_t)NULL);
		}
	}
}

void ICACHE_FLASH_ATTR CMDQueue_pushAtFront(Terminal* terminal, const char* command) {
	Command* node = (Command*) os_zalloc(sizeof(Command));
	if (node != NULL) {
		node->command  = command;
		node->terminal = terminal;
		if (mQueueHead == NULL) {
			mQueueSize = 0;
			mQueueHead = mQueueTail = node;
			node->prev = NULL;
			node->next = NULL;
		} else {
			mQueueHead->prev = node;
			node->prev       = NULL;
			node->next       = mQueueHead;
			mQueueHead       = node;
		}
		mQueueSize ++;

		if (mIsLooping && mIsIdling) {
			system_os_post(USER_TASK_PRIO_0, SIG_PROCESS_COMMAND, (os_param_t)NULL);
		}
	}
}

Command* ICACHE_FLASH_ATTR CMDQueue_pop() {
	Command* currPtr = mQueueTail;
	if (currPtr != NULL) {
		mQueueTail = currPtr->prev;
		if (mQueueTail != NULL) {
			mQueueTail->next = NULL;
		}
		mQueueSize = (mQueueSize < 1) ? (0) : (mQueueSize - 1);
	}
	return currPtr;
}

Command* ICACHE_FLASH_ATTR CMDQueue_popHead() {
	Command* currPtr = mQueueHead;
	if (currPtr != NULL) {
		mQueueHead = currPtr->next;
		if (mQueueHead != NULL) {
			mQueueHead->prev = NULL;
		}
		mQueueSize = (mQueueSize < 1) ? (0) : (mQueueSize - 1);
	}
	return currPtr;
}

Command* ICACHE_FLASH_ATTR CMDQueue_peak() {
	return (mQueueTail == NULL) ? (NULL) : (mQueueTail);
}

Command* ICACHE_FLASH_ATTR CMDQueue_peakHead() {
	return (mQueueHead == NULL) ? (NULL) : (mQueueHead);
}

bool ICACHE_FLASH_ATTR CMDQueue_remove(Terminal* terminal, const char* command) {
	Command* currPtr = mQueueHead;
	while (currPtr != NULL) {
		if (terminal == currPtr->terminal && COMPARE(currPtr->command, command)) {
			Command* prevPtr = currPtr->prev;
			Command* nextPtr = currPtr->next;
			if (prevPtr == NULL) {
				mQueueHead = nextPtr;
				mQueueHead->prev = NULL;
			} else {
				prevPtr->next = nextPtr;
			}
			if (nextPtr == NULL) {
				mQueueTail = prevPtr;
				mQueueTail->next = NULL;
			} else {
				nextPtr->prev = prevPtr;
			}
			mQueueSize = (mQueueSize < 1) ? (0) : (mQueueSize - 1);
			FREEMEM(currPtr);
			return true;
		} else {
			currPtr = currPtr->next;
		}
	}
	return false;
}

void ICACHE_FLASH_ATTR CMDQueue_clear() {
	if (mQueueSize > 0) {
		Command* currPtr = mQueueHead;
		Command* nextPtr = NULL;
		mQueueSize = 0;
		mQueueHead = NULL;
		while (currPtr != NULL) {
			nextPtr = currPtr->next;
			FREEMEM(currPtr);
			currPtr = nextPtr;
		}
		mQueueTail = NULL;
	}
}

unsigned int ICACHE_FLASH_ATTR CMDQueue_size() {
	return mQueueSize;
}

LOCAL void onSystemTask(os_event_t* event) {
	switch (event->sig) {
	case SIG_PROCESS_COMMAND:
	{
		if (!mIsLooping) {
			break;
		}
		if (CMDQueue_size() == 0) {
			mIsIdling = true;
			break;
		}
		if (mListener != NULL) {
			Command* command = CMDQueue_pop();
			mListener(command->terminal, command->command);
			FREEMEM(command);
		}
		if (CMDQueue_size() > 0) {
			mIsIdling = false;
			system_os_post(USER_TASK_PRIO_0, SIG_PROCESS_COMMAND, (os_param_t)NULL);
		} else {
			mIsIdling = true;
		}
	}break;
	}
}
