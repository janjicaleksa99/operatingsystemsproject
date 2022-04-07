/*
 * Test1.h
 *
 *  Created on: Aug 10, 2020
 *      Author: OS1
 */

#ifndef TEST1_H_
#define TEST1_H_

#include "thread.h"

class Test1: public Thread {
public:
	Test1(StackSize stackSize, Time timeSlice);
	Test1();
	virtual ~Test1();
	void run();
};

#endif /* TEST1_H_ */
