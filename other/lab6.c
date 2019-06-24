#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

typedef struct cell {
	size_t offset;
	size_t size;
} cell;

int desc;
int cnt;

void ifSigAlarmDo (int a) {
	lseek(desc,0,SEEK_SET);
	char *ct=malloc(sizeof(char)*cnt);	
	read(desc,ct,cnt);
	write(STDOUT_FILENO,ct,cnt); //1 - desc stdout
	free(ct);
	exit(0);
}
int main (int argc,char* argv[]) {
	if (argc==1) {
		printf("I need a file\n");
		return 0;
	}
	desc=open(argv[1],O_RDONLY);
	if (desc==-1) {
		printf("I can't open a file\n");
		return 0;
	}
	char c=1;
	cell table[101];
	int tec=0;
	table[0].offset=0;
	struct sigaction str;
	str.sa_handler=ifSigAlarmDo;
	str.sa_flags=0;
	sigaction(SIGALRM, &str, NULL);
	while (read(desc,&c,1)>0) {
		++cnt;
		if (c=='\n'){
			table[tec].size=cnt-table[tec].offset;
			++tec;
			table[tec].offset=cnt;
		}
	}
	int need;
	do {
		alarm(5);
		printf("Enter the number of string (<=%d) or 0 to exit within 5 seconds\n",tec);
		scanf("%d",&need);
		alarm(0);
		if (need>tec) {
			continue;
		}
		lseek(desc,table[need-1].offset,SEEK_SET);
		char *ct=malloc(sizeof(char)*table[need-1].size);	
		read(desc,ct,table[need-1].size);
		write(STDOUT_FILENO,ct,table[need-1].size); //1 - desc stdout
		free(ct);
	} while (need);
	close(desc);
	return 0;
}
