#include "shell.h"
#include "jobs.h"
char *infile, *outfile, *appfile;
struct command cmds[MAXCMDS];
int ncmds;
char bkgrnd;
struct sigaction stp, stpDFL, stpIGN;
struct termios term;
int pid;
int fd[2];
extern int crit;

pid_t p = 0,groupP = 0;
int indexOfTask = 0;
int lastIndexB = 0;
void signalToChild (int a); 
void signalToParent (); 
void toBkgrnd(int a); 

int indexToPid(int i)
{
        int indexN;
        if (!cmds[i].cmdargs[1]) {
                indexN = peekOrder();
        } else {
                indexN = atoi(cmds[i].cmdargs[1]);
        }
        indexOfTask = indexN;
        popToOrder(indexN);
        return getTaskPid(indexN);
}

void closeInAndOut(int in, int out) 
{
    if (in != STDIN_FILENO) {
         close(in);
    }
    if (out != STDOUT_FILENO) {
        close(out);
    }
}

void waitAndAfter(int i)
{
        int w;
        int cnt=ncmds;
        if (indexOfTask>0) {
            cnt=getTaskCnt(indexOfTask);
        }
        int cntToWhile=cnt;
        while (cntToWhile > 0) {
                if (waitpid(-groupP, &w, WUNTRACED)==-1) {
                    --cntToWhile;
                    cnt=0;
                    break;
                }
                if (WIFEXITED(w)) {
                    //printf("%d\n", WEXITSTATUS(w));
                    //printf("exited\n");
                    --cnt;
                }
                if (WIFSIGNALED(w)) {
                    printf("\rsignaled\n");
                   // printf("\r%s\n", strsignal(WTERMSIG(w)));
                    --cnt;
                } 
                --cntToWhile;
        }
        sigaction(SIGTTOU, &stpIGN, 0);
        if (tcsetpgrp(0, getpgid(0)) < 0) {
                fprintf(stderr, "failed to tcsetpgrp\n");
        }
        sigaction(SIGTTOU, &stpDFL, 0);
        tcsetattr(0, TCSAFLUSH, &term);
        changeTaskCnt(indexOfTask, cnt);
        if (cnt > 0) {
                if (!indexOfTask) {
                        indexOfTask=pushTask(groupP, cmds[i].cmdargs[0], cnt); //
                } else {
                        pushToOrder(indexOfTask);
                }
                if (indexOfTask < 0) {
                        fprintf(stderr, "Task was not added, but process from group %d was stopped\n", groupP);  
                } else {
                        printf("[%d] Stopped\n", indexOfTask);
                }
        }
 }

void parentDoIt (int i, int in, int out)
{
        closeInAndOut(in,out);//
        if (!(cmds[i].cmdflag & INPIP) && setpgid(p, 0) == -1) { //
                fprintf(stderr, "failed to make the process a group leader\n");
                return;
        }
        //
        if ((cmds[i].cmdflag & INPIP) && setpgid(p,groupP) == -1) {
                fprintf(stderr, "failed to add process to group\n");
        }
        //
        if (!(cmds[i].cmdflag & INPIP)) {
                indexOfTask = 0;
        }
        if (!(cmds[i].cmdflag & OUTPIP)) {
                if (!(cmds[i].cmdflag & OUTPIP)) { //
                        if (bkgrnd) {
                                //
                                int cnt=ncmds;
                                indexOfTask = pushTask(groupP, cmds[i].cmdargs[0],cnt);   //???????????????????????????????????????????   
                                if (indexOfTask < 0) {
                                        fprintf(stderr, "Task was not added, but process %d was runned in the background\n", p);
                                } else {
                                        printf("[%d] %d\n", indexOfTask, p);
                                }
                        }  else {
                                //if (tcsetpgrp(0, groupP) < 0)//
                                //      printf("(\n");
                                waitAndAfter(i);      
                        }
                }
        }
}

int changeFile (char cond, int last, char *file, int flags, int *save)
{
        if (cond) {
                *save = open(file, flags, 0666);
                if (*save < 0) {
                        fprintf(stderr, "file did not open%s\n", file);
                        return -1;
                } else {
                        close(last);
                        dup(*save);
                        close(*save);//
                }
        //close(*save);
 }
        return 0;
}

//stderr
void childDoIt (int i, int ncmds, int *newin, int *newout, int in, int out,int nin)
{
        closeInAndOut(nin,STDOUT_FILENO);
        sigaction(SIGTTOU, &stpDFL, 0);//?
        sigaction(SIGTTIN, &stpDFL, 0);
        sigaction(SIGTSTP, &stpDFL, 0);
        sigaction(SIGINT, &stpDFL, 0);
        sigaction(SIGQUIT, &stpDFL, 0);
        if (!(cmds[i].cmdflag & INPIP) && setpgid(0,0) == -1) {
                fprintf(stderr, "failed to make the process a group leader\n");
                return;
        }
        if ((cmds[i].cmdflag & INPIP) && setpgid(0, groupP) == -1) {
                fprintf(stderr, "failed to add process to group\n");
        }
        //
        if (bkgrnd) {
                //sigaction(SIGINT, &stpIGN, 0);
                //sigaction(SIGQUIT, &stpIGN, 0);
        } else {
                sigaction(SIGTTOU, &stpIGN, 0);
                if (tcsetpgrp(0, getpgid(0)) < 0) {
                        printf("failed to tcsetpgrp\n");
                }
                sigaction(SIGTTOU, &stpDFL, 0);
        }
        int f = 0;
        f |= changeFile(infile && i == 0, STDIN_FILENO, infile, O_RDONLY, newin);
        f |= changeFile(outfile && i == ncmds - 1, STDOUT_FILENO, outfile, O_WRONLY|O_CREAT|O_TRUNC, newout);
        f |= changeFile(appfile && i == ncmds - 1, STDOUT_FILENO, appfile, O_WRONLY|O_CREAT|O_APPEND, newout);

        if (f) {
                exit(0);
        }

        // 
        if (cmds[i].cmdflag & INPIP) {
                close(STDIN_FILENO);
                dup(in);
                close(in);
        }
        if (cmds[i].cmdflag & OUTPIP) {
                close(STDOUT_FILENO);
                dup(out);
                close(out);
        }
        //
        if (execvp(cmds[i].cmdargs[0], cmds[i].cmdargs) == -1) {
                fprintf(stderr, "Unknown command\n");
                exit(127);
        }
        exit(0);
}

int main(int argc,char *argv[])
{
        if (newTaskStruct() == -1) {
                fprintf(stderr, "No memory. Impossible to continue");
                exit(0);
        }
        register int i;
        char line[1024];      /*  allow large command lines  */
        char prompt[50];      /* shell prompt */

        /* PLACE SIGNAL CODE HERE */
        stpDFL.sa_handler = SIG_DFL;
        stpIGN.sa_handler = SIG_IGN;
        tcgetattr(0, &term);

        stp.sa_flags = 0;
        stp.sa_handler = signalToParent;
        sigaction(SIGINT, &stp, 0);

        sigaction(SIGTSTP, &stpIGN, 0);
        sigaction(SIGQUIT, &stpIGN, 0);
        //sigaction(SIGTTIN, &stpIGN, 0); //
        sprintf(prompt,"[%s] ", argv[0]);

        int nin=STDIN_FILENO;//

        int out = STDOUT_FILENO;
        int in = STDIN_FILENO;

        while (1) {    /*until eof  */
                if (!crit) {
                        findExited();
                }
                if (promptline(prompt, line) <= 0) {
                        continue;
                }
                if ((ncmds = parseline(line)) <= 0) {
                        continue;   /* read next line */
                }
                int newin, newout;
                p = 0;
                groupP=0;

                for (i = 0; i < ncmds; i++) {
                        // 
                        in = nin;
                        if (cmds[i].cmdflag & OUTPIP) {
                            if (pipe(fd)==-1) {
                                fprintf(stderr,"failed to pipe\n"); //
                                break;
                            }
                            nin = fd[0]; 
                            out = fd[1];
                        } else {
                            out = STDOUT_FILENO;
                            nin = STDIN_FILENO;
                        }

                        //
                        if (!cmds[i].cmdargs[0]) {
                                continue;
                        }
                        if (!strcmp(cmds[i].cmdargs[0], "cd")) {
                                chdir(cmds[i].cmdargs[1]);
                                closeInAndOut(in,out);
                                continue;
                        }

                        if (!strcmp(cmds[i].cmdargs[0], "exit")) {
                                deleteTaskStruct();
                                exit(0);
                        }

                        if (!strcmp(cmds[i].cmdargs[0], "showTask")) {
                                printf("lastIndexB %d\n",lastIndexB);
                                printTaskStruct();
                                closeInAndOut(in,out);
                                continue;
                        }

                        if (!strcmp(cmds[i].cmdargs[0], "fg")) { //??????????????????????????????????????????????
                                groupP = indexToPid(i); //&
                                p = groupP;
                                int cnt=getTaskCnt(indexOfTask);
                                while (groupP && waitpid(-groupP, 0, WNOHANG)>0 && cnt>0) {
                                    --cnt;
                                }
                                changeTaskCnt(indexOfTask,cnt);
                                if (!groupP) {
                                        printf("incorrect number of job\n");
                                        continue;
                                }
                                if (!cnt) {
                                        continue;
                                }
                                if (tcsetpgrp(0,groupP) < 0) printf("failed to tcsetpgrp\n");
                                kill(-groupP,SIGCONT);
                                waitAndAfter(i);
                                closeInAndOut(in,out);
                                continue;
                        }

                        if (!strcmp(cmds[i].cmdargs[0], "bg")) {
                                groupP = indexToPid(i); //&
                                p = groupP;
                                if (p && !kill(-groupP,SIGCONT)) printf("[%d] %d\n", indexOfTask, p);
                                if (!groupP) {
                                        printf("incorrect number of job\n");
                                }
                                closeInAndOut(in,out);
                                continue;
                        }

                        if (!strcmp(cmds[i].cmdargs[0], "jobs")) {
                                nearJobs();
                                closeInAndOut(in,out);
                                continue;
                        }

                        p = fork();
                        if (!(cmds[i].cmdflag & INPIP)) {
                            groupP=p;
                        }
                        //fprintf(stderr, "p %d, predP %d\n",p,predP);
                        switch (p) {
                                case -1: {
                                        fprintf(stderr, "process not created\n");
                                        break;
                                }
                                case 0: {
                                        childDoIt(i, ncmds, &newin, &newout, in, out,nin);
                                        break;
                                }
                                default: {
                                        parentDoIt(i, in, out);
                                }
                        }
                }
        }  /* close while */
}

 /* PLACE SIGNAL CODE HERE */
void signalToParent (int sigName)
{
        if (sigName == SIGINT) {
                if (p && !bkgrnd) {
                        kill(p, sigName);
                }
                return;
        }
}

