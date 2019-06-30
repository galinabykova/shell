#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MAX_RCV_SZ 60

int main (int argc,char * argv[]) {
	if (argc<2) {
		printf("%s cnt\n",argv[0]);
		return 0;
	}
	int cntB=atoi(argv[1]);
	key_t key;
	int qid;
	char s[BUFSIZ];
	if ((key=ftok(".",getuid()))==-1) {
			fprintf(stderr,"Error\n");
			return 0;
		}
	qid=msgget(key,IPC_CREAT|IPC_EXCL|0660);
	if (qid==-1) {
		fprintf(stderr,"Error\n");
		return 0;
	}
	struct msgbuf {
	   	long mtype;
    	char mtext[BUFSIZ];
	} myMsg;
	while (fgets(s,BUFSIZ,stdin)) {
		myMsg.mtype=1L;
		strcpy(myMsg.mtext,s);
		if (msgsnd(qid,&myMsg,strlen(myMsg.mtext)+1,0)==-1) {
			fprintf(stderr,"Error\n");
			msgctl(qid,IPC_RMID,NULL);
			return 0;
		}
	}
	myMsg.mtype=2L;
	strcpy(myMsg.mtext,"The end");
	for (int i=0;i<cntB;++i) {
		if (msgsnd(qid,&myMsg,strlen(myMsg.mtext)+1,0)==-1) {
			fprintf(stderr,"Error\n");
			msgctl(qid,IPC_RMID,NULL);
			return 0;
		}
	}
	for (int i=0;i<cntB;++i) {
		if (msgrcv(qid,&myMsg,BUFSIZ,0L,0)==-1) {
			fprintf(stderr,"Error\n");
			msgctl(qid,IPC_RMID,NULL);
			return 0;
		}
	}
	msgctl(qid,IPC_RMID,NULL);
	return 0;
}
