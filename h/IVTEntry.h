/*
 * IVTEntry.h
 *
 *  Created on: Aug 12, 2020
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_

class KernelEv;

typedef void interrupt (*pInterrupt) (...);

class IVTEntry {
	unsigned char IVTentry;
	static unsigned  char FreeEntry;
	pInterrupt newISR;
	pInterrupt oldISR;
	static IVTEntry* InterruptVector[256];
	KernelEv* myKEvent;
public:
	IVTEntry(unsigned char IVTentry, KernelEv* KEvent, pInterrupt newISR);
	void inic(pInterrupt intr);
	void setKEvent(KernelEv* KEvent);
	KernelEv* getKEvent() {
		return myKEvent;
	}
	static IVTEntry* retObject(unsigned char i) {
		return InterruptVector[i];
	}
	pInterrupt retOldRoutine() {
		return oldISR;
	}
	pInterrupt retRoutine() {
		return newISR;
	}
	void restore();
	virtual ~IVTEntry();
};

#endif /* IVTENTRY_H_ */
