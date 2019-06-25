//Захваты, которые не сняты явно с использованием fcntl(2), снимаются при завершении процесса с помощью exit(2) или при закрытии файла с помощью close(2).
/*Режим захвата (рекомендательный или обязательный) определяется атрибутами файла. 
 * По умолчанию, в Unix используется рекомендательный захват. Чтобы установить обязательный захват, 
 * файлу необходимо установить бит setgid, 
 * но при этом файл не должен быть доступен по исполнению для группы. 
 * Все захваты, устанавливаемые на такой файл, будут работать в обязательном режиме.

 * Рекомендательный (advisory) режим захвата не взаимодействует с подсистемой ввода/вывода. 
 * Это означает, что захват проверяется только при попытках захвата, но не при операциях чтения и записи. 
 * Как и семафоры и мутексы при работе с памятью, рекомендательный захват указывает другим процессам, что им не следует работать с захваченным участком, 
 * но не ставит физических препятствий.
.* Обязательный (mandatory) режим захвата взаимодействует с подсистемой ввода/вывода. 

 * Это означает, что системные вызовы read(2) или write(2) будут приостанавливаться (неявный захват), пока не будет снят захват соответствующей записи.

 * По умолчанию захват происходит в advisory режиме
 * Mandatory locking включается атрибутом файла: bash>chmod +l file
 * Не работает на NFS (ваши домашние каталоги подключаются по NFS)
 * Используйте /tmp
 * chmod g+s file; chmod g-x file
 * chmode g-s file*/


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int main (int argc,char* argv[]) {
	if (argc<2) {
		printf("I need a file\n");
		return 0;
	}
	int fd=open(argv[1],O_RDWR||O_CREAT,666);
	if (fd<0) {
		printf("I can't open file\n");
		return 0;
	}
	struct flock str;
	str.l_type=F_WRLCK; 
	str.l_whence=SEEK_SET;
	str.l_start=0;
	str.l_len=0;
	if (fcntl(fd,F_SETLK,&str)<0) {
		printf("You should try later %d\n",errno);
		printf("%d %d %d %d %d %d %d %d %d\n",EACCES,EBADF,EDEADLK,EFAULT,EINTR,EINVAL,EMFILE,ENOLCK,EPERM);
		close(fd);
		return 0;
	}
	char s[sysconf(_SC_LINE_MAX)];
	sprintf(s,"vim %s",argv[1]);
	system(s);
	str.l_type=F_UNLCK;
	if (fcntl(fd,F_SETLK,&str)<0) {
		printf("Strange everything\n");
	}
	close(fd);
	return 0;
}
