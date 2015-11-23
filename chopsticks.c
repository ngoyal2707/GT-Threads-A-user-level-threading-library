#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include "gtthread.h"

#include "philosopher.h"
#include "chopsticks.h"

gtthread_mutex_t chopsticks_mutex[5];
/*
 * Performs necessary initialization of mutexes.
 */
void chopsticks_init(){
	int i;
	for(i=0;i<5;i++){
		if(gtthread_mutex_init(&chopsticks_mutex[i])!=0){
			printf("Lock init errors\n");
		} 
	}
}

/*
 * Cleans up mutex resources.
 */
void chopsticks_destroy(){
	int i;
	for(i=0;i<5;i++){
		if(gtthread_mutex_destroy(&chopsticks_mutex[i])!=0){
			printf("Lock destroy errors\n");
		} 
	}
}

/*
 * Uses pickup_left_chopstick and pickup_right_chopstick
 * to pick up the chopsticks
 */   
void pickup_chopsticks(int phil_id){
	int rightChop = phil_id;
	int leftChop = (phil_id+4)%5;

	if(leftChop>rightChop){
		gtthread_mutex_lock(&chopsticks_mutex[leftChop]);
		gtthread_mutex_lock(&chopsticks_mutex[rightChop]);
		pickup_left_chopstick(phil_id);
		pickup_right_chopstick(phil_id);
	}else{
		gtthread_mutex_lock(&chopsticks_mutex[rightChop]);
		gtthread_mutex_lock(&chopsticks_mutex[leftChop]);
		pickup_right_chopstick(phil_id);
		pickup_left_chopstick(phil_id);
	}
}

/*
 * Uses pickup_left_chopstick and pickup_right_chopstick
 * to pick up the chopsticks
 */   
void putdown_chopsticks(int phil_id){
	int rightChop = phil_id;
	int leftChop = (phil_id+4)%5;

	if(leftChop>rightChop){
		putdown_right_chopstick(phil_id);
		putdown_left_chopstick(phil_id);
		gtthread_mutex_unlock(&chopsticks_mutex[rightChop]);
		gtthread_mutex_unlock(&chopsticks_mutex[leftChop]);
	}else{
		putdown_right_chopstick(phil_id);
		putdown_left_chopstick(phil_id);
		gtthread_mutex_unlock(&chopsticks_mutex[leftChop]);
		gtthread_mutex_unlock(&chopsticks_mutex[rightChop]);
	}
}
