/* client.c */
/*
 * v0.1  �ͻ������ȶԷ��������͵����ݽ����жϣ��Ƿ�Ϊ֪ͨ�ϴ�cmd���������ȡ���벢�ϴ���
 *		 ����ȴ��������·���ָ��cmd��
 * v0.2  �޸�download_cmd();�������β���'\0',ʹ���ⲿ�ռ�洢���޸�client_run�ķ���ֵ
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>


//inet_ntop
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "rich_client.h"

#define SERV_PORT 8000
int clientIndex;
int currentIndex;





#if 0
int main(int argc, char *argv[])
{
	client_run(init_client());
}
#endif

int client_run(int sockfd,char *des_buf)
{

	if(-1==sockfd) return -1;
	int IsYourturn=0;
	if(is_notice(sockfd)) //������֪ͨ 
	{
		upload_cmd(sockfd);
		IsYourturn=1;
	}
	download_cmd(sockfd,des_buf);

	return IsYourturn;

}


int init_client(int *PlayerNum)
{
	struct sockaddr_in servaddr;
	int sockfd;
    
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);
    
	Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	
	char data_rev[2];
	/*֮ǰ��������ȫ�ֵ�recv_buf[]������֮����recv_buf[]��һ�ν��յ�����������һ��*/
	/*������char *data_rev�����յ����ֽڣ�����ָ��û�г�ʼ�������¶δ���*/
	
	int n = Read(sockfd, data_rev, 2);/*���շ�����������client[]������Index*/

	if(n<=0)
	{
		printf("init client fail\n");
		return -1;
	}
	clientIndex=data_rev[0];
	printf("my id is %d\n",clientIndex);
	*PlayerNum=data_rev[1];

	return sockfd;
}

/*������͵��������Լ�������һ�£����ʾ��������֪ͨ�Լ�*/
int is_notice(int sockfd)
{
	int n;
	char data_rev;
	n=Readn(sockfd,&data_rev, 1);
	if(n <= 0)
	{
		printf("wait server notice error:the other side has been closed or sth wrong.\n");
		Close(sockfd);
		exit(-1);
	}
	return data_rev == clientIndex;
}

/*��ȡ���� �ϴ�ָ������*/
int upload_cmd(int sockfd)
{
	char send_buf[MAXLINE];

	if(fgets(send_buf, MAXLINE, stdin) == NULL)	exit(-1);
	Writen(sockfd, send_buf, strlen(send_buf));
	return 0;
}

/*��������ָ��*/
int download_cmd(int sockfd,char *des)
{
	int n;
	char recv_buf[MAXLINE];
	n = Read(sockfd, recv_buf, MAXLINE);
	if(n <= 0)
	{
		printf("download cmd error:the other side has been closed or sth wrong.\n");
		Close(sockfd);
		exit(-1);
	}
	memcpy(des,recv_buf,sizeof(recv_buf));
	n=0;
	while(des[n]!='\n'&& n++ < MAXLINE-1);//��ʱ��ôд
		des[n]='\0';
	return 0;
}

#if 1
/* TELL_*������signalʵ�� */
#include <signal.h>
#include "error.h"
static volatile sig_atomic_t sigflag;
static sigset_t newmask, oldmask, zeromask;

static void
sig_usr(int signo)
{
    sigflag = 1;
}

void
TELL_WAIT(void)
{
    if(signal(SIGUSR1, sig_usr) == SIG_ERR)
        err_sys("signal(SIGUSR1) error");
    if(signal(SIGUSR2, sig_usr) == SIG_ERR)
        err_sys("signal(SIGUSR2) error");
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);

    if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        err_sys("SIG_BLOCK error");
}

void 
TELL_PARENT(pid_t pid)
{
    kill(pid, SIGUSR2);
}

void 
WAIT_PARENT(void)
{
    while(sigflag == 0)
        sigsuspend(&zeromask);
    sigflag = 0;

    if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");
}

void
TELL_CHILD(pid_t pid)
{
    kill(pid, SIGUSR1);
}

void 
WAIT_CHILD(void)
{
    while(sigflag == 0)
        sigsuspend(&zeromask);
    sigflag = 0;
    if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        err_sys("SIG_SETMASK error");
}

#endif

