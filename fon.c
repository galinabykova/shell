#include "shell.h"

struct fon *head=NULL;
struct fon *now=NULL;
struct fon *last=NULL;

void pushB (int index) {
	struct fon *node=malloc(sizeof(struct fon));
	node->taskIndex=index;
	if (!node) {
		fprintf(stderr,"Не выделил памяти ВООБЩЕ");
		return;
	}
	node->next=head;
	head=node;
}

int runAllB () {
	now=head;
	last=NULL;
	int everythingIsDeleted=0;
	while (now) {
		if (stepB()) everythingIsDeleted=1;
	}
	return everythingIsDeleted;
}

int stepB () {
	if (!now) {
		now=head;
		last=NULL;
	}
	if (!now) return 0;
	int index=now->taskIndex;
	int pidB=getJobsArr(index);
	if (waitpid(pidB,0,WNOHANG)) { 
		if (last) last->next=now->next;
		else head=now->next;
		popJobsArr(index);
		struct fon *z=now;
		now=now->next;
		free(z);
		return 1;
	} else {
		last=now;
		now=now->next;
	}
	return 0;
}

void killAllB () {
	struct fon *now1=head;
	while (now1) {
		kill(popJobsArr(now1->taskIndex),SIGKILL);
		struct fon *z=now1;
		now1=now1->next;
		free(z);
	}
	printf("\n");
}

void printList () {
	struct fon *now1=head;
	while (now1) {
		printf("%d ",now1->taskIndex);
		now1=now1->next;
	}
	printf("\n");
}

void testB () {
	pushB(1);
	pushB(2);
	pushB(3);
	printList();
	runAllB(1);
	printList();
	runAllB(3);
	printList();
	runAllB(2);
	printList();
	pushB(5);
	printList();
	runAllB(5);
}