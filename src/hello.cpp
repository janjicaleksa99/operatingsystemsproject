/*
 * hello.cpp
 *
 *  Created on: Apr 1, 2015
 *      Author: OS1
 */

#include <iostream.h>
#include <stdio.h>
#include "Test.h"
#include "thread.h"
#include "SCHEDULE.H"
#include "KerneSem.h"
#include "PCB.h"
#include "Idle.h"

extern volatile PCB* running;
volatile PCB* mainThread;
volatile Thread* idleThread;
extern volatile int lockFlag;
extern void inic();
extern void restore();
class PCB;
extern int userMain(int argc, char *argv[]);

extern int syncPrintf(const char *format, ...);


/*extern int userMain(int argc, char *argv[])
{
	cout << "userMain\n";
	Thread* thr = new Test();
	thr->start();
	Thread* thr1 = new Test1();
	thr1->start();

	for (int i = 0; i < 5; ++i) {
	  	lockFlag = 0;
	  	cout<<"main "<<i<<endl;
	  	lockFlag = 1;

		for (int j = 0; j< 30000; ++j)
			for (int k = 0; k < 30000; ++k);
	}

	thr->waitToComplete();
	thr1->waitToComplete();
	return 0;
}*/

int main(int argc, char *argv[])
{
	int ret;
	cout << "main\n";
	mainThread = new PCB(0);
	mainThread->timeSlice = 0;
	printf("%p %d \n", mainThread, mainThread->PID);

	idleThread = new Idle();

	running = mainThread;
	inic();

	ret = userMain(argc, argv);
	cout << "Kraj maina\n";
	PCB::DeleteAllPCB();
	KerneSem::DeleteAllSem();
	restore();
	return ret;
}
