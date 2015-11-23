#include <stdio.h>
#include <stdlib.h>
#include "gtthread.h"

/* Tests creation.
   Should print "Hello World!" */

void *thr1(void *in) {
/*  while(1){*/
    int i = 0;
    for (i = 0; i < 10; ++i){
    printf("1 %d\n",i);	
  }
  return NULL;
}

/*void *thr2(void *in) {*/
/*  while(1){*/
/*    int i = 0;*/
/*    for (i = 0; i < 100; ++i){*/
/*    printf("2 %d\n",i);*/
/*  }*/
/*  return NULL;*/
/*}*/

int main() {
  gtthread_t t1;
  printf("In main\n");
  gtthread_init(0);
  printf("Create thread 1\n");
  gtthread_create( &t1, thr1, NULL);
/*  printf("Create thread 2\n"); */
/*  gtthread_create( &t2, thr2, NULL); */
/*  while(1){ */
    int i = 0;
    for (i = 0; i < 10; ++i){
    printf("main %d\n",i);    
  }
  gtthread_join(t1, NULL);
/*  gtthread_join(t2, NULL); */
  return EXIT_SUCCESS;
}










/*void *thr1(void *in) {*/
/*  printf("Hello World!\n");*/
/*  fflush(stdout);*/
/*  return NULL;*/
/*}*/

/*int main() {*/
/*  gtthread_t t1;*/

/*  gtthread_init(0);*/
/*  gtthread_create( &t1, thr1, NULL);*/

/*  gtthread_yield();*/

/*  return EXIT_SUCCESS;*/
/*}*/
