#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define THINKING 0
#define HUNGRY 1
#define EATING 2

pthread_t * thread;
sem_t *mutex;
sem_t *sem_p;
int *state;
int num_philosof;
int *left, *right;

void printStates(){
	int i=0;
	for(i=0; i<num_philosof-1; i++){
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

void tryGetForks(int i){
	if(state[i] == HUNGRY && state[left[i]] != EATING && state[right[i]] != EATING){
		state[i] = EATING;
		printStates();
		sem_post(&sem_p[i]);
	}
}

void put_forks(int i){
	sem_wait(mutex);
	state[i] = THINKING;
	printStates();
	tryGetForks(left[i]);  // check if neigh. can eat
	tryGetForks(right[i]);
	sem_post(mutex);       // end of critical region
}

void take_forks(int i){
	sem_wait(mutex);
	state[i] = HUNGRY;
	printStates();
	tryGetForks(i);       // try to get forks
	sem_post(mutex);      // end of critical region
	sem_wait(&sem_p[i]);  // block if forks weren't caught
}

void eat(int p){
	int time = 0;
	time = rand() % 10 + 1;
	sleep(time);
}

void think(int p){
	int time = 0;
	time = rand() % 10 + 1;
	sleep(time);
}

void initializeStates(){
	int i;
	for(i=0; i<num_philosof; i++){
		state[i] = THINKING;
	}
	printStates();

}
void* philosopher(void* i){
	int p = (int) i;
	while(1){
		think(p);
	    take_forks(p);
	 	eat(p);
		put_forks(p);

	}
	return 0;
}

int main (int argc, char **argv) {

	int i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <number of philosophers>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	num_philosof = strtol(argv[1], NULL, 10);
	if(num_philosof <= 0){
		fprintf(stderr, "Number of philosophers must be up to 0.\n");
		exit(EXIT_FAILURE);
	}

	// define neighborhood
	left = malloc(sizeof(int) * num_philosof);
	right = malloc(sizeof(int) * num_philosof);
	for(i=0; i<num_philosof; i++){

		right[i] = (i+num_philosof-1) % num_philosof;
		left[i] = (i+1) % num_philosof;
	}

	// allocate mutex
	mutex = malloc (sizeof(sem_t));
	sem_init(mutex, 0, (num_philosof-1));

	// allocate semaphore for each philosopher
	sem_p = malloc(sizeof(sem_t) * num_philosof);
	for(i=0; i<num_philosof; i++){
		sem_init(&sem_p[i], 0, 1);
	}

	// allocate thread for each philosopher
	thread = malloc(sizeof(pthread_t) * num_philosof);

	// allocate states - all starts hungry
 	state = malloc(sizeof(int) * num_philosof);
	initializeStates();

	for(i=0; i < num_philosof; i++){
		if(pthread_create(&thread[i], NULL, philosopher, (void *) i) != 0){
			printf("Error creating threads \n");
		}
	}

	for(i=0; i < num_philosof; i++){
     	pthread_join(thread[i], NULL);
	}


    return 0;
}
