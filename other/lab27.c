//Можно попробовать с p2open, но нужно на солярке

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>



int main (int argc,char *argv[]) {
	if (argc<2) {
		printf("%s file\n",argv[0]);
		return 0;
	}
	char line[BUFSIZ];
	FILE *f=fopen(argv[1],"r");
	if (!f) {
		perror("Error\n");
		return 0;
	}
	FILE *f1=popen("wc -l","w");
	if (!f1) {
		perror("Error\n");
		return 0;
	}
	while (fgets(line,BUFSIZ,f)) {
		fputc(line[0],f1);
	}
	fclose(f);
	pclose(f1);
	return 0;
}