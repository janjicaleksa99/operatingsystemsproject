/*
 * Test.h
 *
 *  Created on: Aug 4, 2020
 *      Author: OS1
 */

#ifndef TEST_H_
#define TEST_H_

#include "thread.h"

class Test: public Thread {
public:
	Test(StackSize stackSize, Time timeSlice);
	Test();
	virtual ~Test();
	void run();
};

#endif /* TEST_H_ */
