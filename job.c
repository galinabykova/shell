#include "shell.h"

const int N=10;

int step=5;

struct jobsArr ja;

void createJobsArr() {
	ja.size=0;
	ja.zero=NULL;
	ja.head=0;
	ja.tail=0;
}
void zoomJobsArr() {
	if (ja.size==0) {
		ja.zero=malloc(sizeof(int)*N);
		ja.size=N;
		if (!ja.zero) {
			fprintf(stderr,"Не выделил памяти ВООБЩЕ");
			return;
		}
		for (int i=0;i<N;++i) {
			ja.zero[i]=-i-1;
		}
	} else {
		int *new;
		new=realloc(ja.zero,sizeof(int)*((ja.size)+step));
		while (!new && step>10) {
			step=step-10;
			new=realloc(ja.zero,ja.size+step);
		}
		while (!new && step>0) {
			--step;
			new=realloc(ja.zero,ja.size+step);
		}
		if (!step) {
			fprintf(stderr,"Нет памяти для увеличения размера arr");
			return;
		}
		int newsize=(ja.size)+step;
		for (int i=ja.size;i<newsize;++i) {
			new[i]=0-i-1;
		}
		ja.zero=new;
		ja.size=newsize;
	}
	ja.tail=ja.size-1;
}
void printJobsArr(){
	printf("Head %d\n",ja.head);
	printf("Tail %d\n",ja.tail);
	for (int i=0;i<ja.size;++i) {
		printf("%d %d\n",i,ja.zero[i]);
	}
	printf("\n");
}
void deleteJobsArr() {
	free(ja.zero);
}
int pushJobsArr(int pid) {
	if (!ja.zero) zoomJobsArr();
	int place=-ja.zero[0];
	if (place==ja.size && !runAllB()) {
		zoomJobsArr();
	}
	ja.zero[0]=ja.zero[place];
	ja.zero[place]=pid;
	return place;
}
int popJobsArr(int index) {
	if (index>=ja.size) {
		fprintf(stderr,"Incorrect index");
		return -1;
	}
	int pid=ja.zero[index];
	ja.zero[ja.tail]=-index;
	ja.tail=index;
	ja.zero[ja.tail]=-ja.size;
	return pid;
}
int getJobsArr(int index) {
	return *(ja.zero+index);
}
int testJ () {
	struct jobsArr ja;
	createJobsArr();
	/*
		добавить процессы 101, 102, 103
	*/
	pushJobsArr(101);
	pushJobsArr(102);
	pushJobsArr(103);
	/*
		удалить задания 1, 2

	*/
	popJobsArr(1);	
	popJobsArr(2);	
	/*
		добавить процессы 201, 212
	*/
	for (int i=1;i<13;++i) {
		pushJobsArr(200+i);
	}
	/*
		удалить 1, 2 ... 5 задания
	*/
	for (int i=1;i<6;++i) {
		popJobsArr(i);
	}
	printJobsArr();
	deleteJobsArr();
}