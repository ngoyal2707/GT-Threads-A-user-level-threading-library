/**********************************************************************
gtthread_sched.c.  

This file contains the implementation of the scheduling subset of the
gtthreads library.  A simple round-robin queue should be used.
 **********************************************************************/
/*
  Include as needed
*/

#include "gtthread.h"
/* Just an example. Nothing to see here. */
#include "gtthread.h"
#include <ucontext.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include "steque.h"
#define STACKCAPACITY 32768

#define MAX_THREAD_COUNT 50000

#define GTTHREAD_RUNNING 0
#define GTTHREAD_CANCELED 1
#define GTTHREAD_DONE 2

/* 
   Students should define global variables and helper functions as
   they see fit.
 */

static ucontext_t sched_context;
context_node *current_context;

int threadMap[MAX_THREAD_COUNT];
steque_t *queue;

static void stub_function(void *(*start_routine)(void *), void * arg);
int liveThreadCount = 0;
gtthread_t mainThreadID;
gtthread_t currentThreadIDPointer;

gtthread_t createNewThreadID(){
	gtthread_t i=currentThreadIDPointer;
	while(threadMap[i] != 0){
		i = (i+1)%MAX_THREAD_COUNT;
		if(i==currentThreadIDPointer){
			return -1;
		}
	}
	currentThreadIDPointer=(i+1)%MAX_THREAD_COUNT;
	return i;
}
void timer_handler(){
#ifdef DEBUG
	printf("In Timer\n");
#endif

	swapcontext(&current_context->context, &sched_context);
}

void blockTimer(){
	sigset_t vtalrm;
	sigemptyset(&vtalrm);
  	sigaddset(&vtalrm, SIGVTALRM);
  	sigprocmask(SIG_BLOCK, &vtalrm, NULL);
#ifdef DEBUG
  	printf("Blocked Timer\n");
#endif
}

void unblockTimer(){
	sigset_t vtalrm;
  	sigemptyset(&vtalrm);
  	sigaddset(&vtalrm, SIGVTALRM);
  	sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
#ifdef DEBUG
  	printf("Unblocked Timer\n");
#endif
}

/*
Round robin scheduler 
*/
static void roundRobinScheduler(){
	while(liveThreadCount){
		blockTimer();
		context_node *nextContext;
		steque_cycle(queue);
		nextContext = (context_node *) steque_front(queue);
		current_context = nextContext;

		unblockTimer();
		if(current_context->completed != 1 && current_context->exited != 1 && current_context->canceled != 1){
			if(swapcontext(&sched_context , &(current_context->context))==-1){
				printf("Error in swapcontext\n");
			}
		}
	}
}

/* 
 Setting up the alarm
*/
void initializeTimer(long preemption_period){
	struct itimerval *T;
	struct sigaction handler;

	T = (struct itimerval*) malloc(sizeof(struct itimerval));
	T->it_value.tv_sec = T->it_interval.tv_sec = 0;
	T->it_value.tv_usec = T->it_interval.tv_usec = preemption_period;

	setitimer(ITIMER_VIRTUAL, T, NULL);

	memset(&handler,0,sizeof(handler));
	handler.sa_sigaction = &timer_handler;

	if(sigaction(SIGVTALRM , &handler, NULL)==-1){
		printf("Error in setting timer handler\n");
	}
#ifdef DEBUG
	printf("Timer set successfully\n");
#endif
}


/*
  The gtthread_init() function does not have a corresponding pthread equivalent.
  It must be called from the main thread before any other GTThreads
  functions are called. It allows the caller to specify the scheduling
  period (quantum in micro second), and may also perform any other
  necessary initialization.

  Recall that the initial thread of the program (i.e. the one running
  main() ) is a thread like any other. It should have a
  gtthread_t that clients can retrieve by calling gtthread_self()
  from the initial thread, and they should be able to specify it as an
  argument to other GTThreads functions. The only difference in the
  initial thread is how it behaves when it executes a return
  instruction. You can find details on this difference in the man page
  for pthread_create.
 */
void gtthread_init(long period){

// Initializing all the data structures
    long preemption_period = period;
  	memset(threadMap,0,sizeof(threadMap));
  	currentThreadIDPointer=0;
  	
  	queue = (steque_t *) malloc(1*sizeof(steque_t));
  	steque_init(queue);

  
  	liveThreadCount=1;
// Initialize scheduler context
	if( !getcontext(&sched_context) ){
		sched_context.uc_stack.ss_sp = (char*) malloc(STACKCAPACITY);;
		sched_context.uc_stack.ss_size = STACKCAPACITY;
		sched_context.uc_stack.ss_flags = 0;
		sched_context.uc_link = NULL;

		makecontext( &sched_context, (void(*)(void)) roundRobinScheduler, 0 ); 
	}

// Initializing main_thread ID
  	mainThreadID = createNewThreadID(); 

	context_node *mainContextNode = (context_node *) malloc(1* sizeof(context_node));
  	mainContextNode->thread_id = mainThreadID;
  	mainContextNode->running=0;
  	mainContextNode->completed=0;
  	mainContextNode->exited=0;
  	mainContextNode->canceled=0;

	if(!getcontext(&mainContextNode->context)){
		mainContextNode->context.uc_stack.ss_sp = (char *)malloc(STACKCAPACITY);
		mainContextNode->context.uc_stack.ss_size = STACKCAPACITY;
		mainContextNode->context.uc_stack.ss_flags = 0;
	    current_context = mainContextNode;
	}

    steque_enqueue(queue, (steque_item) mainContextNode);

// Initialize signal set  and unblock sigvtalrm signal as start

  	unblockTimer();
 	if(preemption_period)
 	 	initializeTimer(preemption_period);
}


/*
  The gtthread_create() function mirrors the pthread_create() function,
  only default attributes are always assumed.
 */
int gtthread_create(gtthread_t *thread,
		    void *(*start_routine)(void *),
		    void *arg){

	// blockTimer();

	context_node * new_thread_context = (context_node *) malloc(1* sizeof(context_node));
	new_thread_context->thread_id = createNewThreadID();
	
	if(new_thread_context->thread_id==-1){
		printf("No more free threads\n");
		return -1;
	}
	*thread =  new_thread_context->thread_id;
    new_thread_context->running=0;
  	new_thread_context->completed=0;
  	new_thread_context->exited=0;
  	new_thread_context->canceled=0;

  	// unblockTimer();
	if(getcontext(&(new_thread_context->context))==-1){
		perror("getcontext");
		return -1;
	}	
	blockTimer();

	new_thread_context->context.uc_link = &sched_context;
	new_thread_context->context.uc_stack.ss_sp = (char*) malloc(STACKCAPACITY);
	new_thread_context->context.uc_stack.ss_size = STACKCAPACITY;
	new_thread_context->context.uc_stack.ss_flags = 0;
	
	steque_enqueue(queue, (steque_item)new_thread_context);
	liveThreadCount++;

	unblockTimer();
	makecontext(&new_thread_context->context,(void(*)(void)) stub_function,2, start_routine, arg);

#ifdef DEBUG
	printf("created successfully\n");
#endif
	return 0;
}	


static void stub_function(void * (*start_routine)(void *), void * arg){
	current_context->running = 1;
	current_context->retval = (void *)start_routine(arg);

	blockTimer();
	if(current_context->exited!=1){
		current_context->completed=1;
		current_context->running=0;
		gtthread_cancel(current_context->thread_id);
	}
	unblockTimer();
	setcontext(&sched_context);

	return;
}

/*
  The gtthread_join() function is analogous to pthread_join.
  All gtthreads are joinable.
 */
int gtthread_join(gtthread_t thread, void **status){

	blockTimer();
	steque_node_t *curr=queue->front;

	while(curr != NULL && ((context_node *)curr->item)->thread_id != thread){
		curr=curr->next;
	}
	if(curr==NULL){
		printf("Didn't find the thread id %lu \n", thread);
		unblockTimer();
		return -1;
	}
	context_node * target_thread_context = (context_node *)curr->item;
	
	while(target_thread_context->completed != 1 && target_thread_context->exited != 1 && target_thread_context->canceled != 1){
		unblockTimer();
		gtthread_yield();
		blockTimer();
	}
	if((status!=NULL) && (target_thread_context->completed==0 && target_thread_context->exited==0 && target_thread_context->canceled==1)){
		*status = (void *) GTTHREAD_CANCELED;
	}
	else if((status!=NULL)&&(target_thread_context->completed==1 || target_thread_context->exited==1)){
		*status = (void *)target_thread_context->retval;
		unblockTimer();
		return 0;
	}
	unblockTimer();
	return -1;
}


/*
  The gtthread_exit() function is analogous to pthread_exit.
 */
void gtthread_exit(void* retval){
	blockTimer();
	current_context->retval = retval;
	current_context->exited = 1;
	current_context->running = 0;
	gtthread_cancel(current_context->thread_id);
	unblockTimer();
	setcontext(&sched_context);
	return;
}	


/*
  The gtthread_yield() function is analogous to pthread_yield, causing
  the calling thread to relinquish the cpu and place itself at the
  back of the schedule queue.
 */
int gtthread_yield(void){
	if(swapcontext(&current_context->context,&sched_context)==-1){
		printf("Error in gtthread_yield while swapping context\n");
		return -1;
	}
	return 0;
}

/*
  The gtthread_yield() function is analogous to pthread_equal,
  returning zero if the threads are the same and non-zero otherwise.
 */
int  gtthread_equal(gtthread_t t1, gtthread_t t2){
	if(t1==t2)
		return 1;
	return 0;
}

/*
  The gtthread_cancel() function is analogous to pthread_cancel,
  allowing one thread to terminate another asynchronously.
 */
int  gtthread_cancel(gtthread_t thread){
	blockTimer();
	steque_node_t *curr = queue->front;

	while(curr != NULL && ((context_node *)curr->item)->thread_id != thread){
		curr=curr->next;
	}
	if(curr==NULL){
		printf("Didn't find the thread id %lu \n", thread);
		unblockTimer();
		return -1;
	}
	context_node * target_thread_context = (context_node *)curr->item;
	if(target_thread_context->canceled != 1){
		free(target_thread_context->context.uc_stack.ss_sp);
		target_thread_context->running = 0;
		target_thread_context->canceled = 1;
		liveThreadCount--;
		threadMap[current_context->thread_id]=0;
		unblockTimer();
		return 0;
	}
	unblockTimer();
	return -1;
}

/*
  Returns calling thread.
 */
gtthread_t gtthread_self(void){
	return current_context->thread_id;
}
