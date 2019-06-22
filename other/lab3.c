#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

const char nameFile[10]="lab3file";
int main () {
	printf("Real User ID=%d\n",getuid());
	printf("Effective User ID=%d\n",geteuid());
	FILE *fp=fopen(nameFile,"r");
	if (fp) {
		printf("I have opened and closed\n");	
		fclose(fp);
	} else {
		perror("I can't open file\n");		
	}
	if (setuid(getuid())<0) {
		perror("Error(\n");
	}
	printf("Real User ID=%d\n",getuid());
	printf("Effective User ID=%d\n",geteuid());
	fp=fopen(nameFile,"w+");
	if (fp) {
		printf("I have opened and closed\n");
		fclose(fp);
	} else {
		perror("I can't open file\n");		
	}	
	return 0;
}

/*
Примерные команды (lab3.out)
gcc lab3.c -o lab3
touch lab3file
chmod 600 lab3file
echo "user:\n" >lab3rez.txt
./lab3 >>lab3rez.txt
echo "tairon:\n" >>lab3rez.txt
sudo -u tairon ./lab3 >>lab3rez.txt
chmod u+s ./lab3
echo "tairon:\n" >>lab3rez.txt
sudo -u tairon ./lab3 >>lab3rez.txt
*/

/*
http://ccfit.nsu.ru/~deviv/courses/unix/unix/ng12c8a.html
*/

/*
Помни, что s-бит к ИСПОЛНЯЕМОМУ файлу
*/
