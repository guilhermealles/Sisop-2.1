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
		right[i] = (i+num_philosophers-1) % num_philosophers;
		left[i] = (i+1) % num_philosophers;
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
    // Free memory area that was allocated for the params
    free(i);

	while(1){
		think();
	    takeForks(p);
	 	eat();
		putForks(p);
	}
	return 0;
}
