/* Обратите внимание, что длина имени файла не учитывается в размере структуры dirent, 
 * поэтому при размещении памяти под эту структуру недостаточно выделить sizeof(struct dirent) байт. 
 * При размещении памяти для копии уже считанной записи директории можно определять объем требуемой памяти как sizeof(struct dirent)+strlen(d.d_name).

 * pathconf

 * нужно экранировать аргумент, иначе shell сам его преобразует (./a.out "*")*/

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

int prov(char *s,char *t) {
	if (s[0]=='\0') {
		if (t[0]=='\0') 
			return 1;
		else 
			return 0;
	}
	switch (t[0]) {
		case '?': 
			return prov(&s[1],&t[1]);
		case '*':
			for (int i=1;i<=strlen(s);++i) {
				if (prov(&s[i],&t[1]))
					return 1;
			}
		default:
			if (s[0]==t[0]) {
				return prov(&s[1],&t[1]);
			} else {
				return 0;
			}
	}
}

int main (int argc,char *argv[]) {
	if (argc<2) {
		printf("I need arg\n");
		return 0;
	}
	DIR* d=opendir(".");
	struct dirent *f;
	int i=0;
	while (f=readdir(d)) {
		if (prov(f->d_name,argv[1])) {
			printf("%s\n",f->d_name);
		}
		++i;
		//printf("%s\n",f->d_name);
	}
	closedir(d);
	return 0;
}

