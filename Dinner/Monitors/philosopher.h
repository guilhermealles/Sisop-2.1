#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#define THINKING 0
#define HUNGRY 1
#define EATING 2

void initMonitor(int num_phil);
void initializeStates();
void printStates();
void tryGetForks(int i);
void putForks(int i);
void takeFoks(int i);
void eat(int p);
void think(int p);
void* philosopher(void* i);

#endif
