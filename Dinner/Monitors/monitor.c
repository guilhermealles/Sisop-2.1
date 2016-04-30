#include <stdlib.h>
#include <pthread.h>
#include "monitor.h"

pthread_cond_t *condition_variables;
pthread_mutex_t mutex;

extern int *states;
extern int *left;
extern int *right;

void initMonitor(int num_phil) {
	int i;

	// Allocate and initialize condition variables
	condition_variables = malloc(sizeof(pthread_cond_t) * num_phil);
	for (i=0; i<num_phil; i++) {
		pthread_cond_init(&condition_variables[i], NULL);
	}

	// Initialize mutex
	pthread_mutex_init(&mutex, NULL);
}

void tryGetForks(int i){
	if(states[i] == HUNGRY && states[left[i]] != EATING && states[right[i]] != EATING){
		states[i] = EATING;
		pthread_cond_signal(&condition_variables[i]);
	}
}

void putForks(int i){
	pthread_mutex_lock(&mutex);

	states[i] = THINKING;
	tryGetForks(left[i]);
	tryGetForks(right[i]);

	pthread_mutex_unlock(&mutex);
}

void takeForks(int i){
	pthread_mutex_lock(&mutex);

	states[i] = HUNGRY;

	tryGetForks(i);       // try to get forks
	while (states[i] == HUNGRY) {
		pthread_cond_wait(&condition_variables[i], &mutex);
	}

	pthread_mutex_unlock(&mutex);
}
