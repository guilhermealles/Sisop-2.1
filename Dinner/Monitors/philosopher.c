#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "philosopher.h"

pthread_cond_t *condition_variables;
pthread_mutex_t mutex;

int *states;
int num_philosophers;
// left and right arrays are the indices of the neighbours of the philosopher i
int *left, *right;

void initMonitor(int num_phil) {
	int i;
	num_philosophers = num_phil;

	// define neighborhood
	left = malloc(sizeof(int) * num_philosophers);
	right = malloc(sizeof(int) * num_philosophers);
	for(i=0; i<num_philosophers; i++){
		right[i] = (i+num_philosophers-1) % num_philosophers;
		left[i] = (i+1) % num_philosophers;
	}

	// Allocate and initialize condition variables
	condition_variables = malloc(sizeof(pthread_cond_t) * num_philosophers);
	for (i=0; i<num_philosophers; i++) {
		pthread_cond_init(&condition_variables[i], NULL);
	}

	// Initialize mutex
	pthread_mutex_init(&mutex, NULL);

	// allocate states and initialize states
 	states = malloc(sizeof(int) * num_philosophers);
	initializeStates();
}

void initializeStates() {
	int i;
	for(i=0; i<num_philosophers; i++){
		states[i] = THINKING;
	}
	printStates();
}

void printStates(){
	int i=0;
	for(i=0; i<num_philosophers; i++){
		if(states[i] == 0)
			printf("T - ");
		if(states[i] == 1)
			printf("H - ");
		if(states[i] == 2)
			printf("E - ");

		if (i == (num_philosophers - 1) )
			printf ("\n");
	}

}

void tryGetForks(int i){
	if(states[i] == HUNGRY && states[left[i]] != EATING && states[right[i]] != EATING){
		states[i] = EATING;
		printStates();
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
	printStates();
	tryGetForks(i);       // try to get forks
	while (states[i] == HUNGRY) {
		pthread_cond_wait(&condition_variables[i], &mutex);
	}

	pthread_mutex_unlock(&mutex);
}

void eat(){
	int time = 0;
	time = rand() % 10 + 1;
	sleep(time);
}

void think(){
	int time = 0;
	time = rand() % 10 + 1;
	sleep(time);
}

void* philosopher(void* i){
	int p = *((int*)i);
	while(1){
		think();
	    takeForks(p);
	 	eat();
		putForks(p);

	}
	return 0;
}
