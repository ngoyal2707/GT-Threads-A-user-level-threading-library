CC = gcc            # default is CC = cc
CFLAGS = -g -Wall   # default is CFLAGS = [blank]

GTTHREADS_SRC = gtthread_sched.c gtthread_mutex.c steque.c
GTTHREADS_OBJ = $(patsubst %.c,%.o,$(GTTHREADS_SRC))

# pattern rule for object files
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

#### Producer-Consumer ####
producer_consumer: producer_consumer.o steque.o
	$(CC) -o producer_consumer producer_consumer.o steque.o -lpthread

#### Dining ####
# dining_main: dining_main.o
# 	$(CC) -o dining_main dining_main.o -lpthread

#### GTThreads ####
gtthread_main: gtthread_main.o $(GTTHREADS_OBJ)
	$(CC) -o gtthread_main gtthread_main.o $(GTTHREADS_OBJ)

#### Dining ####	
dining_main: dining_main.o chopsticks.o philosopher.o $(GTTHREADS_OBJ)
	$(CC) -o dining_main dining_main.o chopsticks.o philosopher.o  $(GTTHREADS_OBJ)

#### Dining ####	
dining_main_pthread: dining_main.o 
	$(CC) -o dining_main dining_main.o chopsticks.o philosopher.o -lpthread

#### GTThreads ####
test_join: test_join.o $(GTTHREADS_OBJ)
	$(CC) -o test_join test_join.o $(GTTHREADS_OBJ)

#### GTThreads ####
test_join2: test_join2.o $(GTTHREADS_OBJ)
	$(CC) -o test_join2 test_join2.o $(GTTHREADS_OBJ)
clean:
	$(RM) -f *.o producer_consumer gtthread_main dining_main dining_main_pthread test_join2 test_join
