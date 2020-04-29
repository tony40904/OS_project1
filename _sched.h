#ifndef MYSCHED_H_
#define MYSCHED_H_

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <semaphore.h>
#include <math.h>
#include <errno.h>
#include <sched.h>
#include "list.h"

#define wait_unit(unit) ({ for(size_t i = 0; i < unit; ++i)wait_unit_1; })
#define wait_unit_1 ({ volatile unsigned long i; for(i = 0; i < 1000000UL; i++); })
#define policy(S) (POLICY[(int)S%7%4])
#define DEBUG

struct ready_queue{
	struct list_head list;
	long start, exe;
	pid_t pid;
};


void swap(int * a, int * b);
void sort(int value[], int index[], int N, int ascend);
void resort(int value[], int index[], int N, int ascend, const int T[], const int T_inverse[]);
void inverse_permutation(const int a[], int b[], int N);
struct ready_queue * find_shortest(struct ready_queue *ready);
void check_terminate(struct ready_queue *ready, struct sched_param *param, unsigned long local_clock, int *flag);
struct ready_queue * check_preempt(struct ready_queue *ready, struct ready_queue *tmp, unsigned long local_clock, int *preempt);
void check_remain(struct ready_queue *ready, struct sched_param *param, unsigned long * local_clock);

#endif
