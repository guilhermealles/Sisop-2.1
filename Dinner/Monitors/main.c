#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "monitor.h"
#include "philosopher.h"

pthread_t *philosophers;

int main (int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <number of philosophers>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int num_phil = strtol(argv[1], NULL, 10);
	if (num_phil <= 1) {
		fprintf(stderr, "Error: number of philosophers must be greater than 1.\n");
		exit(EXIT_FAILURE);
	}

	initMonitor(num_phil);
	initializeStates(num_phil);

	// allocate thread for each philosopher
	philosophers = malloc(sizeof(pthread_t) * num_phil);

	int i;
	for(i=0; i < num_phil; i++) {
		// This area of memory will be freed as soon as the thread is created.
		int *arg = malloc(sizeof(int));
		*arg = i;
		if(pthread_create(&philosophers[i], NULL, philosopher, (void *)arg) != 0) {
			free(arg);
			fprintf(stderr, "Error creating threads.\n");
			exit(EXIT_FAILURE);
		}
	}

	for(i=0; i < num_phil; i++){
     	pthread_join(philosophers[i], NULL);
	}

	//TODO Clean up allocated memory

    return 0;
}
