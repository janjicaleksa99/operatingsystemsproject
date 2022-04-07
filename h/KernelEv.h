/*
 * KernelEv.h
 *
 *  Created on: Aug 13, 2020
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_

#include "KerneSem.h"

class Event;
struct QNode;
class Queue;

class KernelEv {
	Event* myEvent;
	int value;
	Queue blocked;
	volatile PCB* ThreadCreator;
public:
	friend class Event;
	void signal();
	KernelEv(Event* myEvent);
	int val() const;
	void setval(int v);
	void block();
	void unblock();
	virtual ~KernelEv();
	int& operator--();
	int operator++(int);
};

#endif /* KERNELEV_H_ */
