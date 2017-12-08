#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void die(const char *errstr, ...);
void doservice(char *, int);
void runservice(char *, int);
void spawn(char *const []);
void usage(char *);

void main(int argc, char *argv[])
{
	if(argc<2)
		usage(argv[0]);
	if(!strcmp(argv[1],"-a")){
		
		if(argc<3){
			execlp("ls", "ls", "/etc/svc/run/", (char*)NULL);
			die("cannot list /etc/svc/run/\n");
		}
		else {
			FILE *fd;
			char *svcpath=(char*)malloc(sizeof(argv[2])+sizeof("/etc/svc/avail/"));
			strcpy(svcpath,"/etc/svc/avail/");
			strcat(svcpath,argv[2]);
			if ( (fd=fopen(svcpath,"r"))){
				fclose(fd);
				char **cmd=(char**)malloc(5*sizeof(char*));
				cmd[0]=(char*)malloc(sizeof("ln"));
				strcpy(cmd[0],"ln");
				cmd[1]=(char*)malloc(sizeof("-sf"));
				strcpy(cmd[1],"-sf");
				cmd[2]=(char*)malloc(sizeof(svcpath));
				strcpy(cmd[2],svcpath);
				cmd[3]=(char*)malloc(sizeof(argv[2])+sizeof("/etc/svc/run/"));
				strcpy(cmd[3],"/etc/svc/run/");
				strcat(cmd[3],argv[2]);
				cmd[4]=NULL;
				spawn(cmd);
				for(int i=3;i>=0;i-=1)
					free(cmd[i]);
				printf("%s activated\n",argv[2]);
				free(cmd);
			}
			free(svcpath);
			exit(0);
		}
	}
	else if(!strcmp(argv[1],"-c")){
		system("mkdir /etc/svc/avail");
		system("mkdir /etc/svc/run");
		system("mkdir /run/svc");
	}
	else if(!strcmp(argv[1],"-d")){
		if(argc<3)
			die("No service given\n");
		char *svcpath=(char*)malloc(sizeof(argv[2])+sizeof("/etc/svc/run/"));
		strcpy(svcpath,"/etc/svc/run/");
		strcat(svcpath,argv[2]);
		FILE *fd=fopen(svcpath,"r");
		if(fd){
			fclose(fd);
			char **cmd=(char**)malloc(4*sizeof(char*));
			cmd[0]=(char*)malloc(sizeof("rm"));
			strcpy(cmd[0],"rm");
			cmd[1]=(char*)malloc(sizeof("-f"));
			strcpy(cmd[1],"-f");
			cmd[2]=(char*)malloc(sizeof(svcpath));
			strcpy(cmd[2],svcpath);
			cmd[3]=NULL;
			spawn(cmd);
			for(int i=2;i>=0;i-=1)
				free(cmd[i]);
			free(cmd);
			free(svcpath);
			die("%s deactivated\n",argv[2]);
		}
		else die("Service is not marked as to run.\n");
	}
	else if(!strcmp(argv[1],"-k")){
		if(argc<3)
			doservice(0,0);
		else
			doservice(argv[2],0);
	}
	else if(!strcmp(argv[1],"-s")){
		if(argc<3)
			doservice(0,1);
		else
			doservice(argv[2],1);
	}
	else if(!strcmp(argv[1],"-l")){
		system("ls /etc/svc/avail");
	}
	else if(!strcmp(argv[1],"-r")){
		if(argc<3)
			die("no service given\n");
		char *cmd=(char*)malloc(sizeof("svc -k ")+sizeof(argv[2]));
		strcpy(cmd,"svc -k ");
		strcat(cmd,argv[2]);
		system(cmd);
		cmd[5]='s';
		system(cmd);
		free(cmd);
		die("\n");
	}
	else usage(argv[0]);
}

void
die(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	}

	exit(1);
}

void
doservice(char *name, int mode)//add order of loadings
{
	if(!name){//запустить все службы
		DIR *run=opendir("/etc/svc/run/");
		if(!run){
			die("cannot open /etc/svc/run\n");
		}
		struct dirent *obj;
		char **list=(char**)malloc(sizeof(char*));//list of services
		int number=0;//number of services in /etc/svc/run
		readdir(run);readdir(run);//skip . and ..
		while((obj=readdir(run))){
			number+=1;
			list=(char**)realloc(list,number*sizeof(char*));
			list[number-1]=(char*)malloc(sizeof(obj->d_name));
			strcpy(list[number-1],obj->d_name);
		}
		closedir(run);
		for(int i=0;i<number;i+=1)
			runservice(list[i],mode);
	}
	else{//запустить конкретную
		runservice(name,mode);
		exit(0);
	}
}

void
runservice(char *name, int mode)
{
	char **cmd=(char **)malloc(3*sizeof(char*));
	cmd[2]=NULL;
	cmd[1]=(char*)malloc(sizeof("-s"));
	char *action;
	if(mode==1)
		{
			strcpy(cmd[1],"-s");
			action="started";
		}
	else if(!mode)
		{
			strcpy(cmd[1],"-k");
			action="killed";
		}
	else if(mode==2)
		{
			strcpy(cmd[1],"-r");
			action="restarted";
		}
	FILE *serv;
	char *path[]={"/etc/svc/run/","/etc/svc/avail/","./","/bin/","/usr/bin/","/usr/local/bin","/sbin/","/usr/sbin/","/usr/local/sbin/"};
	for(int i=0;i<9;i+=1){
		cmd[0]=(char*)malloc(sizeof(path[i])+sizeof(name));
		strcpy(cmd[0],path[i]);
		strcat(cmd[0],name);
		serv=fopen(cmd[0],"r");
		if(serv){
			fclose(serv);
			spawn(cmd);
			for(int i=2;i>=0;i-=1)
				free(cmd[i]);
			free(cmd);
			printf("%s service: %s\n",action, name);
			return;
		}
		free(cmd[0]);
	}
	printf("cannot find service: %s\n",name);
}

void
spawn(char *const argv[])
{
	if (fork() == 0) {
		setsid();
		execvp(argv[0], argv);
		perror(" failed\n");
	}
}

void
usage(char *name)
{
	printf("usage: %s [-adklrsh] [service]\n\n",name);
	printf("-h - show this help\n");
	printf("-a - list all links in /etc/svc/run (all activated services)\n");
	printf("-c - prepare svc directories\n");
	printf("-a [ser] - activate service »ser« to be run on startup\n");
	printf("-d [ser] - deactivate service »ser«\n");
	printf("-k - kill all services\n");
	printf("-k [ser] - kill the service »ser«\n");
	printf("-l - list all services in /etc/svc/avail\n");
	printf("-r [ser] - restart service »ser«\n");
	printf("-s - run all services\n");
	printf("-s [ser] - run service »ser«\n");
}
