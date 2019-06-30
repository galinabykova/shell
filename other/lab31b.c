#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MAX_RCV_SZ 60

int main (int argc,char *argv[]) {
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
    myMsg.mtype=1;
	while (1) {
		if (msgrcv(qid,&myMsg,BUFSIZ,0L,0)==-1) {
			fprintf(stderr,"Error3\n");
			return 0;
		}
		if (myMsg.mtype==2L) {
			break;
		}
		printf("%s: %s",argv[0],myMsg.mtext);
	}
	myMsg.mtype=3;
	if (msgsnd(qid,&myMsg,strlen(myMsg.mtext)+1,0)==-1) {
			fprintf(stderr,"Error4\n");
			return 0;
	}
	return 0;
}
