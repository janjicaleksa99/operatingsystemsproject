/*
 * KerneSem.h
 *
 *  Created on: Aug 11, 2020
 *      Author: OS1
 */
#include "Semaphor.h"
#include "PCB.h"
#include <stdio.h>

#ifndef KERNESEM_H_
#define KERNESEM_H_

class KerneSem;

struct QNode {
	volatile PCB* pcb;
	QNode* next;
};


class Queue {
protected:
public:
	friend class KerneSem;
	QNode *first, *last;
	Queue() {
		first = last = 0;
	}

	~Queue() {
		QNode* old = 0, *cur = first;
		while (cur != 0) {
			old = cur;
			cur = cur->next;
			delete (QNode*)old;
		}
	}

	volatile PCB* get() {
		if (first) {
			QNode* temp = first;
			first = first->next;
			return temp->pcb;
		}
		else return 0;
	}

	void put(volatile PCB* pcb) {
		if (first == 0) {
			first = new QNode();
			first->pcb = pcb;
			first->next = 0;
			last = first;
		}
		else {
			QNode* temp = new QNode();
			temp->pcb = pcb;
			temp->next = 0;
			last->next = temp;
			last = temp;
		}
	}
};

struct SemNode {
	KerneSem* sem;
	SemNode* next;
};

class KerneSem {
public:
	KerneSem(Semaphore* mySem);
	virtual ~KerneSem();
	static void TakeOutTimeUnblockedThread(PCB* pcb);
	static void TimeSignal();
	int val() const;
	void setval(int v);
	void block(int);
	void unblock();
	static void DeleteAllSem();
	int& operator--();
	int operator++(int);
private:
	int value;
	Semaphore* mySem;
	Queue blocked;
	static SemNode *first, *last;
};

#endif /* KERNESEM_H_ */
