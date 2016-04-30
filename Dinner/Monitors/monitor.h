#ifndef MONITOR_H
#define MONITOR_H

#define THINKING 0
#define HUNGRY 1
#define EATING 2

void initMonitor(int num_phil);
void putForks(int i);
void takeForks(int i);
int tryGetForks(int i);
void printStates();

#endif
