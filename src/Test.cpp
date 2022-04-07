/*
 * Test.cpp
 *
 *  Created on: Aug 4, 2020
 *      Author: OS1
 */

#include "Test.h"
#include "SCHEDULE.H"
#include "Semaphor.h"
#include <iostream.h>

extern volatile int lockFlag;
extern Semaphore *sem1;
Semaphore *sem = new Semaphore();
volatile int brojac1 = 0;

Test::Test(StackSize stackSize, Time timeSlice): Thread(stackSize, timeSlice) {
	// TODO Auto-generated constructor stub

}

Test::Test() : Thread() {

}

Test::~Test() {
	// TODO Auto-generated destructor stub
}

void Test::run()  {
	for (int i =0; i < 5; ++i) {
		cout<<"u a() ="<< i <<endl;

			for (int k = 0; k<10000; ++k)
				for (int j = 0; j <30000; ++j);
		}
}
