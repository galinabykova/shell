#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>


int main () {
	char line[BUFSIZ];
	char s[100]="echo '";
	int cnt=6;
	for (char i='A';i<'z';++i) {
		s[cnt++]=i;
	}
	s[cnt++]='\'';
	s[cnt]='\0';
	FILE *f=popen(s,"r");
	FILE *f1=popen("tr 'a-z' 'A-Z'","w");

	while(fgets(line,BUFSIZ,f))
		fputs(line, f1);

	pclose(f);
	pclose(f1);
	return 0;
}