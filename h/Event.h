// File: event.h
#ifndef _event_h_
#define _event_h_

#include "IVTEntry.h"
#include "KernelEv.h"
#include <stdio.h>

#define PREPAREENTRY(IvtNo, CallF) \
void interrupt Int##IvtNo() { \
		IVTEntry* ivt = IVTEntry::retObject(IvtNo);\
		ivt->getKEvent()->signal();\
		if (CallF)\
			ivt->retOldRoutine();\
		dispatch();\
	}\
	IVTEntry* ivt##IvtNo = new IVTEntry(IvtNo, 0, (pInterrupt) Int##IvtNo)


typedef unsigned char IVTNo;

class KernelEv;

class Event {
public:
 Event (IVTNo ivtNo);
 ~Event ();
 void wait ();
protected:
 friend class KernelEv;
 void signal(); // can call KernelEv
private:
 KernelEv* myImpl;
};
#endif
