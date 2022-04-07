
#include "Test1.h"
#include "SCHEDULE.H"
#include "Semaphor.h"
#include <iostream.h>

extern volatile int lockFlag;
extern Semaphore *sem;
Semaphore *sem1 = new Semaphore();
extern volatile int brojac1;

Test1::Test1(StackSize stackSize, Time timeSlice): Thread(stackSize, timeSlice) {
	// TODO Auto-generated constructor stub

}

Test1::Test1() : Thread() {

}

Test1::~Test1() {
	// TODO Auto-generated destructor stub
}

void Test1::run()  {
	for (int i =0; i < 5; ++i) {
		cout<<"u b() ="<< i <<endl;

		for (int k = 0; k<10000; ++k)
			for (int j = 0; j <30000; ++j);
	}
}
