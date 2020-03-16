#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <sys/ddi.h> //for Solaris
#include <string.h>
#include <wait.h>
#include "shell.h"

int exceptions (int i) {
	if (!strcmp(cmds[i].cmdargs[0],"cd")) {
		chdir(cmds[i].cmdargs[1]);
		//если аргументов нет, куда переносит cd?
		return 1;
	}
	if (!strcmp(cmds[i].cmdargs[0],"exit")) {
		killAllB();
		deleteJobsArr();
		exit(0);
	}	
	if (!strcmp(cmds[i].cmdargs[0],"showTask")) {
		printJobsArr();
		return 1;
	}
	if (!strcmp(cmds[i].cmdargs[0],"showB")) {
		printList();
		return 1;
	}
 	if (!strcmp(cmds[i].cmdargs[0],"fg")) {
 		int indexN;
 		if (!cmds[i].cmdargs[1]) {
 			runAllB();
 			if (!head) return 1;
 			indexN=head->taskIndex;
 		} else {
			indexN=atoi(cmds[i].cmdargs[1]);
		}
		int pidN=getJobsArr(indexN);
		printf("%d\n",pidN);
		if (pidN<0) return 1;

		if (tcsetpgrp(0,getpgid(pidN))<0)
			printf("(\n");
		kill(pidN,SIGCONT);

		int w;
		if (waitpid(pidN,&w,WUNTRACED)==-1) {
            fprintf(stderr,"ERROR waitpid");
        }

		sigaction(SIGTTOU,&stpIGN,0);
		if (tcsetpgrp(0,getpgid(0))<0)
			printf("(\n");
		sigaction(SIGTTOU,&stpDFL,0);

		if (WIFEXITED(w)) {
			printf("%d\n",WEXITSTATUS(w));
			printf("\nexited\n");
		} else {
			if (WIFSIGNALED(w)) {
				printf("\nsignaled\n");
				printf("%s\n",strsignal(WTERMSIG(w)));
			}
		}
		if (WIFCONTINUED(w)) {
			printf("\nWHY\n");
		}
		if (WIFSTOPPED(w)) {
			kill(pidN,SIGCONT);
			printf("\nstopped\n");
 			printf("process %d was running in the background\n",pidN);
		}
		return 1;
	}
	//также нужно реализовать
	/*
	 * pushd and popd
     * logout, bye, etc
     * fg, bg, jobs, and the & suffix
     * history
     * set, unset, export
    */
	return 0;
}