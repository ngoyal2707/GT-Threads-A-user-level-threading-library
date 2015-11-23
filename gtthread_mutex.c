#include "gtthread.h"


int gtthread_mutex_init(gtthread_mutex_t *mutex){

	blockTimer();

	if(mutex->mutex_lock == 1){
		return -1;
	}
	mutex->mutex_lock = 0;
	mutex->mutex_owner = current_context->thread_id;
	
	unblockTimer();
	return 0; 
}
int gtthread_mutex_lock(gtthread_mutex_t *mutex){
	
	blockTimer();
	
	// Possibly reentrant lock
	if(mutex->mutex_owner == current_context->thread_id && mutex->mutex_lock==1){
		unblockTimer();
		return 0;
	}

	// Wait till the lock is free 
	while(mutex->mutex_lock == 1 && mutex->mutex_owner != current_context->thread_id){
		unblockTimer();
		gtthread_yield();
		blockTimer();
	};
	mutex->mutex_lock = 1;
	mutex->mutex_owner = current_context->thread_id;

	unblockTimer();
	return 0;
}
int  gtthread_mutex_unlock(gtthread_mutex_t *mutex){
	blockTimer();

	if(mutex->mutex_owner != current_context->thread_id){
		unblockTimer();
		return -1;
	}

	// if(mutex->mutex_owner == )
	mutex->mutex_lock = 0;
	mutex->mutex_owner = -1;
	unblockTimer();
	return 0;
}
int  gtthread_mutex_destroy(gtthread_mutex_t *mutex){
	blockTimer();
	mutex->mutex_owner=-1;
	mutex->mutex_lock = 0;
	return 0;
}