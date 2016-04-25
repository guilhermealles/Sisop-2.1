#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "philosopher.h"

pthread_t *philosophers;

int main (int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <number of philosophers>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int num_phil = strtol(argv[1], NULL, 10);
	
	initMonitor(num_phil);
	
	// allocate thread for each philosopher
	philosophers = malloc(sizeof(pthread_t) * num_phil);
	
	int i;
	for(i=0; i < num_phil; i++) {
		if(pthread_create(&philosophers[i], NULL, philosopher, (void *) i) != 0) {
			fprintf(stderr, "Error creating threads.\n");
			exit(EXIT_FAILURE);
		}
	}
	
	while (1) {
		sleep(1);
		printStates();
	}

	for(i=0; i < num_phil; i++){
     	pthread_join(philosophers[i], NULL);
	}
	
	//TODO Clean up allocated memory

    return 0;
}
