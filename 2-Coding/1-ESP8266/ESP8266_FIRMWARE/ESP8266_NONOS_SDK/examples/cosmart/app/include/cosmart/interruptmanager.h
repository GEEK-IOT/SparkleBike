/*
 * interruptmanager.h
 *
 *  Created on: 2016年1月21日
 *      Author: 80074591
 */

#ifndef APP_INCLUDE_COSMART_INTERRUPTMANAGER_H_
#define APP_INCLUDE_COSMART_INTERRUPTMANAGER_H_
#include "c_types.h"
/**
 * !!! Notice that !!!
 * RULE #1
 *    Interrupt would pause all the functions
 *    including some cases in which irom cache
 *    is disabled (E.G. flash operations).
 *    If cached functions are called, then it
 *    would cause a conflict situation.
 * RULE #2
 *    These is only one interrupt handle function
 *    could be register to system. So do not register
 *    another interrupt handle function and deal
 *    interrupt by register callback in this interrupt
 *    manager.
 */

#define CALLBACK_LIST_SIZE 32

typedef void (*interrupt_callback)(void);
struct GPIOCallback {
	uint32             gpio_pin;
	interrupt_callback callback_handle;
};

void InterruptManager_initialize();
bool InterruptManager_registerInterruptCallback(struct GPIOCallback *callback);
void InterruptManager_unregisterInterruptCallback(struct GPIOCallback *callback);

#endif /* APP_INCLUDE_COSMART_INTERRUPTMANAGER_H_ */
