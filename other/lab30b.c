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
	qid=msgget(getuid(),0660);
	struct msgbuf  {
       long mtype;
       char mtext[MAX_RCV_SZ];
    } myMsg;
	for (int i=0;i<10;++i) {
		int t=msgrcv(qid,&myMsg,MAX_RCV_SZ,0L,0);
		printf("type: %ld\nmsg: %s\nret: %d\n",myMsg.mtype,myMsg.mtext,t);
	}
	msgctl(qid,IPC_RMID,NULL);
	return 0;
}