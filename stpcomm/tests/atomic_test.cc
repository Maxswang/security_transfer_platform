
#include "stpcomm//atomic.h"
#include "stpcomm//thread.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void AtomicDemo()
{
	AtomicIntegerL a(10);
	long value = ++a;
	printf("value = %ld, atomic a = %ld\n", value, a.value());
	assert(value == 11); assert(a.value() == 11);
	value = --a;
	printf("value = %ld, atomic a = %ld\n", value, a.value());
	assert(value == 10); assert(a.value() == 10);
	value = a--;
	printf("value = %ld, atomic a = %ld\n", value, a.value());
	assert(value == 10); assert(a.value() == 9);
	value = a++;
	printf("value = %ld, atomic a = %ld\n", value, a.value());
	assert(value == 9); assert(a.value() == 10);
}

AtomicIntegerL g_counter(0);
class MyThread2 : public Thread
{
public:
	virtual void Run()
	{
		for (int i = 0; i < 30; ++i)
		{
			long value = ++g_counter;
			// printf("counter %d\n", value);
			fprintf(stderr, "counter %ld\n", value);
			// Sleep(100);
		}
	}
};

void AtomicDemo2()
{
	MyThread2 a, b;
	a.Start();
	b.Start();
	a.Stop();
	b.Stop();
}

void AtomicDemo3()
{
	AtomicIntegerUL a(0);
	unsigned long value = --a;
	printf("value = %lu, atomic a = %lu\n", value, a.value());
    printf("value = %ld, atomic a = %ld\n", value, a.value());
}

int main()
{
	printf("main start\n");
//	 AtomicDemo();
//	 AtomicDemo2();
	AtomicDemo3();



	printf("main over\n");
	return 0;
}
