#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "monitor.h"

#define FREE 0
#define BEING_USED 1
#define LEFT_FORK i
#define RIGHT_FORK right[i]

pthread_cond_t *forks_condition_variables;
pthread_mutex_t mutex;
int *forks; // forks[i] == p indicates that the philosopher p has the fork i

extern int *states;
extern int *left;
extern int *right;
extern int num_philosophers;

void initMonitor(int num_phil) {
	int i;

	// Allocate and initialize forks array
	forks = malloc(sizeof(int) * num_phil);
	for (i=0; i < num_phil; i++) {
		forks[i] = FREE;
	}

	// Allocate and initialize condition variables
	forks_condition_variables = malloc(sizeof(pthread_cond_t) * num_phil);
	for (i=0; i<num_phil; i++) {
		pthread_cond_init(&forks_condition_variables[i], NULL);
	}

	// Initialize mutex
	pthread_mutex_init(&mutex, NULL);
}

void putForks(int i){
	pthread_mutex_lock(&mutex);

	states[i] = THINKING;
	printStates();

	forks[LEFT_FORK] = FREE;
	pthread_cond_signal(&forks_condition_variables[LEFT_FORK]);

	forks[RIGHT_FORK] = FREE;
	pthread_cond_signal(&forks_condition_variables[RIGHT_FORK]);

	pthread_mutex_unlock(&mutex);
}

void takeForks(int i){
	pthread_mutex_lock(&mutex);

	states[i] = HUNGRY;
	printStates();

	int first_fork_index = (i == 0) ? RIGHT_FORK : LEFT_FORK;
	int second_fork_index = (i == 0) ? LEFT_FORK : RIGHT_FORK;

	while (tryGetForks(first_fork_index) == 0) {
		pthread_cond_wait(&forks_condition_variables[first_fork_index], &mutex);
	}
	while (tryGetForks(second_fork_index) == 0) {
		pthread_cond_wait(&forks_condition_variables[second_fork_index], &mutex);
	}
	// Acquired both forks
	states[i] = EATING;
	printStates();

	pthread_mutex_unlock(&mutex);
}

/**
 * The functions below are only called from within the monitor, and
 * thus don't need to be protected by the mutex. They must not be called
 * from outside of the monitor.
 */

int tryGetForks(int i){
	if (forks[i] == FREE) {
		forks[i] = BEING_USED;
		return 1;
	}
	else {
		return 0;
	}
}

void printStates(){
	int i=0;
	for(i=0; i<num_philosophers-1; i++){
		if(states[i] == 0)
			printf("T - ");
		if(states[i] == 1)
			printf("H - ");
		if(states[i] == 2)
			printf("E - ");
	}
	if(states[i] == 0)
		printf("T\n");
	if(states[i] == 1)
		printf("H\n");
	if(states[i] == 2)
		printf("E\n");

}
