#include "shell.h"

char *infile, *outfile, *appfile;
struct command cmds[MAXCMDS];
char bkgrnd;
struct sigaction stp,stp1,stp2,stp3,stp4,stpDFL,stpIGN;
int pid;
int fd[2];

pid_t p=0,predP=0,indexOfTask=0;
void signalToChild (int a);
void signalToParent ();
void toBkgrnd(int a);//перетащить в shell.h
void parentDoIt (int in,int out){
	if (in==STDIN_FILENO && setpgid(p,0)==-1) {
		fprintf(stderr,"failed to make the process a group leader\n");
	}
	if (in!=STDIN_FILENO && setpgid(p,predP)==-1) {
		fprintf(stderr,"failed to add process to group\n");
	}
	if (in==STDIN_FILENO) {
		indexOfTask=pushJobsArr(p);
	}
    if (bkgrnd) {
 		printf("process %d was running in the background\n",p);
 		pushB(indexOfTask);
 	} else {
 	/*	if (in!=STDIN_FILENO) {
		close(in);
        }
		if (out!=STDOUT_FILENO) {
			close(out);
		}*/
        
		//if (tcsetpgrp(0,getpgid(p))<0)//
		//	printf("(\n");

		int w;
		if (waitpid(p,&w,WUNTRACED | WCONTINUED)==-1) {
            fprintf(stderr,"ERROR waitpid");
        }

		sigaction(SIGTTOU,&stpIGN,0);
		if (tcsetpgrp(0,getpgid(0))<0)
			printf("(\n");
		sigaction(SIGTTOU,&stpDFL,0);

		if (WIFEXITED(w)) {
			printf("%d\n",WEXITSTATUS(w));
			popJobsArr(indexOfTask);
		} else {
			if (WIFSIGNALED(w)) {
				printf("%s\n",strsignal(WTERMSIG(w)));
			}
		}

		if (WIFSTOPPED(w)) {
			kill(p,SIGCONT);
 			printf("process %d was running in the background\n",p);
 			pushB(indexOfTask);
		} 
 	    p=0;
 	}
 	stepB();
}

void changeFile (char cond,int last, char*file, int flags, int *save) {
	if (cond) {
		*save=open(file,flags,0666); 
		if (*save<0) {
			fprintf(stderr,"file did not openÑ%s\n",file);
		} else {
			close(last);
			dup(*save);
		}
	//close(*save);
	}
}
//stderr
void childDoIt (int i,int ncmds,int *newin,int *newout,int in,int out) {
    sigaction(SIGTTOU,&stpDFL,0);//?
	sigaction(SIGTTIN,&stpDFL,0);
	sigaction(SIGTSTP,&stpDFL,0);
    sigaction(SIGINT,&stpDFL,0);
	if (in==STDIN_FILENO && setpgid(0,0)==-1) {
		fprintf(stderr,"failed to make the process a group leader\n");
	}
	if (in!=STDIN_FILENO && setpgid(0,predP)==-1) {
		fprintf(stderr,"failed to add process to group\n");
	}
	if (bkgrnd) {
        sigaction(SIGINT,&stpIGN,0);
    	sigaction(SIGQUIT,&stpIGN,0);
	} else {
		sigaction(SIGTTOU,&stpIGN,0);
		if (tcsetpgrp(0,getpgid(0))<0)
			printf("(\n");
		sigaction(SIGTTOU,&stpDFL,0);
	}
	/*if (in!=STDIN_FILENO) {
		close(STDIN_FILENO);
		dup(in);
	}
	if (out!=STDOUT_FILENO) {
		close(STDOUT_FILENO);
		dup(out);
	}*/
 	changeFile(infile && i==0,STDIN_FILENO,infile,O_RDONLY,newin);  			
 	changeFile(outfile && i==ncmds-1,STDOUT_FILENO,outfile,O_WRONLY|O_CREAT|O_TRUNC,newout);  	 
 	changeFile(appfile && i==ncmds-1,STDOUT_FILENO,appfile,O_WRONLY|O_CREAT|O_APPEND,newout);

	if (execvp(cmds[i].cmdargs[0], cmds[i].cmdargs)==-1) {
 		fprintf(stderr,"Unknown command\n");
        exit(127);
 	}
 	exit(0);
}



int main(int argc, char *argv[])
{
    register int i;
    char line[1024];      /*  allow large command lines  */
    int ncmds;
    char prompt[50];      /* shell prompt */

    /* PLACE SIGNAL CODE HERE */
	stpDFL.sa_handler=SIG_DFL;
	stpIGN.sa_handler=SIG_IGN;
	createJobsArr();

	//sigaction(SIGTTIN,&stpDFL,0);
	//sigaction(SIGTTOU,&stpDFL,0);//
	stp.sa_flags=0;
	stp.sa_handler=signalToParent;
	sigaction(SIGINT,&stp,0);

	//sigaction(SIGTTOU,&stpIGN,0);
	//sigaction(SIGTTIN,&stpIGN,0);
	sigaction(SIGTSTP,&stpIGN,0);

    sprintf(prompt,"[%s] ", argv[0]);
    //int nin=STDIN_FILENO;
    int out=STDOUT_FILENO;
    int in=STDIN_FILENO;

    while (1) {    /*until eof  */
		if (promptline(prompt, line, sizeof(line))<=0) {
			continue;
		}
    	if ((ncmds = parseline(line)) <= 0) {
    	    continue;   /* read next line */
    	}
 		int newin,newout;
 	   for (i = 0; i < ncmds; i++) {
 	   		//in=nin;
 	   		/*if (cmds[i].cmdflag & OUTPIP) {
 	   			pipe(fd);
 	    		nin=fd[0];
 	   			out=fd[1];
 	   		} else {
 	   			out=STDOUT_FILENO;
 	   			nin=STDIN_FILENO;
 	   		}*/
 	   		if (exceptions(i)) {
 	   			continue;
 	   		}
 	   		predP=p;//
 	   		p=fork();
 	   		switch (p) {
 	   			case -1: {
 	   				fprintf(stderr,"process not created\n");
 	   				break;
 	   			}
 	   			case 0: {
 	   				childDoIt(i,ncmds,&newin,&newout,in,out);
 	   				break;
 	   			}
 	   			default: {
 	   				parentDoIt(in,out);
 	   			}
 	   		}
    	}
  	
 	}  /* close while */
 }
 /* PLACE SIGNAL CODE HERE */
void signalToParent (int sigName) {
	if (sigName==SIGINT) {
		if (p && !bkgrnd) {
			kill(p,sigName);
		}
		return;
	}
}


