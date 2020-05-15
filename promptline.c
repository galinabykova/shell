 #include <stdio.h>
 #include <unistd.h>
 #include <string.h>
 #include <wctype.h>
 #include <termios.h>
 #include "shell.h"
  
 int promptline(char *prompt, char *line) {
        struct termios termE;
        tcflag_t savflags;
        tcflag_t savflags1;
        tcgetattr(0, &termE);
        savflags = termE.c_lflag;
        savflags1 = termE.c_oflag;
        termE.c_lflag &= (unsigned int) (~(ECHO | ICANON));
        cc_t min=termE.c_cc[VMIN];
        termE.c_cc[VMIN]=1;

        //termE.c_oflag &= ~(TABDLY);
        //termE.c_oflag |= TAB0;

        tcsetattr(0, TCSANOW, &termE);
        termE.c_lflag = savflags;
        termE.c_oflag = savflags1;
        termE.c_cc[VMIN]=min;

       //setbuf(stdin,(char*)NULL);

        int n = 0;
        write(1, prompt, strlen(prompt));
        int c;
        int d = -1;
        int e = -1;
        while (1) {
                if (d != -1) {
                        c = d;
                        d = -1;
                }
                else if (e != -1) {
                        c = e;
                        e = -1;
                } else { 
                        c = getchar();
                }
               //printf("%d\n",c);
                if (c == '\t') {
                        continue;
                }
                if (c == 27) {
                        d = getchar();
                        if (d != '\n') e = getchar();
                        if (d == '[' && (e == 'A' || e == 'B' || e == 'C' || e == 'D')) {
                                d = -1;
                                e = -1;
                        } 
                        continue;
                }
                if (c == 127) {
                        if (n<=0) continue;
                        --n;
                        putchar(8);
                        putchar(' ');
                        putchar(8);
                        continue;
                }
                if (c == 4) {
                        *(line) = 'e';
                        *(line + 1) = 'x';
                        *(line + 2) = 'i';
                        *(line + 3) = 't';
                        *(line + 4) = '\n';
                        *(line + 5) = '\0';
                        tcsetattr(0, TCSAFLUSH, &termE);
                        return 5;
                }
                putchar(c);
                fflush(stdout);
                *(line + n) = (char)c;
    	        ++n;
                /*
                *  check to see if command line extends onto
                *  next line.  If so, append next line to command line
                */
                if (n >= 2 && *(line + n - 2) == '\\' && *(line + n - 1) == '\n') {
                         *(line + n) = ' ';
                        *(line + n - 1) = ' ';
                        *(line + n - 2) = ' ';
                        continue;
                }
                if (c == '\n') {
                        *(line + n)='\0';
                        tcsetattr(0, TCSAFLUSH, &termE);
                        return n;
                }
        }
}
