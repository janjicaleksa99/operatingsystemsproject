/*
 * Idle.h
 *
 *  Created on: Aug 7, 2020
 *      Author: OS1
 */

#ifndef IDLE_H_
#define IDLE_H_

#include "thread.h"

class Idle: public Thread {
public:
	Idle(StackSize stackSize, Time timeSlice);
	Idle();
	virtual ~Idle();
	PCB* retPCB() {
		return myPCB;
	}
	void run();
};

#endif /* IDLE_H_ */
