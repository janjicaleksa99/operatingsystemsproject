#ifndef TIMER_CPP
#define TIMER_CPP

#include "thread.h"
#include "schedule.h"
#include "KerneSem.h"
#include "Idle.h"
#include <iostream.h>
#include <stdio.h>

unsigned oldTimerOFF, oldTimerSEG; // stara prekidna rutina
int i = 1;

extern volatile PCB* mainThread;
extern volatile Thread* idleThread;
extern volatile int brojac;
volatile unsigned ttsp, ttss, ttbp;
extern volatile int lockFlag;
extern volatile PCB* running;
extern volatile int Tajmer;
extern volatile int flagT;
extern void tick();
volatile int locknum = 0;

void lock() {
	++running->Locknum;
	running->LockFlag = 0;
}

void unlock() {
	if (--running->Locknum == 0)
		running->LockFlag = 1;
	if (running->Locknum < 0)
		running->Locknum = 0;
}

void interrupt timer() {	// prekidna rutina

	KerneSem::TimeSignal();

	//PCB::ProcessRequests();

	if (running->timeSlice != 0) {
		if (--brojac == 0 || running == mainThread) {
			if (running->LockFlag) {
				asm {
					// cuva sp
					mov ttsp, sp
					mov ttss, ss
					mov ttbp, bp
				}
				//printf("\nTimer promena konteskta \n");
				running->sp = ttsp;
				running->ss = ttss;
				running->bp = ttbp;

				if (running != mainThread && running != ((Idle*)idleThread)->retPCB()) {
					if (!running->zavrsio && !running->blokiran && !running->TimeBlocked) {
						Scheduler::put((PCB*)running);
						//printf("Stavljanje niti u Scheduleru! \n %p %d \n", running, running->PID);
					}
				}
				running = Scheduler::get();
				//printf("%p %d \n", running, running->PID);

				if (running == 0 && PCB::imaNiti() == 0) {
					//printf("Povratak na main \n");
					running = mainThread;
				}
				else if (running == 0 && PCB::imaNiti() == 1) {
					/*Thread* idle_thread = new Idle();
					idle_thread->start();
					idleThread = idle_thread;
					//printf(" Idle ");
					dispatch();*/
					running = ((Idle*)idleThread)->retPCB();
					//printf(" Idle ");
				}

				ttsp = running->sp;
				ttss = running->ss;
				ttbp = running->bp;

				brojac = running->timeSlice;

				asm {
					mov sp, ttsp   // restore sp
					mov ss, ttss
					mov bp, ttbp
				}
				//printf("Kraj Timera \n");
				//zahtevana_promena_konteksta = 0;
			}
			//else zahtevana_promena_konteksta = 1;
		}
	}
	// poziv stare prekidne rutine koja se
     // nalazila na 08h, a sad je na 60h
     // poziva se samo kada nije zahtevana promena
     // konteksta – tako se da se stara
     // rutina poziva samo kada je stvarno doslo do prekida
	tick();
	asm int 60h;
	i++;
}

// postavlja novu prekidnu rutinu
void inic(){
	asm{
		cli
		push es
		push ax

		mov ax,0   //  ; inicijalizuje rutinu za tajmer
		mov es,ax

		mov ax, word ptr es:0022h //; pamti staru rutinu
		mov word ptr oldTimerSEG, ax // sa ulaza 08h
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer	 //postavlja
		mov word ptr es:0020h, offset timer //novu rutinu na ulaz 08h

		mov ax, oldTimerSEG	 //	postavlja staru rutinu
		mov word ptr es:0182h, ax //; na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
}

// vraca staru prekidnu rutinu
void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax


		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
}
#endif
