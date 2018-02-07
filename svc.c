#include <dirent.h>
#include "utils.h"

void doservice(char *, int);
int filter(const struct dirent *);
void runservice(char *, int);
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
			for(int c=2;c<argc;c+=1)
			{
				char *svcpath=smprintf("/etc/svc/avail/%s",argv[c]);
				printf("%s\n",svcpath);
				fd=fopen(svcpath,"r");
				if ( fd ){
					fclose(fd);
					char **cmd=(char**)malloc(5*sizeof(char*));
					cmd[0]=smprintf("ln");
					cmd[1]=smprintf("-sf");
					cmd[2]=smprintf("%s",svcpath);
					cmd[3]=smprintf("/etc/svc/run/%s",argv[c]);
					cmd[4]=NULL;
					spawn(cmd);
					for(int i=3;i>=0;i-=1)
						if(cmd[i]) free(cmd[i]);
					printf("%s activated\n",argv[c]);
					free(cmd);
				}
				free(svcpath);
			}
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
		char *svcpath=smprintf("/etc/svc/run/%s",argv[2]);
		FILE *fd=fopen(svcpath,"r");
		if(fd){
			fclose(fd);
			char **cmd=(char**)malloc(4*sizeof(char*));
			cmd[0]=smprintf("rm");
			cmd[1]=smprintf("-f");
			cmd[2]=smprintf("%s",svcpath);
			cmd[3]=NULL;
			spawn(cmd);
			for(int i=2;i>=0;i-=1)
				if(cmd[i]) free(cmd[i]);
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
			for(int i=2;i<argc;i+=1)
				doservice(argv[i],0);
		exit(0);
	}
	else if(!strcmp(argv[1],"-s")){
		if(argc<3)
			doservice(0,1);
		else
			for(int i=2;i<argc;i+=1)
				doservice(argv[i],1);
		exit(0);
	}
	else if(!strcmp(argv[1],"-l")){
		system("ls /etc/svc/avail");
	}
	else if(!strcmp(argv[1],"-r")){
		if(argc<3)
			die("no service given\n");
		char *cmd=smprintf("svc -k %s", argv[2]);
		system(cmd);
		cmd[5]='s';
		system(cmd);
		free(cmd);
		exit(0);
	}
	else usage(argv[0]);
}

void
doservice(char *name, int mode)//add order of loadings
{
	if(!name){//запустить все службы
		/*DIR *run=opendir("/etc/svc/run/");
		if(!run){
			die("cannot open /etc/svc/run\n");
		}
		struct dirent *obj;
		readdir(run);readdir(run);//skip . and ..
		while((obj=readdir(run))){
			runservice(obj->d_name,mode);
		}
		closedir(run);*/
		struct dirent **objects;
		int num=scandir("/etc/svc/run", &objects, filter, alphasort);
		if(mode)
			for(int i=0;i<num;i+=1)
				runservice(objects[i]->d_name,mode);
		else for(int i=num-1;i>=0;i-=1)
				runservice(objects[i]->d_name,mode);
	}
	else{//запустить конкретную
		runservice(name,mode);
	}
}

int filter(const struct dirent *test)
{
	if (!strcmp(test->d_name,".") || !strcmp(test->d_name,".."))
		return 0;
	return 1;
}

void
runservice(char *name, int mode)
{
	char **cmd=(char **)malloc(3*sizeof(char*));
	cmd[2]=NULL;
	if(mode==1)
		{
			cmd[1]=smprintf("start");
			FILE *serv;
			char *path[]={"/etc/svc/run/","/etc/svc/avail/","./","/bin/","/usr/bin/","/usr/local/bin","/sbin/","/usr/sbin/","/usr/local/sbin/"};
			for(int i=0;i<9;i+=1){
				cmd[0]=smprintf("%s%s",path[i],name);
				strcpy(cmd[0],path[i]);
				strcat(cmd[0],name);
				serv=fopen(cmd[0],"r");
				if(serv){
					fclose(serv);
					system(smprintf("ln -sf %s /run/svc/",cmd[0]));
					free(cmd[0]);
					cmd[0]=smprintf("/run/svc/%s",name);
					spawn(cmd);
					for(int i=2;i>=0;i-=1)
						if(cmd[i]) free(cmd[i]);
					free(cmd);
					printf("Started service: %s\n", name);
					return;
				}
				free(cmd[0]);
			}
		}
	else if(!mode)
		{
			cmd[0]=smprintf("/run/svc/%s",name);
			cmd[1]=smprintf("stop");
			spawn(cmd);
			sleep(0.1);
			system(smprintf("rm -f %s",cmd[0]));
			for(int i=2;i>=0;i-=1)
				if(cmd[i]) free(cmd[i]);
			free(cmd);
			printf("Killed service: %s\n", name);
			return;
		}
	for(int i=2;i>=0;i-=1)
		if(cmd[i]) free(cmd[i]);
	free(cmd);
	printf("cannot find service: %s\n",name);
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
	exit(0);
}
