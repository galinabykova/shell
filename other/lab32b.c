#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MAX_RCV_SZ 60

int main (int argc,char *argv[]) {
	if (argc<2) {
		printf("%s name(number>0)\n",argv[0]);
		return 0;
	}
	char s[BUFSIZ];
	key_t key;
	int qid;
	if ((key=ftok(".",getuid()))==-1) {
			fprintf(stderr,"Error1\n");
			return 0;
	}
	qid=msgget(key,0660);
	if (qid==-1) {
			fprintf(stderr,"Error2\n");
			return 0;
	}
	struct msgbuf  {
       long mtype;
       char mtext[BUFSIZ];
    } myMsg;
    myMsg.mtype=atoi(argv[1]);
    while (fgets(s,BUFSIZ,stdin)) {
		strcpy(myMsg.mtext,s);
		if (msgsnd(qid,&myMsg,strlen(myMsg.mtext)+1,0)==-1) {
			fprintf(stderr,"Error\n");
			msgctl(qid,IPC_RMID,NULL);
			return 0;
		}
	}
	strcpy(myMsg.mtext,"end");
	if (msgsnd(qid,&myMsg,strlen(myMsg.mtext)+1,0)==-1) {
			fprintf(stderr,"Error\n");
			return 0;
	}
	return 0;
}
