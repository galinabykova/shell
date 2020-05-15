shell: shell.c parseline.c promptline.c jobs.c
	gcc -g shell.c parseline.c promptline.c jobs.c -o shell -std=c11 -Wall -Wextra -Wpedantic -Wunused -Wconversion -D_POSIX_C_SOURCE=200112L
