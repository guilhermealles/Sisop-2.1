#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "philosopher.h"
#include "monitor.h"

int *states;
// left and right arrays are the indices of the neighbours of the philosopher i
int *left, *right;
int num_philosophers;

void initializeStates(int num_phil) {
    num_philosophers = num_phil;
    states = malloc(sizeof(int) * num_philosophers);

	int i;
	for(i=0; i<num_philosophers; i++){
		states[i] = THINKING;
	}

    // define neighborhood
	left = malloc(sizeof(int) * num_philosophers);
	right = malloc(sizeof(int) * num_philosophers);
	for(i=0; i<num_philosophers; i++){
		right[i] = (i+1) % num_philosophers;
		left[i] = (i+num_philosophers-1) % num_philosophers;
	}
}

void eat(int p){
	int time = 0;
	time = rand() % 10 + 1;
    //printf("Philosopher %d will eat for %d seconds.\n", p, time);
	sleep(time);
}

void think(int p){
	int time = 0;
	time = rand() % 10 + 1;
    //printf("Philosopher %d will think for %d seconds.\n", p, time);
	sleep(time);
}

void* philosopher(void* i){
	int p = *((int*)i);
    // Free memory area that was allocated for the params
    free(i);

	while(1){
		think(p);
	    takeForks(p);
	 	eat(p);
		putForks(p);
	}
	return 0;
}
