#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

void child1 (int d) {
	char c='A';
	for (;c<'z';++c) { //Ctrl-D
		write(d,&c,1);
	}
	close(d);
}

void child2 (int d) {
	char c;
	while (read(d,&c,1)) {
		printf("%c",toupper(c));
		//fflush(stdout);
	}
	printf("\n");
	close(d);
}

int main () {
	int fd[2];
	if (pipe(fd)==-1) {
		perror("Error\n");
		return 0;
	}
	int p=fork();
	switch (p) {
		case -1:
			perror("Error\n");
			return 0;
		case 0:
			child1(fd[1]);
			return 0;
	}
	close(fd[1]);
	p=fork();
	switch (p) {
		case -1:
			perror("Error\n");
			return 0;
		case 0:
			child2(fd[0]);
			return 0;
	}
	close(fd[0]);
	wait(0);
	wait(0);
	return 0;
}