#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc,char *argv[]) {
	if (argc<2) {
		printf("More args\n");
		return 0;
	}
	pid_t p=fork();
	if (p<0) {
		printf("Error\n");
		return 0;
	}
	if (p) {
		int a;
		waitpid(p,&a,0);
		printf("%d\n",a);
	} else {
		execvp(argv[1],&(argv[1]));
		printf("Error\n");		
	}
}
