#ifndef PTASK_H
#define PTASK_H

int task_create(int i, void*(*task)(void *), int period, int drel, int prio);

int get_task_index(void* arg);

void set_activation(int i);

int deadline_miss(int i);

void wait_for_activation(int i);

void wait_for_task_end(int i);

int task_period(int i);

void ptask_init(int policy);

#endif
