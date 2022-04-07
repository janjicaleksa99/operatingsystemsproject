/*
 * KerneSem.cpp
 *
 *  Created on: Aug 11, 2020
 *      Author: OS1
 */

#include "KerneSem.h"
#include "schedule.h"
#include "PCB.h"

extern volatile PCB* running;
extern volatile PCB* mainThread;

struct SemNode;

KerneSem::KerneSem(Semaphore* mySem) {
	this->mySem = mySem;
	if (first == 0) {
		first = new SemNode();
		first->sem = this;
		first->next = 0;
		last = first;
	}
	else {
		SemNode* temp = new SemNode();
		last->next = temp;
		last = temp;
		last->sem = this;
		last->next = 0;
	}
}

KerneSem::~KerneSem() {
	// TODO Auto-generated destructor stub
}

void KerneSem::DeleteAllSem() {
	SemNode* old = 0, *cur = KerneSem::first;
	while (cur != 0) {
		old = cur;
		cur = cur->next;
		delete (SemNode*)old;
	}
}

int KerneSem::val() const {
	return value;
}

void KerneSem::setval(int v) {
	value = v;
}

void KerneSem::block(int TimeFlag) {
	if (TimeFlag == 1)
		running->TimeBlocked = 1;
	else if (TimeFlag == 0)
		running->blokiran = 1;
	//printf("Blokirana nit je %p %d \n", running, running->PID);
	blocked.put(running);
	dispatch();
}

void KerneSem::unblock() {
	volatile PCB* pcb = blocked.get();
	if (pcb) {
		pcb->blokiran = 0;
		pcb->TimeBlocked = 0;
		pcb->CurrentWaitTime = 0;
		Scheduler::put((PCB*)pcb);
	}
}

/*void KerneSem::TakeOutTimeUnblockedThread(PCB* pcb) {
	SemNode* cur = first;
	int i = 0;
	while (cur != 0) {
		KerneSem* sem = cur->sem;
		(*sem)++;
		int ret = sem->blocked.TakeElemOut(pcb);
		if (ret)
			break;
		cur = cur->next;
	}
}*/

void KerneSem::TimeSignal() {
	SemNode* cur = first;
	while (cur != 0) {
		KerneSem* sem = cur->sem;
		Queue* blocked = &sem->blocked;
		QNode* curB = blocked->first, *prevB = 0, *old;
		while (curB != 0) {
			int flag = 0;
			volatile PCB* pcb = curB->pcb;
			if (pcb->TimeBlocked) {
				pcb->CurrentWaitTime++;
				if (pcb->CurrentWaitTime == pcb->MaxTimeToWait) {
					(*sem)++;
					pcb->TimeBlocked = 0;
					pcb->CurrentWaitTime = 0;
					pcb->MaxTimeToWait = -1;
					//printf("Nit koja bi trebalo da se odblokira %p %d \n", pcb, pcb->PID);
					if (pcb != mainThread) {
						Scheduler::put((PCB*)pcb);
						//printf("Nit koja je zaista odblokirana %p %d \n", pcb, pcb->PID);
					}
					if (prevB == 0) {
						old = curB;
						blocked->first = curB->next;
						curB = blocked->first;
						delete old;
						flag = 1;
					}
					else {
						old = curB;
						prevB->next = curB->next;
						curB = prevB->next;
						delete old;
						flag = 1;
					}
				}
			}
			if (flag == 0) {
				prevB = curB;
				curB = curB->next;
			}
			else flag = 0;
		}
		cur = cur->next;
	}
}

int& KerneSem::operator--() {
	value = value - 1;
	return value;
}

int KerneSem::operator++(int i) {
	int ret = value;
	value++;
	return ret;
}

SemNode* KerneSem::first = 0;
SemNode* KerneSem::last = 0;

