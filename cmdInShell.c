#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <sys/ddi.h> //for Solaris
#include <string.h>
#include <wait.h>
#include "shell.h"

int exceptions (int i) {
	if (!strcmp(cmds[i].cmdargs[0],"cd")) {
		chdir(cmds[i].cmdargs[1]);
		//если аргументов нет, куда переносит cd?
		return 1;
	}
	if (!strcmp(cmds[i].cmdargs[0],"exit")) {
		exit(0);
	}	
	//также нужно реализовать
	/*
	 * pushd and popd
     * logout, bye, etc
     * fg, bg, jobs, and the & suffix
     * history
     * set, unset, export
    */
	return 0;
}