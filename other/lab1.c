#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <ulimit.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>

extern char **environ;

void ifi () {
	printf("Real User ID=%d\n",getuid());
	printf("Effective User ID=%d\n",geteuid());
	printf("Real Group ID=%d\n",getgid());
	printf("Effective Group ID=%d\n",getegid());		
}

void ifs () {
	setpgid(0,0)==EOF;
	/* int setpgid(pid_t pid, pid_t pgid);
	 * If pid is equal to 0,
	 * the process ID of the calling process is used. 
	 * If pgid is equal to 0, 
	 * the process specified by pid becomes a process group leader.
	 * Upon successful completion, 
	 * 0 is returned.
	 * Otherwise, -1 is returned and errno is set to indicate the error
	*/
}

void ifp () {
	printf("pid=%d\n",getpid());
	printf("ppid=%d\n",getppid());
	printf("pgid=%d\n",getpgid(0)); 
	/* возвращает идентификатор группы для процесса, идентификатор которого равен 
	 * pid, или для вызывающего процесса, если pid равен 0*/
}

void getLimit (int RLIMIT_TYPE,char *name) {
	//printf("ulimit=%ld*512 bytes=",ulimit(1));
	struct rlimit rl;
	/* struct rlimit{
	*	 rlim_t rlim_cur;  // Soft limit 
   	*	 rlim_t rlim_max;  // Hard limit (ceiling for rlim_cur) 	
	*  };
	* */ 
	struct rlimit rl1;
	getrlimit(RLIMIT_TYPE/*FSIZE*/,&rl1);
	if (rl1.rlim_cur!=RLIM_INFINITY) 
		printf("%s %ld\n",name,rl1.rlim_cur);
	else 
		printf("%s unlimited\n",name);
}

void ifu () {
	getLimit(RLIMIT_CORE,		"core file size           (blocks,c)  ");
	getLimit(RLIMIT_DATA,		"data seg size            (bytes, d)  ");
	getLimit(RLIMIT_NICE,		"scheduling priority             (e)  ");
	getLimit(RLIMIT_FSIZE,		"file size                (blocks,f)  ");
	getLimit(RLIMIT_SIGPENDING,	"pending signals                 (i)  ");
	getLimit(RLIMIT_MEMLOCK,	"max locked memory         (bytes,l)  ");
	getLimit(RLIMIT_RSS,		"max memory size           (bytes,m)  ");
	getLimit(RLIMIT_NOFILE,		"open files                      (n)  ");					
	printf(						"pipe size:                (bytes,p)   %d\n",PIPE_BUF);
	getLimit(RLIMIT_MSGQUEUE,	"POSIX message queues      (bytes,q)  ");	
	getLimit(RLIMIT_RTPRIO,		"real-time priority              (r)  ");
	getLimit(RLIMIT_STACK,		"stack size                (bytes,s)  ");	
	getLimit(RLIMIT_CPU,		"cpu time                 (second,t)  ");
	getLimit(RLIMIT_NPROC,		"max users processes             (u)  ");
	getLimit(RLIMIT_AS,			"virtual memory            (bytes,v)  ");
	getLimit(RLIMIT_LOCKS,		"file locks                      (x)  ");
}

void ifc () {
	struct rlimit rl;
	getrlimit(RLIMIT_CORE,&rl);
	printf("core: %ld\n",rl.rlim_cur);
}

void set (char c,char *optarg,int itIsC) {
	int type;
	if (!optarg) {
		printf("-%c must has argument\n",c);
		return;	
	}
	if (optarg[0]=='\0') {
		printf("-%c must has argument\n",c);
		return;
	}
	if (itIsC) {
		type=RLIMIT_CORE;
	} else {
		if (optarg[1]=='\0' || optarg[2]=='\0')	{
			printf("Uncorrected argument\n");
		}		
		switch (optarg[0]) {
			case 'c': 
				type=RLIMIT_CORE;
				break;
			case 'd':
				type=RLIMIT_DATA;
				break;
			case 'e': 
				type=RLIMIT_NICE;
				break;
			case 'f':
				type=RLIMIT_FSIZE;
				break;
			case 'i': 
				type=RLIMIT_SIGPENDING;
				break;
			case 'l':
				type=RLIMIT_MEMLOCK;
				break;
			case 'm': 
				type=RLIMIT_RSS;
				break;
			case 'n':
				type=RLIMIT_NOFILE;
				break;	
			case 'q':
				type=RLIMIT_MSGQUEUE;
				break;
			case 'r': 
				type=RLIMIT_RTPRIO;
				break;
			case 's':
				type=RLIMIT_STACK;
				break;
			case 't': 
				type=RLIMIT_CPU;
				break;
			case 'u':
				type=RLIMIT_NPROC;
				break;	
			case 'v': 
				type=RLIMIT_AS;
				break;
			case 'x':
				type=RLIMIT_LOCKS;
				break;	
			default: 
				printf("unknown key for U\n");
				return;
		}
		optarg+=2;
	}
	long long a=atol(optarg);
	if (!a && optarg[0]!='0' && optarg[1]!='\0'){
		printf("argument of %c must be number>=0\n", c);
		return;
	} 	
	struct rlimit rl;	
	getrlimit(type,&rl);
	rl.rlim_cur=a;
	setrlimit(type,&rl);
}

void help () {
	printf("I have keys: -i, -s, -p, -u, -U, -c, -C, -d, -c, -V\n");	
	printf("-i            print real and effective ids of user and group\n");	
	printf("-s            process becomes a group leader\n");	
	printf("-p            print process id, parent process id and process group id\n");	
	printf("-u            print value of ulimits\n");	
	printf("-UZ=new_ulimit  change value of ulimit Z\n");	
	printf("  Z can be: c core file size             \n");
	printf("            d data seg size              \n");
	printf("            e scheduling priority        \n");
	printf("            f file size                  \n");
	printf("            i pending signals            \n");
	printf("            l max locked memory          \n");
	printf("            m max memory size            \n");
	printf("            n open files                 \n");					
	printf("            p pipe size                  \n");
	printf("            q POSIX message queues       \n");	
	printf("            r real-time priority         \n");
	printf("            s stack size                 \n");	
	printf("            t cpu time                   \n");
	printf("            u max users processes        \n");
	printf("            v virtual memory             \n");
	printf("            x file locks                 \n");
	printf("-c            print the size of a core file that can be created, in bytes\n");	
	printf("-C            size  change size of a core file\n");	
	printf("-d            print current working directory\n");	
	printf("-v            print all environment variables and their values\n");	
	printf("-Vname=value  create a new environment variable or change the name of an existing variable\n");	
}

int main(int argc, char *argv[])
{
	char options[ ] = "hispuU::cC::dvV::"; /* valid options */
	int c, invalid = 0;
	char path[255];
	opterr=0; //предотвращаем запись ошибок
	if (argc==1) {
		help();
		return 0;
	}
	int ind=argc-1;
	while (ind>0) {
	optind=ind;
	c = getopt(argc, argv, options);
	switch (c) {
		case 'h':
			help();
			break;
		case 'i':
			ifi();
			break;
		case 's':
			ifs();			
			break;
		case 'p':
			ifp();
			break;
		case 'u':
			ifu();
			break;
		case 'U':
			set(c,optarg,0);
			break;
		case 'c':
			ifc();
			break;
		case 'C':
			set(c,optarg,1);
			break;
		case 'd':
			printf("PATH to D:%s\n",getcwd(path,255));
			break;
		case 'v':
			printf("\nenvironment variables:\n");
			for (char **p = environ; *p != 0; p++)
			printf(" %s\n", *p);
			break;
		case 'V':
			putenv(optarg);
			break;
		case '?':
			printf("I don't know %c. I have keys: -i, -s, -p, -u, -U, -c, -C, -d, -c, -V\n",optopt);
			break;
	}
	for (--ind;ind>0 && argv[ind][0]!='-';--ind);
	}
}
