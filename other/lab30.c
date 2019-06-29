#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MAX_RCV_SZ 60

int main () {
	int qid;
	qid=msgget(getuid(),IPC_CREAT|IPC_EXCL|0660);
	struct msgbuf {
       long mtype;
       char mtext[MAX_RCV_SZ];
    } myMsg;
	for (int i=0;i<10;++i) {
		myMsg.mtype=getuid();
		strcpy(myMsg.mtext,"lyalya");
		msgsnd(qid,&myMsg,strlen(myMsg.mtext)+1,0);
	}
	//sleep(100);
	//msgctl(qid,IPC_RMID,NULL);
	return 0;
}

	/*
	key_t key;
	int qid;
	if ((key=ftok(".",'a'))==-1) {
		perror("Error");
		return 0;
	}
	qid=msgget(key,IPC_CREAT|IPC_EXCL|0660);
	if (qid==-1) {
		perror("Error");
		return 0;
	}
	struct msgbuf myMsg;
	myMsg.mtype=1L;
	myMsg.mtext="lyalya";
	if (msgsnd(qid,&myMsg,strlen(my.Msg.mtext)+1,0)==-1) {
		perror("Error");
		return 0;
	}
	msgctl(mid,IPC_RMId,NULL);
	return 0;
	*/