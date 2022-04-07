/*
 * KernelEv.cpp
 *
 *  Created on: Aug 13, 2020
 *      Author: OS1
 */

#include "KernelEv.h"
#include "Event.h"
#include "SCHEDULE.H"
#include <iostream.h>

extern volatile PCB* running;

KernelEv::KernelEv(Event* myEvent) {
	this->myEvent = myEvent;
	value = 1;
	ThreadCreator = running;
}

void KernelEv::signal() {
	myEvent->signal();
}

int KernelEv::val() const {
	return value;
}

void KernelEv::setval(int v) {
	value = v;
}

void KernelEv::block() {
	ThreadCreator->blokiran = 1;
	//printf(" Blokirana je nit %p %d ", ThreadCreator, ThreadCreator->PID);
	dispatch();
}

void KernelEv::unblock() {
	//volatile PCB* pcb = blocked.get();
	ThreadCreator->blokiran = 0;
	Scheduler::put((PCB*)ThreadCreator);
	//printf(" Odblokirana je nit %p %d ", ThreadCreator, ThreadCreator->PID);
}

int& KernelEv::operator--() {
	return --value;
}

int KernelEv::operator++(int i) {
	int ret = value;
	value++;
	return ret;
}


KernelEv::~KernelEv() {
	// TODO Auto-generated destructor stub
}

