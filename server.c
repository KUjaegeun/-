#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<sys/time.h>
#include<errno.h>

typedef struct{
	int listenSd, connectSd;
	struct sockaddr_in srvAddr, clntAddr;
	int clntAddrLen, readLen, strLen;
	char rBuff[BUFSIZ];
	int maxFd;
	fd_set defaultFds, rFds;
	int res, i;
}allvalue;
void errProc(const char*);
void startServer(allvalue *,char **);
void binding(allvalue *);
void listening(allvalue *);
void reading(allvalue *);
void writing(allvalue *);

int main(int argc, char** argv)
{
	allvalue all;
	all.maxFd = 0;
	if(argc !=2)
	{
		printf("Usage : %s [Port Number]\n",argv[0]);
		return -1;
	}
	startServer(&all,argv);
	binding(&all);
	listening(&all);

	FD_ZERO(&all.defaultFds);
	FD_SET(all.listenSd, &all.defaultFds);
	all.maxFd = all.listenSd;
	
	all.clntAddrLen = sizeof(all.clntAddr);
	
	while(1)
	{
		all.rFds = all.defaultFds;
		printf("Monitoring ...\n");
		if((all.res = select(all.maxFd + 1, &all.rFds, 0,0,NULL)) == -1) break;
		for(all.i = 0; all.i<all.maxFd+1;all.i++)
		{
			if(FD_ISSET(all.i, &all.rFds))
			{
				if(all.i==all.listenSd)
				{
					all.connectSd = accept(all.listenSd, (struct sockaddr *)&all.clntAddr,&all.clntAddrLen);
					if(all.connectSd == -1)
					{
						fprintf(stderr,"Accept Error");
						continue;
					}
					fprintf(stderr,"A client is connected...\n");
					FD_SET(all.connectSd, &all.defaultFds);
					if(all.maxFd < all.connectSd)
					{
						all.maxFd = all.connectSd;
					}
				}
				else
				{
					all.readLen = read(all.i , all.rBuff, sizeof(all.rBuff)-1);
					if(all.readLen ==0)
					{
						fprintf(stderr,"A client is disconnected ... \n");
						FD_CLR(all.i , &all.defaultFds);
						close(all.i);
						continue;
					}
					all.rBuff[all.readLen] = '\0';
					printf("Client(%d): %s\n",all.i-3,all.rBuff);
					write(all.i,all.rBuff, strlen(all.rBuff));
				}
			}
		}
	}		
	close(all.listenSd);
	return 0;
}



void startServer(allvalue *all,char **argv)
{
	printf("Server start...\n");
	all->listenSd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(all->listenSd == -1) errProc("socket");

	memset(&all->srvAddr, 0, sizeof(all->srvAddr));
	all->srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	all->srvAddr.sin_family = AF_INET;
	all->srvAddr.sin_port = htons(atoi(argv[1]));
}
void binding(allvalue *all)
{
	if(bind(all->listenSd,(struct sockaddr *)&all->srvAddr,sizeof(all->srvAddr))==-1)
		errProc("bind\n");
}
void listening(allvalue *all)
{
	if(listen(all->listenSd,5)<0) errProc("listen");
}
void errProc(const char *str)
{
	fprintf(stderr, "%s: %s",str,strerror(errno));
	exit(10);
}
