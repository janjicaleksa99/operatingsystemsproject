/*
 * PCB.cpp
 *
 *  Created on: Aug 4, 2020
 *      Author: OS1
 */

#include "PCB.h"
#include "Schedule.h"
#include "KerneSem.h"
#include <iostream.h>

class PCB;
extern volatile PCB* running;
extern void lock();
extern void unlock();

void Signal0() {
	running->zavrsio = 1;
#ifndef BCC_BLOCK_IGNORE
	running->unblock();
#endif
	dispatch();
}


PCB::PCB(Thread *thread) {
	myThread = thread;
	zavrsio = 0;
	blokiran = 0;
	TimeBlocked = 0;
	CurrentWaitTime = 0;
	value = 0;
	PID = sledeciPID++;
	LockFlag = 1;
	Locknum = 0;
	// Signali odavde do kraja konstruktora
	firstR = lastR = 0;
	/*for (int i = 0; i < 16; i++) {
		SignalList[i].first = 0;
		SignalList[i].BlockedSignal = 0;
	}

	if (running == 0) { //  mainThread nit ulazi ovde
		SignalList[0].first = new SigHandNode();
		SignalList[0].first->hand = Signal0;
		SignalList[0].first->next = 0;
	}

	if (running != 0) { // Ostale niti ulaze ovde
		PCBCreator = running;
		for (int i = 0; i < 16; i++) {
			SignalList[i].BlockedSignal = running->SignalList[i].BlockedSignal;
			/*SigHandNode* cur = running->SignalList[i].first;
			while (cur != 0) {
				SigHandNode* newN = new SigHandNode();
				if (SignalList[i].first == 0) {
					newN->hand = cur->hand;
					newN->next = 0;
					SignalList[i].first = newN;
					last[i] = newN;
				}
				else {
					newN->hand = cur->hand;
					newN->next = 0;
					last[i]->next = newN;
					last[i] = newN;
				}
			cur = cur->next;
			}
		}*/
		/*SignalRequest* curSR = running->firstR;
		while (curSR != 0) {
			SignalRequest* newN = new SignalRequest();
			if (firstR == 0) {
				newN->ID = curSR->ID;
				newN->next = 0;
				firstR = newN;
				lastR = newN;
			}
			else {
				newN->ID = curSR->ID;
				newN->next = 0;
				lastR->next = newN;
				lastR = newN;
			}
			curSR = curSR->next;
		}
	}*/
}

PCB::~PCB() {
	/*delete myThread;
	volatile Node* stari = 0, *tek = PCB::prvi;
	while (tek != 0) {
		stari = tek;
		tek = tek->next;
		delete (PCB*)stari;
	}*/
}

void PCB::wrapper2() volatile {
	myThread->run();
}

int PCB::imaNiti() {
	int ret = 0;
	volatile Node* tek = PCB::prvi;
	while (tek != 0) {
		if (tek->pcb->blokiran || tek->pcb->TimeBlocked) {
			ret = 1;
		}
		tek = tek->next;
	}
	return ret;
}

void PCB::block() {
	lock();
	++value;
	blocked.put(running);
	unlock();
}

void PCB::unblock() {
	lock();
	for (int i = 0; i < value; i++) {
		volatile PCB* pcb = blocked.get();
		pcb->blokiran = 0;
		Scheduler::put((PCB*)pcb);
	}
	unlock();
}

void PCB::DeleteAllPCB() {
	volatile Node* stari = 0, *tek = PCB::prvi;
	while (tek != 0) {
		stari = tek;
		tek = tek->next;
		delete (Node*)stari;
	}
}

void PCB::ProcessRequests() {
	volatile Node* curN = PCB::prvi;
	while (curN != 0) {
		SignalRequest* cur = curN->pcb->firstR, *prev = 0, *old;
		//printf("\n %p Pokazivac na request \n", cur);
		while (cur != 0) {
			if (curN->pcb->SignalList[cur->ID].BlockedSignal == 1 || PCB::BlockGlobSignals[cur->ID] == 1) {
				prev = cur;
				cur = cur->next;
				continue;
			}

			SigHandNode* curS = curN->pcb->SignalList[cur->ID].first;
			printf("\n %p curs \n", curS);
			while (curS != 0) {
				cout << "Poziv handlera" << endl;
				curS->hand();
				curS = curS->next;
			}
			old = cur;
			if (prev == 0) {
				cur = cur->next;
				curN->pcb->firstR = cur;
				delete old;
			}
			else {
				if (cur->next == 0)
					curN->pcb->lastR = prev;
				prev->next = cur->next;
				cur = cur->next;
				delete old;
			}

		}
		curN = curN->next;
	}
}

unsigned long PCB::BlockedThreadsNum() {
	unsigned long num = 0;
	volatile Node* tek = PCB::prvi;
			while (tek != 0) {
				if (tek->pcb->TimeBlocked || tek->pcb->blokiran) {
					++num;
				}
				tek = tek->next;
			}
			return num;
}

volatile Node* PCB::prvi = 0;
volatile Node* PCB::posl = 0;
unsigned long PCB::sledeciPID = 1;
int PCB::BlockGlobSignals[16] = {0};
