/* server.c */
/*
 * v0.1 �������Ϳͻ��˳��򣬷�������ָ֪ͨ���ͻ����ϴ�ָ����պ��cmd�㲥��ÿ���ͻ���
 * 		�����յ��ַ�'x'���л��ͻ���������
*/

/*˵��
step	server		  			client
  1		write to N 		--->	 read  		������ָ֪ͨ���ͻ�N��Ӧ  ���пͻ���read��
  2		read from N		<--- 	N write 	�ͻ���N��Ӧ���������ϴ��������ͻ��ȴ�����
  3  	write to All	--->	 read  		���пͻ�����������
    	... ...
    	
  		�ظ�1 2 3,���N�ϴ���������Ϊ'x'������������������л�֪ͨN+1��Ӧ
		���step2�пͻ�N�Ͽ����ӣ���ʹ��del_clientɾ����sockfd
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include "wrap.h"

//toupper
#include <ctype.h>

//inet_ntop
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "rich_server.h"
#include <signal.h>
#include "rich_client.h"

#define MAXLINE 80
#define SERV_PORT 8000

int maxi, maxfd, listenfd;
int client[FD_SETSIZE];
fd_set allset;
int g_currentIndex;
int g_index;
int player_num;

#if 1
int main(int argc, char **argv)
{	
	g_currentIndex=0;
	init_server();
	TELL_PARENT(getppid());
	wait_client(*argv[1]);//*argv[1]��������
	printf("You can begin the game!\n");
	server_run();
	return 0;
}
#endif


/*����write���ɹ�ʱ������sigpipe�ź�
 *write���ɹ������ڶԷ����ӹر������ 
*/
static void
write_err(int signo)
{
    printf("a player quit\n");
	client[g_index]=-1;
	player_num--;
	return;
}


int init_server(void)
{	
	struct sockaddr_in servaddr;
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);


	Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	Listen(listenfd, 20);

	maxfd = listenfd;		/* initialize */
	maxi = -1;			/* index into client[] array */
	int i;
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;	/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	if(signal(SIGPIPE, write_err) == SIG_ERR)
	{
		printf("server signal action init error\n");
		exit(0);
	}
	return 0;

}

int wait_client(int num)
{
	int client_num,i,nready,connfd;
	fd_set rset;
	socklen_t cliaddr_len;
	struct sockaddr_in	cliaddr;
	char str[INET_ADDRSTRLEN];
	for(client_num=0;client_num < num;)//�ȴ�����2���ͻ�������
	{
		rset = allset;	/* structure assignment */
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if (nready < 0)
			perr_exit("select error");

		if (FD_ISSET(listenfd, &rset)) { /* new client connection */
			cliaddr_len = sizeof(cliaddr);
			connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
			printf("A player join the game:\n");
			printf("received from %s at PORT %d\n",
			       inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
			       ntohs(cliaddr.sin_port));

			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					client[i] = connfd; /* save descriptor */
					break;
				}
				
			if (i == FD_SETSIZE) {
				fputs("too many clients\n", stderr);
				exit(1);
			}
			char send[2];
			send[0]=i;		//client id
			send[1]=num;	//player num
			Write(connfd,send,2);
			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd; /* for select */
			if (i > maxi)
				maxi = i;	/* max index in client[] array */
			
			client_num++;

		}	

	}
	return 0;

}


/*
 * �ͻ��˺ͷ������������Ӻ����пͻ��˵ȴ�������֪ͨ
 * ������ֻ��ǰ������Ŀͻ��˽�������
 */
int server_run(void)
{
	ssize_t n;
	char send_buf[MAXLINE];
	char recv_buf[MAXLINE];
	int sockfd;
	player_num=maxi+1;/*maxi=-1 ��ʾ������*/
	g_currentIndex=0;
	while(player_num > 0)	
	{
	
		//printf("current index=%d\n",g_currentIndex);
		sockfd = client[g_currentIndex];//ֻ�Ե�ǰid�����Ӵ���

		if(sockfd == -1) 
		{
			if(++g_currentIndex > maxi) 
				g_currentIndex=0;
			continue;
			//return -1;		//something wrong
		}
					
		send_buf[0]=g_currentIndex;
		
		for(g_index=0; g_index <= maxi; g_index++)		//ָ֪ͨ���ͻ��˷�������
			if(client[g_index]!=-1)
				Writen(client[g_index], send_buf, 1);	
		
		if ( (n = Read(sockfd, recv_buf, MAXLINE)) == 0) {
			/* connection closed by client */
			Close(sockfd);
			FD_CLR(sockfd, &allset);
			client[g_currentIndex]=-1;
			player_num--;
			//del_client(g_currentIndex);
		} 
		else 
		{
			ssize_t j;
			for (j = 0; j < n; j++)
				send_buf[j] =recv_buf[j];
			for(g_index=0; g_index <= maxi; g_index++)
			{
				if(client[g_index]!=-1)
					Writen(client[g_index], send_buf, n);	
				usleep(1000);	//������ֻ��һ������ ������ʱ�ᶪʧ����
			}
		
			if(recv_buf[0]!='x')	continue;//����ͻ��˲�����x�������
		}
		
		if(++g_currentIndex > maxi) g_currentIndex=0;	//��һ���ͻ�	

	}
	return 0;
}

#if 0
int notice_client(int fd)
{
	char c;
	Write(fd,&c, 1);
}

int recv_client(int fd)
{
	ssize_t n = Read(sockfd, buf, MAXLINE);
	if(n==0)
	{
		/* connection closed by client */
		Close(sockfd);
		FD_CLR(sockfd, &allset);
		del_client(g_currentIndex);

	}

}
#endif
/*��client�Ͽ����Ӻ�ɾ���ÿͻ�*/
void del_client(int index)
{
	int i;
	for(i = index; i < maxi; i++)
		client[i]=client[i+1];
	client[maxi]=-1;
	maxi--;
}

