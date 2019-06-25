//Для версий exec(2), не передающих среду исполнения в качестве параметра, в качестве новой среды используется массив указателей, на который указывает внешняя переменная environ.
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

extern char ** environ;

int execvpe (const char *file, char *const argv[], char * /*const */envp[]) {
	environ=envp;
	execvp(file,argv);
	return -1;
}

int main (int argc,char *argv[]) {
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
		char *k[3];
		k[0]="ty=5";
		k[1]="lq=slfj";
		k[2]=NULL;
		execvpe(argv[1],&(argv[1]),k);
		printf("Error\n");		
	}
}
