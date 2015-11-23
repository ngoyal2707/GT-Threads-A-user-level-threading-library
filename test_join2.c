#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "gtthread.h"

static gtthread_t main_thread;

void *thr1(void *in) {
  printf("thread 1: entering\n");

  void * ret_val;
  gtthread_join( main_thread, &ret_val);

  printf("return from main: %d\n", ret_val);

  printf("thread 1: exiting\n");
  fflush(stdout);
  return 0;
}

int main() {
  printf("main: entering\n");

  gtthread_init(100);

  main_thread = gtthread_self();

  gtthread_t t1;
  gtthread_create(&t1, thr1, NULL);

  gtthread_exit(100);

  printf("main: exiting\n");
  fflush(stdout);
  return 0;
}