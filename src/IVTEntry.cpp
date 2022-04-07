/*
 * IVTEntry.cpp
 *
 *  Created on: Aug 12, 2020
 *      Author: OS1
 */

#include "IVTEntry.h"
#include "KernelEv.h"
#include <dos.h>



IVTEntry::IVTEntry(unsigned char IVTentry, KernelEv* KEvent, pInterrupt newISR) {
	this->IVTentry = IVTentry;
	myKEvent = KEvent;
	IVTEntry::InterruptVector[IVTentry] = this;
	this->newISR = newISR;
}

void IVTEntry::inic(pInterrupt intr) {
	asm cli;
#ifndef BCC_BLOCK_IGNORE
	oldISR = getvect(IVTentry);
	setvect(IVTentry, intr);
#endif
	asm sti;
}

void IVTEntry::restore() {
	asm cli;
	#ifndef BCC_BLOCK_IGNORE
		setvect(IVTentry, oldISR);
	#endif
	asm sti;
}

void IVTEntry::setKEvent(KernelEv* KEvent) {
	myKEvent = KEvent;
}

IVTEntry::~IVTEntry() {
	restore();
}

unsigned char IVTEntry::FreeEntry = 0x20;
IVTEntry* IVTEntry::InterruptVector[256];
