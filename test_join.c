#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "gtthread.h"

void *thr1(void *in) {
  return ((int)in) * 2;
}

int main() {
  gtthread_init(1000);

  void * r1, * r2, * r3;
  r1 = (void *)malloc(sizeof(int));
  gtthread_t t1, t2, t3;

  gtthread_create(&t1, thr1, 1);
  gtthread_join(t1, &r1);
  printf("%d\n", r1);

  gtthread_create(&t2, thr1, r1);
  gtthread_join(t2, &r2);

  gtthread_create(&t3, thr1, r2);
  gtthread_join(t3, &r3);
  
  printf("return value: %d\n", r3);
  fflush(stdout);
  return 0;
}