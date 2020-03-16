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

#define MAXARGS 256
#define MAXCMDS 50

struct command {
	char *cmdargs[MAXARGS];
	char cmdflag;
};
/*  cmdflag's  */

#define OUTPIP  01
#define INPIP   02

extern struct command cmds[];
extern char *infile, *outfile, *appfile;
extern char bkgrnd;
extern struct sigaction stp,stp1,stpIGN,stpDFL;

int parseline(char *);
int promptline(char *, char *, int);
int exceptions (int);


struct fon {
	int taskIndex;
	struct fon *next;
};

extern struct fon *head;

void pushB (int index);

int runAllB ();

int stepB ();

void killAllB();

void printList ();

struct jobsArr {
	int* zero;
	int size;
	int head;
	int tail;
};

extern struct jobsArr ja;
void createJobsArr();
void printJobsArr();
void deleteJobsArr();
int pushJobsArr(int pid);
int popJobsArr(int index);
int getJobsArr(int index);

