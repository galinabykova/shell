/* После исполнения tcgetattr(3C) рекомендуется сохранить копию этой структуры, чтобы програм­ма могла вернуть начальное состояние терминального интерфейса. 
 * Дело в том, что функции tcsetattr(3C) изменяют настройки не файлового дескриптора вашего процесса, а настройки драйвера в ядре Unix. 
 * Внесенные вами изменения не откатываются  автоматически при заверше­нии программы, 
 * поэтому ненормально завершившаяся программа может оставить терминал в непригодном для работы состоянии.

 * НО ПОЧЕМУ_ТО ЕСЛИ Я МЕНЯЮ НАСТРОЙКИ В ПОДПРОЦЕССЕ, ОНИ МЕНЯЮТСЯ БОЛЬШЕ ЖЁСТКО НЕЖЕЛИ В ПРОЦЕССЕ, ЗАПУЩЕННОМ ИЗ SHELL
 */

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <termios.h>

struct termios last;

void child1 (int d) {
	struct termios my;
	my=last;
	my.c_lflag=my.c_lflag&(~ICANON)&(~ECHO);
	tcsetattr(fileno(stdin),TCSANOW,&my);

	char c;
	while ((c=getchar())!=EOF) { //Ctrl-D
		printf("%c",toupper(c));
		//write(d,&c,1);
	}
	close(d);

	tcsetattr(fileno(stdin),TCSANOW,&last);
}

void child2 (int d) {
	char c;
	while (read(d,&c,1)) {
		printf("%c",toupper(c));
		fflush(stdout);
	}
	close(d);
}

int main () {
	tcgetattr(fileno(stdin),&last);

	int fd[2];
	pipe(fd);
	int p=fork();
	switch (p) {
		case -1:
			perror("Error\n");
			tcsetattr(fileno(stdin),TCSANOW,&last);
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
			tcsetattr(fileno(stdin),TCSANOW,&last);
			return 0;
		case 0:
			child2(fd[0]);
			return 0;
	}

	tcsetattr(fileno(stdin),TCSANOW,&last);
	close(fd[0]);
	wait(0);
	wait(0);
	return 0;
}