/*
 * Semaphor.cpp
 *
 *  Created on: Aug 11, 2020
 *      Author: OS1
 */

#include "Semaphor.h"
#include "KerneSem.h"
#include <iostream.h>

extern volatile int lockFlag;
extern volatile PCB* running;
extern void lock();
extern void unlock();
volatile int Tajmer = 0;
volatile int flagT = 0;

Semaphore::Semaphore(int init) {
	myImpl = new KerneSem(this);
	myImpl->setval(init);
}

Semaphore::~Semaphore() {
	delete myImpl;
}

int Semaphore::wait (Time maxTimeToWait) {
	lock();
	int ret = 1;
	if (maxTimeToWait > 0) {
		if (--*myImpl < 0) {
			running->MaxTimeToWait = maxTimeToWait;
			//printf("MaxTimeToWait je %d od niti %d \n", running->MaxTimeToWait, running->PID);
			running->CurrentWaitTime = 0;
			myImpl->block(1);
			ret = 0;
		}
	}
	else if (maxTimeToWait == 0) {
		if (--*myImpl < 0)
			myImpl->block(0);
	}
	unlock();
	return ret;
}

int Semaphore::signal(int n) {
	lock();
	int ret;
	if (n == 0) {
		if ((*myImpl)++ < 0)
			myImpl->unblock();
		ret = 0;
	}
	else if (n > 0) {
		int i;
		if (n > (-myImpl->val()))
			for (i = 0; i < -myImpl->val(); i++)
				myImpl->unblock();
		else
			for (i = 0; i < n; i++)
				myImpl->unblock();
		myImpl->setval(myImpl->val() + n);
		ret = i; //Broj odblokiranih niti
	}
	else
		ret = n;
	unlock();
	return ret;
}

int Semaphore::val() const {
	return myImpl->val();
}
