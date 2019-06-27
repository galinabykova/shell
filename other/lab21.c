#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

int cnt;
struct termios last;

void obr (int a) {
	if (a==SIGINT) {
		++cnt;
		putchar('\07');
		fflush(0);
	}
	if (a==SIGQUIT) {
		printf("%d\n",cnt);
		tcsetattr(fileno(stdin),TCSANOW,&last);
		exit(0);
	}
}
int main () {
	struct termios my;
	tcgetattr(fileno(stdin),&last);
	my=last;
	my.c_lflag=my.c_lflag&(~ECHO);
	my.c_cc[VINTR]=27;
	tcsetattr(fileno(stdin),TCSANOW,&my);
	struct sigaction a;
	a.sa_handler=obr;
	a.sa_flags=0;
	sigaction(SIGINT,&a,NULL);
	sigaction(SIGQUIT,&a,NULL);
	while (1) {
		pause();
	}
	tcsetattr(fileno(stdin),TCSANOW,&last);
	return 0;
}

/*Ctrl-4 sigquit*/