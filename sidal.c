#include "utils.h"

#define START 1<<0
#define STOP 1<<1
#define AVAIL 1<<2
#define UNAVAIL 1<<3
#define WAIT 1<<4
#define MARK 1<<5

static char* basename(char *name);
static int doservice(const char *path);
static char* findpath(char *name);
static int runservice(const char *target);
static void usage(char *name);

static short int mode=0;
static char *ecmd=NULL; /* overwritten cmd */

char*
basename(char *name)
{
	int i, l1, l2;
	char *out;
	l2=strlen(name)+1;
	for (l1=l2-1;l1>0 && name[l1-1]!='/';l1-=1);
	l2=l2-l1;
	out=(char*)malloc(l2*sizeof(char));
	for (i=0;i<l2;i+=1)
		out[i]=name[i+l1];
	return out;
}

int
doservice(const char *path)
{
	int ret=1;
	pid_t service;
	char **cmd=(char **)malloc(3*sizeof(char*));
	cmd[0]=path;
	cmd[2]=NULL;
	if (ecmd) {
		cmd[1]=ecmd;
	} else if ((mode & START) && (mode & STOP)) {
		cmd[1]="restart";
	} else if (mode & START) {
		cmd[1]="start";
	} else if (mode & STOP) {
		cmd[1]="stop";
	} else {
		cmd[1]=NULL;
	}
	service=spawn(cmd);
	free(cmd);
	if (mode & WAIT) {
		/* wstatus should be initialized to trace errors */
		int wstatus=-1;
		waitpid(service,&wstatus,0);
		ret=WIFEXITED(wstatus);
		if (ret) {
			if (WEXITSTATUS(wstatus) != 0) {
				ret=0;
			}
		}
	}
	return ret;
}

char*
findpath(char *name)
{
	char **paths;
	FILE *fp;
	if ((mode & START) && (mode & STOP)) {
		paths=(char**)malloc(3*sizeof(char*));
		paths[0]="/run/sidal/";
		paths[1]="/etc/sidal/run/";
		paths[2]=NULL;
	} else if (mode & STOP) {
		paths=(char**)malloc(3*sizeof(char*));
		paths[0]="/run/sidal/";
		paths[1]="/etc/sidal/run/";
		paths[2]=NULL;
	} else if (mode & START) {
		paths=(char**)malloc(11*sizeof(char*));
		paths[0]="/etc/sidal/run/";
		paths[1]="/etc/sidal/avail/";
		paths[2]="/bin/";
		paths[3]="/sbin/";
		paths[4]="/usr/bin/";
		paths[5]="/usr/sbin/";
		paths[6]="/usr/local/bin/";
		paths[7]="/usr/local/sbin/";
		paths[8]="";
		paths[9]="./";
		paths[10]=NULL;
	}
	else if (mode & AVAIL) {
		paths=(char**)malloc(2*sizeof(char*));
		paths[0]="/etc/sidal/avail/";
		paths[1]=NULL;
	}
	else if (mode & UNAVAIL) {
		paths=(char**)malloc(2*sizeof(char*));
		paths[0]="/etc/sidal/run/";
		paths[1]=NULL;
	}
	for (int i=0;paths[i];i+=1) {
		if ((fp=fopen(smprintf("%s%s",paths[i],name),"r"))) {
			const char *path=paths[i];
			free(paths);
			fclose(fp);
			return smprintf("%s%s",path,name);
		}
	}
	return 0;
}

int
runservice(const char *target)
{
	char *path=findpath(target);
	if (!path) {
		warn("cannot find service: %s",target);
		return -1;
	}
	if (mode&START | mode&STOP) {
		printf("%s service: %s\n",((mode & START) && (mode & STOP)) ? "restarting" : (mode & START) ? "starting" : "stopping",target);
		int res=doservice(path);
		if (mode&WAIT) {
			if (res) {
				printf("service %s successfully: %s\n",((mode & START) && (mode & STOP)) ? "restarted" : (mode & START) ? "started" : "stopped",target);
			} else {
				printf("errors occured while %s service: %s\n",((mode & START) && (mode & STOP)) ? "restarting" : (mode & START) ? "starting" : "stopping",target);
			}
		}
		if (mode&MARK && res) {
			if ((mode & STOP) && strstr(path,"/run/sidal/")) {
				system(smprintf("rm %s",path));
			}
			if (mode & START) {
				system(smprintf("ln -sf %s /run/sidal",path));
			}
		}
	} else if (mode & AVAIL) {
		system(smprintf("ln -sf %s /etc/sidal/run",path));
	} else if (mode & UNAVAIL) {
		system(smprintf("rm %s",path));
	}
	free(path);
	return 0;
}

void
usage(char *name)
{
	printf("usage: %s [acefklmrsuw] [cmd] services\n",name);
	exit(0);
}

int
main(int argc, char *argv[])
{
	if (argc<2)
		usage(argv[0]);
	if (strchr(argv[1],'c')) {
		system("mkdir /etc/sidal");
		system("mkdir /etc/sidal/avail");
		system("mkdir /etc/sidal/default");
		system("mkdir /etc/sidal/run");
		system("mkdir /run/sidal");
		exit(0);
	}
	if (strchr(argv[1],'l')) {
		system("ls /etc/sidal/run");
		exit(0);
	}
	if (strchr(argv[1],'a')) {
		mode=mode|AVAIL;
	} else if (strchr(argv[1],'u')) {
		mode=mode|UNAVAIL;
	} else if (strchr(argv[1],'s')) {
		mode=mode|START;
	} else if (strchr(argv[1],'k')) {
		mode=mode|STOP;
	} else if (strchr(argv[1],'r')) {
		mode=mode|START|STOP;
	}
	if (strchr(argv[1],'w')) {
		mode=mode|WAIT;
	}
	if (strchr(argv[1],'m')) {
		mode=mode|MARK;
	}
	if (strchr(argv[1],'e')){
			ecmd=argv[2];	
		}
	if (!(argc-2-(ecmd ? 1 : 0))) {
		if (mode&AVAIL) {
			system("ls /etc/sidal/avail");
		} else if (mode & START) {
			system("ls /run/sidal");
		} else {
			usage(argv[0]);
		}
	}
	if (!strchr(argv[1],'f')) {
		for (int i=0;i<argc-2- (ecmd ? 1 : 0);i+=1) {
			runservice(argv[2+(ecmd ? 1 : 0)+i]);
		}
	}
	else {
		FILE *ex;
		for (int i=0;i<argc-2- (ecmd ? 1 : 0);i+=1) {
			if ((ex=fopen(smprintf("/run/sidal/%s",basename(argv[2+(ecmd ? 1 : 0)+i])),"r")) && mode&START && !(mode&STOP)) {
				fclose(ex);
			} else if (!(ex=fopen(smprintf("/run/sidal/%s",basename(argv[2+(ecmd ? 1 : 0)+i])),"r")) && !(mode&START) && mode&STOP);
			else runservice(argv[2+(ecmd ? 1 : 0)+i]);
		}
	}
	return 0;
}
