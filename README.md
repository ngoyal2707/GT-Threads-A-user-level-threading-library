# GT-Threads library #

The GT-Threads library provides the API for user level threading. It follows the API structure similar to pthread library on POSIX. It is intended for Unix users.


Author: Naman Goyal


**1) What Linux platform the library was tested on?**
	
The librray was tested on ubuntu 14.04 system with 64 bit architecture.


**2) How the preemptive scheduler is implemented.**

The preemptive scheduler is implemented by setting up the system timer in gthread_init() funciton. It generates a SIGVTALRM signal periodically, which is handled by a custom handler. The handler swaps the context to the scheduler and the scheduler then swaps the context to the thread next in the queue. It can be understod with following steps:

a) In gtthread_init() initalize queue, create context for scheduler and main thread. And start the timer after modifying the SIGVTALRM handler.

b) When you get control inside SIGVTALRM, swap context to shceduler context.

c) Inside scheduler, take the front element of steque out and add it to the end by calling steque_cycle(). And change the context to the new front.  

**3) How to compile the library and run test program.**

The probram and library work with the given makefile without change, I have tried it with the submission page on Udacity which uses the default makefile.

The exact steps are as following:

a) Extract the compressed folder in a folder.

b) Browse to the folder and build using following command:
	make

c) include the gthread.h header file in your program and link it to the object files created by above command as following:
	gcc test.c *.o



**4) How the deadlocks in your Dining Philosophers solution are prevented.**
	
The dining philosopher will result into deadlock if all the philosopher tries to pick the same side chopstick first (all left or all right). To prevent the deadlock we have to break the order by changing the order of chopstick pickup for anyone of the philosopher. I have done it by picking up the larger number of chopstick for each of the philosopher. Thus if we number the chopsticks in incremental order in clockwise fashion then the philosopher number 0,1,2,3 will choose left chostick (1,2,3,4 accordingly) first but the last philosopher 4 will choose right chopstick (also 4) first. This will break the order and prevent deadlock. 


