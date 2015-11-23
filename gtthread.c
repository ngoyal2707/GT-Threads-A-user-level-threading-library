#include "gtthread.h"
#include "gtthread_helper.h"
#include <ucontext.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>

gtthread_t mainThreadID;
ucontext_t mainThreadContext;
long preemption_period;
static sigset_t vtalrm;

void timer_handler(){
	context_node *currentContext, *previousContext;
    queue_size();
  currentContext = queue_top();   
  queue_pop();
  printf("In timer\n");
  
  // printf("Queue address : %p\n", context_queue);
  if(!queue_empty()){
    printf("Swapping\n");
    previousContext = queue_top();
    // printf("Got previous context\n");
    queue_push(currentContext);
    // printf("Pushed to the back of queue\n");
    if(currentContext==NULL){
      printf("current contest is NULL\n");
    }
    if(previousContext==NULL){
      printf("previous context is NULL\n");
    }
    queue_print();
    queue_size();
    // printf("Found queue size\n");
    // printf("%lu\n", &(currentContext->context));
    // printf("%lu\n", &(previousContext->context));
    swapcontext(&(currentContext->context) , &(previousContext->context));
    // printf("Swapped\n");
  }else{
    queue_push(currentContext);
  }
}

void initializeTimer(){
    /* 
     Setting up the alarm
  */
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

  printf("TImer is set\n");
}


void gtthread_init(long preemption_period_1){
	
// Initializing all teh data structures
  preemption_period = preemption_period_1;
  memset(threadMap,0,sizeof(threadMap));
  
  context_queue = (queue *) malloc(1* sizeof(*context_queue));
  context_queue->front = NULL; 
  context_queue->back = NULL;  
  context_queue->size = 0;
  
// Initializing main_thread ID
  mainThreadID = createNewThreadID(); 
  printf("Main thread id: %d\n", mainThreadID);

  context_node *mainContextNode = (context_node *) malloc(1* sizeof(* mainContextNode));

  mainContextNode->thread_id = mainThreadID;

  queue_push(mainContextNode);

// Initialize signal set  and unblock sigvtalrm signal as start

  sigemptyset(&vtalrm);
  sigaddset(&vtalrm, SIGVTALRM);
  unblockTimer();

  initializeTimer(preemption_period);
}

void blockTimer(){
  sigprocmask(SIG_BLOCK, &vtalrm, NULL);
}

void unblockTimer(){
  sigprocmask(SIG_UNBLOCK, &vtalrm, NULL);
}

int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *), void * args){
	context_node * new_thread_context = (context_node *) malloc(1* sizeof(context_node));
  
  new_thread_context->thread_id = createNewThreadID();

  printf("Child thread id: %d\n",new_thread_context->thread_id );
	if(getcontext(&(new_thread_context->context))==-1){
		perror("getcontext");
		exit(EXIT_FAILURE);
	}

	new_thread_context->context.uc_link = &mainThreadContext;
  new_thread_context->context.uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);

  makecontext(&new_thread_context->context,start_routine,0);

  blockTimer();

  queue_push(new_thread_context);

  unblockTimer();
}