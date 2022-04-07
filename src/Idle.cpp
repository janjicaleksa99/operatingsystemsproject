/*
 * Idle.cpp
 *
 *  Created on: Aug 7, 2020
 *      Author: OS1
 */

#include "Idle.h"
#include "SCHEDULE.H"
#include "PCB.h"
#include "thread.h"
#include <iostream.h>
#include <stdio.h>


Idle::Idle(StackSize stackSize, Time timeSlice): Thread(stackSize, timeSlice) {
	// TODO Auto-generated constructor stub

}

Idle::Idle() : Thread() {

}

Idle::~Idle() {
	// TODO Auto-generated destructor stub
}

extern void lock();
extern void unlock();

void put(PCB* pcb) {
	lock();
	Scheduler::put((PCB*)pcb);
	unlock();
}

int get() {
	lock();
	int ret;
	PCB* temp;
	temp = Scheduler::get();
	if (temp == 0)
		ret =  0;
	else {
		Scheduler::put((PCB*)temp);
		ret = 1;
	}
	unlock();
	return ret;
}

/*void Idle::run()  {
	static int br = 0;
	unsigned long num = PCB::BlockedThreadsNum();
	while (num > PCB::BlockedThreadsNum()) {

		num = PCB::BlockedThreadsNum();
	}
	//put((PCB*)temp);
	//cout << "Kraj Idle \n";
}*/

void Idle::run()  {
	volatile int i = 1;
	PCB* temp;
	while (1) {
		if (get() == 1) {
			dispatch();
		}
	}
	//put((PCB*)temp);
	//cout << "Kraj Idle \n";
}
