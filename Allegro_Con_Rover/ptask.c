/*file       ptask.c
details      This module contains the functions to create and manage the tasks

author     Davide Rasla
todo       
*/  
#include "main.h"

//----------------------------------------
//			PTASK LIBRARY
//----------------------------------------

/*
This function creates a periodic task 
setting the scheduler algorithm to SCHED_FIFO
*/
int task_create(int i, void*(*task)(void *), int period, int drel, int prio){
	
	pthread_attr_t 	myatt;
	struct 			sched_param mypar;
	int 			tret;
	printf("Created task: %d\n",i );
	tp[i].arg 		= i;
	tp[i].period 	= period;
	tp[i].deadline 	= drel;
	tp[i].priority 	= prio;
	tp[i].dmiss 	= 0;

	pthread_attr_init(&myatt);
	pthread_attr_setinheritsched(&myatt,PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&myatt, SCHED_FIFO);

	mypar.sched_priority = tp[i].priority;
	pthread_attr_setschedparam(&myatt, &mypar);

	tret = pthread_create(&tid[i], &myatt, task, (void*)(&tp[i]));
      if (tret != 0) {
          perror("task_create");
          return -1;
        }
	return tret;

}



/*
Retrieves the task index stored in tp->arg.
*/

int get_task_index(void* arg){	
	struct task_par *tp;
	tp = (struct task_par *)arg;
	return tp->arg;
}



/*
Retrieves the task period stored in tp[i].period.
*/

int task_period(int i){
	return tp[i].period;
}



/*
Reads the current time and computes the next activation
time and the absolute deadline of the task.
*/

void set_activation(int i){

	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	
	time_copy(&(tp[i].at), t);
	time_copy(&(tp[i].dl), t);

	time_add_ms(&(tp[i].at), tp[i].period);
	time_add_ms(&(tp[i].dl), tp[i].deadline);

}



/*
If the thread is still in execution when re‐activated, it
increments the value of dmiss and returns 1, otherwise
returns 0.
*/

int deadline_miss(int i){

	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	
	if (time_cmp(&now, tp[i].dl) > 0) {
		tp[i].dmiss++;
		return 1;
	}
	
	return 0;
}



/*
Suspends the calling thread until the next activation and,
when awaken, updates activation time and deadline.

NOTE: Even though the thread calls time_add_ms() after
the wake‐up time, the computation is correct.
*/

void wait_for_activation(int i){

	clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &(tp[i].at), NULL);
	
	time_add_ms(&(tp[i].at), tp[i].period);
	time_add_ms(&(tp[i].dl), tp[i].period);
}



/*
The main thread blocks until the other created threads finish
*/

void wait_for_task_end(int i){
	pthread_join(tid[i], NULL);
}



/*
Sets the piority of the main thread higher than the others and
the policy to use to schedule the threads
*/

void ptask_init(int policy){
	
	struct sched_param my_par;
	my_par.sched_priority = PRI + 7;

	pthread_setschedparam(pthread_self(), policy, &my_par);
}