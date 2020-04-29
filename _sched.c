#define _GNU_SOURCE
#include "list.h" 
#include "_sched.h"

// 1->FIFO, 2->RR
int POLICY[2] = {1, 2};

struct ready_queue * check_preempt(struct ready_queue *ready, struct ready_queue *tmp, unsigned long local_clock, int * preempt){
	struct ready_queue *tmp1;
	
	tmp1 = list_entry(ready->list.next, struct ready_queue, list);

	if(tmp != tmp1){
		if(tmp1->start + tmp1->exe - local_clock > tmp->exe){
			tmp1->exe = tmp1->start + tmp1->exe - local_clock;
			tmp1->start = -1;
			*preempt = 1;
			list_del(&(tmp->list));
			list_add(&(tmp->list), &(ready->list));
			tmp->start = local_clock;
		}
	}
	else{
		*preempt = 1;
	}
	return tmp1;
}


struct ready_queue * find_shortest(struct ready_queue *ready){
	long long shortest = 100000000000; 
	struct list_head *shortest_pos, *pos;
	struct ready_queue *tmp;
	list_for_each(pos, &(ready->list)){
		tmp = list_entry(pos, struct ready_queue, list);
		if( tmp->exe < shortest ){
			shortest = tmp->exe;
			shortest_pos = pos;
		}
	}
	tmp = list_entry(shortest_pos, struct ready_queue, list);
	list_del(&(tmp->list));
	list_add(&(tmp->list), &(ready->list));
	
	return tmp;
}

// 1 -> ascending, 0 -> descending
void swap(int * a, int * b){
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void inverse_permutation(const int a[], int b[], int N){
	int count = 0, cursor = 0, probe = 0;
	for(unsigned i = 0; i < N; ++i)
		b[i] = -1;
	
	while(count < N){
		if(b[a[probe]] == -1){
			b[a[probe]] = probe;
			probe = a[probe];
			++count;
		}
		else{
			for(; cursor < N; ++cursor)
				if(b[cursor] == -1){
					probe = cursor;
					break;
				}
		}
	}
}


void check_terminate(struct ready_queue *ready, struct sched_param *param, unsigned long local_clock, int *flag){
	struct ready_queue *tmp;

	tmp = list_entry(ready->list.next, struct ready_queue, list);

	if(tmp->start + tmp->exe == local_clock){
		list_del(&(tmp->list));
		if(!list_empty(&ready->list)){
			tmp = find_shortest(ready);
			tmp->start = local_clock;
			if(tmp->pid){
		 		if(sched_setscheduler(tmp->pid, SCHED_FIFO, param)){
					printf("3 sched_setscheduler error: %s\n", strerror(errno));
					exit(1);
				}
			}
			else{
				*flag = 1;
			}
		}
	}
}

void sort(int value[], int index[], int N, int ascend){
	if(ascend){
		for(unsigned i = 0; i < N; ++i){
			for(unsigned j = i + 1; j < N; ++j){
				if(value[i] > value[j]){
					swap(&value[i], &value[j]);
					swap(&index[i], &index[j]);
				}
			}
		}
	}
	else{
		for(unsigned i = 0; i < N; ++i){
			for(unsigned j = i + 1; j < N; ++j){
				if(value[i] < value[j]){
					swap(&value[i], &value[j]);
					swap(&index[i], &index[j]);
				}
			}
		}
	}
}


void resort(int value[], int index[], int N, int ascend, const int T[], const int T_inverse[]){
        unsigned head = 0, end = 0;
        if(ascend){
                while(end + 1 < N){
                        if(value[head] == value[end + 1]){
                                ++end;
                        }
                        if(value[head] != value[end + 1] || end + 1 == N){
                                for(unsigned i = head; i <= end; ++i){
                                        for(unsigned j = head; j <= end; ++j){
                                                if(T[T_inverse[index[i]]] < T[T_inverse[index[j]]]){
                                                        swap(&index[i], &index[j]);
                                                }
                                        }
                                }
                                head = (++end);
                        }
                }
        }
        else{
                while(end + 1 < N){
                        if(value[head] == value[end + 1]){
                                ++end;
                        }
                        if(value[head] != value[end + 1] || end + 1 == N){
                                for(unsigned i = head; i < end; ++i){
                                        for(unsigned j = head + 1; j <= end; ++j){
                                                if(T[T_inverse[index[i]]] > T[T_inverse[index[j]]]){
                                                        swap(&index[i], &index[j]);
                                                }
                                        }
                                }
                                head = (++end);
                        }
                }
        }
}

void check_remain(struct ready_queue *ready, struct sched_param *param, unsigned long * local_clock){
	struct ready_queue *tmp;
	tmp = list_entry(ready->list.next, struct ready_queue, list);
	while(*local_clock < tmp->start + tmp->exe){
		wait_unit_1;
		++(*local_clock);
	}
	
	list_del(&(tmp->list));
	if(!list_empty(&ready->list)){	
		tmp = find_shortest(ready);
		tmp->start = *local_clock;
		if(sched_setscheduler(tmp->pid, SCHED_FIFO, param)){
			printf("3 sched_setscheduler error: %s\n", strerror(errno));
			exit(1);
		}
	}
}

int main(){

	#ifdef DEBUG
	setbuf(stdout, NULL);
	#endif
	
	char S[4];
	int N;
	scanf("%s\n%d", S, &N);
	
	char P[N][32];
	int R[N], T[N];
	for(size_t i = 0; i < N; ++i){
		scanf("%s %d %d", P[i], &R[i], &T[i]);
	}

	int R_index[N], T_index[N], R_inverse[N], T_inverse[N];
	for(size_t i = 0; i < N; ++i)
		R_index[i] = T_index[i] = i;
	
	cpu_set_t mask, mask1;

	CPU_ZERO(&mask);
	CPU_SET(0, &mask);
	
	CPU_ZERO(&mask1);
	CPU_SET(1, &mask1);

	// cpu 1 -> main
	if(sched_setaffinity(0, sizeof(cpu_set_t), &mask1)){
		printf("sched_setaffinity error: %s\n", strerror(errno));
		exit(1);
	}
	
	// set the scheduler
	struct sched_param param, param_;
	param.sched_priority = sched_get_priority_max((S[0] == 'R') ? SCHED_RR : SCHED_FIFO);
	param_.sched_priority = 0;
	
	// cpu 0 -> child
	if(S[0] == 'F' || S[0] == 'R'){
		if(sched_setscheduler(0, policy(S[0]), &param)){
			printf("1 sched_setscheduler error: %s\n", strerror(errno));
			exit(1);
		}
	}
	
  	// sort the ready and execution time, record the index in *_index
	sort(R, R_index, N, 1);
	sort(T, T_index, N, 1);
	inverse_permutation(R_index, R_inverse, N);
	inverse_permutation(T_index, T_inverse, N);
	if(S[0] == 'S' || S[0] == 'P')
		resort(R, R_index, N, 1, T, T_inverse);
	
	// start
	pid_t pid;
	unsigned long local_clock = 0;
	unsigned long i = 0;
	struct timespec start;
	struct ready_queue ready, *tmp, *tmp1;
	
	INIT_LIST_HEAD(&ready.list);

	for(; i < N; ++i){
		int empty, preempt = 0;
		int mypid, flag = 0;
		
		// wait to be forked
		while(local_clock != R[i]){
			if((S[0] == 'S' || S[0] == 'P') && !list_empty(&ready.list))
				check_terminate(&ready, &param, local_clock, &flag);
			
			wait_unit_1;
			++local_clock;
		}
			
		empty = list_empty(&ready.list);
		tmp = (struct ready_queue*)malloc(sizeof(struct ready_queue));
		tmp->start = (empty) ? R[i] : -1;
		tmp->exe = T[T_inverse[R_index[i]]];
		list_add_tail(&(tmp->list), &(ready.list));
		
		if(S[0] == 'S' || S[0] == 'P'){
			if(!list_empty(&ready.list)){
				check_terminate(&ready, &param, local_clock, &flag);
				if(S[0] == 'P'){
					tmp1 = check_preempt(&ready, tmp, local_clock, &preempt);
				}
			}
		}


		pid = fork();

		if(!pid){
			mypid = getpid();
			syscall(336, 1, mypid, &start);
			printf("%s %d\n", P[R_index[i]], getpid());
			
			for(unsigned long _i = 0; _i < T[T_inverse[R_index[i]]]; ++_i){
				wait_unit_1;
			}
	
			syscall(336, 0, mypid, &start);
			exit(0);
		}
		else if(pid == -1){
			printf("Fork error!\n");
			exit(1);
		}
		else{
			tmp->pid = pid;
			
			// cpu 0 -> child
			if(sched_setaffinity(pid, sizeof(cpu_set_t), &mask)){
				printf("sched_setaffinity error: %s\n", strerror(errno));
				exit(1);
			}
			if(flag){
				if(sched_setscheduler(pid, SCHED_FIFO, &param)){
					printf("policy: %d, sched_setscheduler error: %s\n", SCHED_FIFO, strerror(errno));
					exit(1);
				}
			}
			else{
				if(S[0] == 'S' || S[0] == 'P'){
					if(empty){
						if(sched_setscheduler(pid, SCHED_FIFO, &param)){
							printf("policy: %d, sched_setscheduler error: %s\n", SCHED_FIFO, strerror(errno));
							exit(1);
						}
					}
					else{
						if(S[0] == 'P' && preempt){
							// need to add to fifo first, to avoid empty fifo ready queue,
							// which will make idle start to run!
							if(sched_setscheduler(pid, SCHED_FIFO, &param)){
								printf("policy: %d, sched_setscheduler error: %s\n", SCHED_FIFO, strerror(errno));
								exit(1);
							}
							if(sched_setscheduler(tmp1->pid, SCHED_IDLE, &param_)){
								printf("policy: %d, sched_setscheduler error: %s\n", SCHED_IDLE, strerror(errno));
								exit(1);
							}
		      			}
						else{
							if(list_entry(ready.list.next, struct ready_queue, list)->pid == pid){
								if(sched_setscheduler(pid, SCHED_IDLE, &param_)){
									printf("policy: %d, sched_setscheduler error: %s\n", SCHED_IDLE, strerror(errno));
									exit(1);
								}
							}
						}
					}
				}
			}
			
			if((S[0] == 'S' || S[0] == 'P') && !list_empty(&ready.list))
				check_terminate(&ready, &param, local_clock, &flag);
		}
	}
	
	while(!list_empty(&ready.list) && (S[0] == 'S' || S[0] == 'P')){
		check_remain(&ready, &param, &local_clock);
	}

	while(wait(NULL) > 0);
	
	exit(0);
}
