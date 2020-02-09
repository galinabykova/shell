//5 лекция Ожидание изменения состояния подпроцесса

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <sys/ddi.h> //for Solaris
#include <string.h>
#include <wait.h>
#include <signal.h>
#include <termios.h>
#include <sys/wait.h>
#include "shell.h"

char *infile, *outfile, *appfile;
struct command cmds[MAXCMDS];
char bkgrnd;
struct sigaction stp,stp1,stp2;
int pid;
int fd[2];

pid_t p=0,predP=0;
void signalToParent (int sigName);
void signalToChild (int a);
void setSignalsToParent ();
void parentDoIt (int in,int out){
	if (bkgrnd) {
		//Есть подозрение, что если запустить очень много фоновых процессов, можно достигнуть максимума по количеству процессов. При этом эти процессы вполне себе могут завершиться, но будут занимать место в таблице процессоров.
		//Нужно как-то считывать? Есть что-то похожее на waitpid, но не wait? Тип если завершился, убирает из таблицы, а иначе сразу завершается?
 		printf("В фоновом режиме запущен процесс pid=%d\n",p);
 	} else {
 	  	//а если  группа не успеет создаться?

		siginfo_t si;
		waitid(P_PID,p,&si,WEXITED|WSTOPPED);

 	  	if (si.si_code==CLD_STOPPED) {
			kill(p,SIGCONT);
		} 

		stp2.sa_handler=SIG_IGN;
		stp2.sa_flags=SA_ONESHOT;
		sigaction(SIGTTOU,&stp2,0);
		if (tcsetpgrp(0,getpgid(0))<0)
			printf("(\n");
		if (si.si_code==CLD_EXITED) {
			if (in!=STDIN_FILENO) {
				close(in);
			}
			if (out!=STDOUT_FILENO) {
				close(out);
			}
		}
	//	printf("я %d, ловит %d\n", getpid(),tcgetpgrp(0));//
 	  	p=0;
	//	printf("Do it\n");
 	}
}
void changeFile (char cond,int last, char*file, int flags, int *save) {
	if (cond) { //нужно проверить, имеем ли мы право на ввод или вывод
		*save=open(file,flags,0666); //нужно уточнять права?
		if (*save<0) {
			fprintf(stderr,"Не удалось открыть файл %s\n",file);
		} else {
			close(last);
			dup(*save);
		}
	//close(*save);
	}
}
//stderr
void childDoIt (int i,int ncmds,int *newin,int *newout,int in,int out) {
	//printf("будем читать в %d\n",in);
	//printf("будем записывать в %d\n",out);
	if (in==STDIN_FILENO && setpgid(0,0)==-1) {
		fprintf(stderr,"Не удалось сделать подроцесс лидером группы\n");
	}
	if (in!=STDIN_FILENO && setpgid(0,predP)==-1) {
		fprintf(stderr,"Не удалось сделать подроцесс лидером группы\n");
	}
	stp2.sa_handler=SIG_IGN;
	stp2.sa_flags=SA_ONESHOT;
	sigaction(SIGTTOU,&stp2,0);
	if (bkgrnd) {
		stp.sa_handler=SIG_IGN;
		sigaction(SIGINT,&stp,0);
		sigaction(SIGQUIT,&stp,0);
	} else {
		stp.sa_handler=SIG_DFL;
		sigaction(SIGINT,&stp,0);
		if (tcsetpgrp(0,getpgid(0))<0)
			printf("(\n");
	}
//	printf("я %d, ловит %d\n", getpid(),tcgetpgrp(0));//
// Есть подозрение, что следующие два if можно выполнить при помощи chandeFile. И мне не очень нравится проверка в цикле для каждого, хотя выполняем только в начале и в конце
	if (in!=STDIN_FILENO) {
		close(STDIN_FILENO);
		dup(in);
	}
	if (out!=STDOUT_FILENO) {
		close(STDOUT_FILENO);
		dup(out);
	}
 	changeFile(infile && i==0,STDIN_FILENO,infile,O_RDONLY,newin);  			
 	changeFile(outfile && i==ncmds-1,STDOUT_FILENO,outfile,O_WRONLY|O_CREAT|O_TRUNC,newout);  	 
 	changeFile(appfile && i==ncmds-1,STDOUT_FILENO,appfile,O_WRONLY|O_CREAT|O_APPEND,newout);
 	//exec ломает всё?

    if (execvp(cmds[i].cmdargs[0], cmds[i].cmdargs)==-1) {
 		fprintf(stderr,"Неизвестная команда\n");
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
    setSignalsToParent();
    sprintf(prompt,"[%s] ", argv[0]);
    int nin=STDIN_FILENO;
    int out=STDOUT_FILENO;
    int in=STDIN_FILENO;
    while (1) {    /*until eof  */
		if (promptline(prompt, line, sizeof(line))<=0) {
			//printf("Введите \n");
			//break;
			continue;
		}
    	if ((ncmds = parseline(line)) <= 0) {
    	    continue;   /* read next line */
    	}
 		int newin,newout;
 	   for (i = 0; i < ncmds; i++) {
 	   		in=nin;
 	   		//printf("%s\n",cmds[i].cmdargs[0]);//
 	   		//printf("cmds[i].cmdflag==INPIP: %d\n",cmds[i].cmdflag==INPIP);
 	   		//printf("cmds[i].cmdflag==OUTPIP: %d\n",cmds[i].cmdflag==OUTPIP);
 	   		//эта штука работает лишь для пары из-за парсера
 	   		if (cmds[i].cmdflag & OUTPIP) {
 	   			pipe(fd);
 	   			nin=fd[0];
 	   			out=fd[1];
 	   		} else {
 	   			out=STDOUT_FILENO;
 	   			nin=STDIN_FILENO;
 	   		}
 	   		if (exceptions(i)) {
 	   			continue;
 	   		}
 	   		predP=p;
 	   		p=fork();
 	   		switch (p) {
 	   			case -1: {
 	   				fprintf(stderr,"Подпроцесс не создался(\n");
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
//	printf("%d\n",sigName);
	if (sigName==SIGINT) {
		//printf("%d\n",p);
		if (p && !bkgrnd) {
			//printf("\n");
			kill(p,sigName);
		}
		//printf("Обработка\n");
		return;
	}
}

void setSignalsToParent () {
	stp.sa_flags=0;
	stp.sa_handler=signalToParent;
	//stp.sa_mask=0;
	sigaction(SIGINT,&stp,0);//здесь или не здесь?
		//sigaction(SIGTTOU,&stp,0);//
		//sigaction(SIGTTIN,&stp,0);//
//	sigaction(SIGTSTP,&stp,0);//здесь или не здесь?	
}

