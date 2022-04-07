/*
 * PCB.h
 *
 *  Created on: Aug 4, 2020
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_

#include "thread.h"

class PCB;

struct Node {
	volatile PCB* pcb;
	Node* next;
};

class BlockedList {
	Node* first, *last;
public:
	BlockedList() {
		first = last = 0;
	}
	volatile PCB* get() {
			if (first) {
				Node* temp = first;
				first = first->next;
				return temp->pcb;
			}
			else return 0;
		}

		void put(volatile PCB* pcb) {
			if (first == 0) {
				first = new Node();
				first->pcb = pcb;
				first->next = 0;
				last = first;
			}
			else {
				Node* temp = new Node();
				temp->pcb = pcb;
				temp->next = 0;
				last->next = temp;
				last = temp;
			}
		}
};

struct SigHandNode;

struct SignalRequest {
	SignalId ID;
	SignalRequest* next;
};

struct SigNode {
	int BlockedSignal;
	SigHandNode* first;
};

struct SigHandNode {
	SignalHandler hand;
	SigHandNode* next;
};

class PCB {
	int value;
public:
	friend class Thread;
	unsigned sp;
	unsigned ss;
	unsigned bp;
	unsigned zavrsio;
	int blokiran;
	int TimeBlocked;
	int MaxTimeToWait;
	int CurrentWaitTime;
	BlockedList blocked;
	static unsigned long sledeciPID;
	unsigned long PID;
	StackSize stackSize;
	Time timeSlice;
	volatile PCB* PCBCreator;
	static volatile Node *prvi, *posl;
	static int imaNiti();
	Thread* myThread;
	void (PCB::*thread_func)();
	SigNode SignalList[16];
	static int BlockGlobSignals[16];
	SigHandNode* last[16];
	SignalRequest *firstR, *lastR;
	int LockFlag;
	int Locknum;

	PCB(Thread* thread);
	PCB();
	void block();
	void unblock();
	void wrapper2() volatile;
	virtual ~PCB();
	static void DeleteAllPCB();
	static unsigned long BlockedThreadsNum();
	static void ProcessRequests();
};

#endif /* PCB_H_ */
