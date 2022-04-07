/*
 * Event.cpp
 *
 *  Created on: Aug 12, 2020
 *      Author: OS1
 */

#include "Event.h"
#include "thread.h"
#include "KernelEv.h"
#include "IVTEntry.h"
#include <iostream.h>

//extern volatile int lockFlag;
extern void lock();
extern void unlock();
extern volatile PCB* running;

Event::Event(IVTNo ivtNo) {
	myImpl = new KernelEv(this);
	IVTEntry* ivt = IVTEntry::retObject(ivtNo);
	ivt->inic(ivt->retRoutine());
	ivt->setKEvent(myImpl);
}

void Event::wait() {
	if (myImpl->ThreadCreator == running) {
		lock();
		myImpl->block();
		unlock();
	}
}

void Event::signal() {
	lock();
	myImpl->unblock();
	unlock();
}

Event::~Event() {
	delete myImpl;
}

