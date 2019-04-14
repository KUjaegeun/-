#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<termios.h>
#include<unistd.h>
void errProc();
int linux_kbhit(void);
int linux_getch(void);
typedef struct{
	int x;
	int y;
}location;
int main(int argc, char** argv)
{
	location loc;
	int clntSd;
	struct sockaddr_in clntAddr;
	int clntAddrLen, readLen, recvByte, maxBuff;
	char wBuff[BUFSIZ];
	char rBuff[BUFSIZ];

	if(argc != 3) {
		printf("Usage: %s [IP Address] [Port]\n", argv[0]);
	}
	clntSd = socket(AF_INET, SOCK_STREAM,0);
	if(clntSd == -1 ) errProc();
	printf("=======client program=====\n");
	memset(&clntAddr, 0, sizeof(clntAddr));
	clntAddr.sin_family = AF_INET;
	clntAddr.sin_addr.s_addr = inet_addr(argv[1]);
	clntAddr.sin_port = htons(atoi(argv[2]));
	if(connect(clntSd, (struct sockaddr *) &clntAddr, sizeof(clntAddr))==-1)
	{
		close(clntSd);
		errProc();
	}
	while(1)
	{
		loc.x=4;
		readLen = strlen((location *)&loc);
		write(clntSd,(location *)&loc, readLen+1);
		recvByte = 0;
		maxBuff = BUFSIZ-1;
		do{
			recvByte += read(clntSd, (location *)&loc,4);
			maxBuff -= recvByte;
		}while(recvByte <(readLen-1));
		printf("server: %d \n",loc.x);
		loc.x++;
	}
	printf("END ^^\n");
	close(clntSd);
	return 0;
}
void errProc(){
	fprintf(stderr, "Error: %s\n",strerror(errno));
	exit(errno);
}
int linux_kbhit(void)
{
	struct termios oldt, newt;
	int ch;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return ch;
}
int linux_getch(void)

{

	int ch;
	struct termios buf, save;
	tcgetattr(0, &save);
	buf = save;
	buf.c_lflag &= ~(ICANON | ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	tcsetattr(0, TCSAFLUSH, &buf);
	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);
	return ch;

}
