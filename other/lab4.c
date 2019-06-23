#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct element {
	char *ptr;
	struct element *next;
} element;

int main () {
	char s[sysconf(_SC_LINE_MAX)]; //sysconf, чтобы не нужно было компилировать при изменении машинки
	element *first=malloc(sizeof(element));
	element *tec=first;
	while (1) {
	gets(s); //вообще-то эта штука срывает буфер. Почему мы используем её? Или типа безопасно, если мы правильно указали размер?
	int size=strlen(s);
	if (size>0 && s[0]=='.') {
		break;
	}
	char *ptr=malloc(sizeof(char)*(size+1));//
	if (!ptr) {
		printf("Memory error\n");
 		while (tec) {
                printf("%s\n",(tec->ptr));
                element *z=tec;
                tec=tec->next;
                free(z->ptr);
                free(z);
        }

	}
	tec->ptr=ptr;
	tec->next=malloc(sizeof(element));
	tec=tec->next;
	tec->next=NULL;
	for (int i=0;i<size;++i) {
		ptr[i]=s[i];
	}
	ptr[size]='\0';
	}
	tec=first;
	while (tec->next) {
		printf("%s\n",(tec->ptr));
		element *z=tec;
		tec=tec->next;
		free(z->ptr);
		free(z);
	}
	free(tec);
}
