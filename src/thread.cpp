/*
 * thread.cpp*/

#include "thread.h"
#include "SCHEDULE.h"
#include "PCB.h"
#include "Idle.h"
#include <iostream.h>
#include <stdio.h>
#include <dos.h>

extern volatile PCB* mainThread;
extern volatile Thread* idleThread;
volatile PCB* running = 0;
volatile int lockFlag = 1;
extern void lock();
extern void unlock();
typedef void (*thread_method)();
class PCB;


void wrapper1() {
	if (running) {
#ifndef BCC_BLOCK_IGNORE
		running->wrapper2();
#endif
		running->zavrsio = 1;
		//running->PCBCreator->myThread->signal(1);
		//running->myThread->signal(2);
#ifndef BCC_BLOCK_IGNORE
		running->unblock();
#endif
		dispatch();
	}
}

Thread::Thread(StackSize stackSize, Time timeSlice) {
	lock(); // Zakljucavanje kriticne sekcije
	myPCB = new PCB(this);
	//printf("%p %d \n", myPCB, myPCB->PID);
	if (!PCB::prvi) {
		PCB::prvi = new Node();
		PCB::prvi->pcb = myPCB;
		PCB::prvi->next = 0;
		PCB::posl = PCB::prvi;
	}
	else {
		Node* temp = new Node();
		temp->next = 0;
		PCB::posl->next = temp;
		PCB::posl = temp;
		PCB::posl->pcb = myPCB;
	}
	unlock(); // Otkljucavanje kriticne sekcije
	myPCB->stackSize = stackSize;
	if (timeSlice)
		myPCB->timeSlice = timeSlice;
	else myPCB->timeSlice = 0;

	unsigned *st = new unsigned[stackSize / 2];

#ifndef BCC_BLOCK_IGNORE

	st[stackSize / 2 - 1] = 0x200; //PSW

	st[stackSize / 2 - 2] = FP_SEG(wrapper1); //PC
	st[stackSize / 2 - 3] = FP_OFF(wrapper1); //CS I IP

	myPCB->sp = FP_OFF(st + stackSize / 2 - 12);
	myPCB->ss = FP_SEG(st + stackSize / 2 - 12);
	myPCB->bp = FP_OFF(st + stackSize / 2 - 12);
#endif
}

void Thread::registerHandler(SignalId signal, SignalHandler handler) {
	SigHandNode* first = myPCB->SignalList[signal].first;
	lock();
	if (myPCB->SignalList[signal].first == 0) {
		myPCB->SignalList[signal].first = new SigHandNode();
		myPCB->SignalList[signal].first->hand = handler;
		myPCB->SignalList[signal].first->next = 0;
		myPCB->last[signal] = myPCB->SignalList[signal].first;
	}
	else {
		SigHandNode* temp = new SigHandNode();
		myPCB->last[signal]->next = temp;
		myPCB->last[signal] = temp;
		myPCB->last[signal]->hand = handler;
		myPCB->last[signal] = 0;
	}
	unlock();
}

void Thread::unregisterAllHandlers(SignalId id) {
	SigHandNode* first = myPCB->SignalList[id].first;
	SigHandNode* cur = first, *old;
	while (cur != 0) {
		old = cur;
		cur = cur->next;
		delete old;
	}
	myPCB->SignalList[id].first = 0;
}

void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	SigHandNode* first = myPCB->SignalList[id].first;
	SignalHandler ptr1 = 0, ptr2 = 0;
	SigHandNode* cur = first, *node1, *node2;
	while (cur != 0) {
		if (cur->hand == hand1) {
			ptr1 = hand1;
			node1 = cur;
		}
		if (cur->hand == hand2) {
			ptr2 = hand2;
			node2 = cur;
		}
		cur = cur->next;
	}
	if (ptr1 == 0 || ptr2 == 0)
		return;
	node1->hand = ptr2;
	node2->hand = ptr1;
}

void Thread::blockSignal(SignalId signal) {
	myPCB->SignalList[signal].BlockedSignal = 1;
}

void Thread::blockSignalGlobally(SignalId signal) {
	PCB::BlockGlobSignals[signal] = 1;
}

void Thread::unblockSignal(SignalId signal) {
	myPCB->SignalList[signal].BlockedSignal = 0;
}

void Thread::unblockSignalGlobally(SignalId signal) {
	PCB::BlockGlobSignals[signal] = 0;
}

void Thread::signal(SignalId signal) {
	if (signal < 0 || signal > 15)
		return;
	lock();
	if (myPCB->firstR == 0) {
		myPCB->firstR = new SignalRequest();
		printf("Dodato u requestove %p \n", myPCB->firstR);
		myPCB->firstR->ID = signal;
		myPCB->firstR->next = 0;
		myPCB->lastR = myPCB->firstR;
	}
	else {
		SignalRequest* temp = new SignalRequest();
		printf("Dodato u requestove %p \n", temp);
		myPCB->lastR->next = temp;
		myPCB->lastR = temp;
		myPCB->lastR->ID = signal;
		myPCB->lastR->next = 0;
	}
	unlock();
}

Thread::~Thread() {
	waitToComplete();
	lock();
	volatile Node* tek = PCB::prvi, *preth = 0, *stari;
	while (tek != 0) {
		if (preth == 0) {
			if (tek->pcb == myPCB) {
				stari = tek;
				PCB::prvi = tek->next;
				tek->next = 0;
				delete ((Node*) stari); // DODATO
				break;
			}
			preth = tek;
		}
		else {
			if (tek->pcb == myPCB) {
				stari = tek;
				preth->next = tek->next;
				tek->next = 0;
				delete ((Node*) stari); // DODATO
				break;
			}
			preth = tek;
		}
		tek = tek->next;
	}
	tek = PCB::prvi;
	if (tek == 0)
		PCB::posl = 0;
	else
	while (tek != 0) {
		if (tek->next == 0)
			PCB::posl = tek;
		tek = tek->next;
	}
	delete myPCB;
	unlock();
}

void Thread::start() {
	Scheduler::put((PCB*)myPCB);
	//cout << "Korisnicka nit je ubacena u red spremnih\n";
}

volatile int brojac;
volatile unsigned tsp, tss, tbp;

void interrupt _dispatch() {
	//cout << "Dispatch\n";
	asm {
			// cuva sp
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}

		running->sp = tsp;
		running->ss = tss;
		running->bp = tbp;

		if (running != mainThread && running != ((Idle*)idleThread)->retPCB())
			if (!running->zavrsio && !running->blokiran && !running->TimeBlocked) {
				Scheduler::put((PCB*)running);
				//printf("Stavljanje niti u Scheduleru! \n %p %d \n", running, running->PID);
			}
		running = Scheduler::get();
		//printf("%p %d \n", running, running->PID);

		if (running == 0 && PCB::imaNiti() == 0) {
			//printf("Povratak na main!\n");
			running = mainThread;
		}
		else if (running == 0 && PCB::imaNiti() == 1) {
			/*Thread* idle_thread = new Idle();
			idle_thread->start();
			idleThread = idle_thread;
			//printf(" Idle ");
			dispatch();*/
			running = ((Idle*)idleThread)->retPCB();
		}

		tsp = running->sp;
		tss = running->ss;
		tbp = running->bp;

		brojac = running->timeSlice;

		asm {
			mov sp, tsp   // restore sp
			mov ss, tss
			mov bp, tbp
		}
}

void dispatch () {
	asm cli;
	_dispatch();
	asm sti;
}

ID Thread::getId() {
	return myPCB->PID;
}

ID Thread::getRunningId() {
	if (running != 0) {
		return running->PID;
	}
	else return -1;
}

Thread* Thread::getThreadById(ID id) {
	volatile Node* tek = PCB::prvi;
	while (tek != 0) {
		if (tek->pcb->PID == id)
			return tek->pcb->myThread;
		tek = tek->next;
	}
	return 0;
}

void Thread::waitToComplete() {
	while (myPCB->zavrsio == 0) {
		running->blokiran = 1;
		myPCB->block();
		dispatch();
	}
}
