#ifndef JOBS_H
#define JOBS_H 

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <signal.h>
#include <termios.h>
#include <sys/wait.h>

struct Task {
        pid_t pid;
        int cnt;
        int next;
        char* name;
        int placeInOrder;
};

struct TaskStruct {

        int headFreeTask; 
        int tailFreeTask;
    
        int headWorkTask;

        struct Task* zero;

        int* order;
        int lastInOrder;

        int size;
};


struct TaskStruct ta; 

int newTaskStruct (); 
void deleteTaskStruct();
void printTaskStruct();
int increaseTaskStruct();

int findExited(); 
int pushTask(int pid, char *n, int cnt);
int changeTaskCnt(int index, int cnt);
void killAllTasks();
int getTaskPid(int index); 
int getTaskCnt(int index); 
int markToPop(int index); 

void nearJobs(); 
void printExitedJobs(int now); 

int peekOrder();
int pushToOrder(int index); 
int popToOrder(int index);

/* JOBS_H: */
#endif
