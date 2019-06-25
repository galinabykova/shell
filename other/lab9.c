#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc,char *argv[]) {
	pid_t p=fork();
	if (p<0) {
		printf("Error\n");
		return 0;
	}
	if (p) {
		waitpid(p,0,0);
		printf("The end\n");
	} else {
		argv[0]="cat";
		execvp("cat",argv);
		printf("Error\n");
	}
}
