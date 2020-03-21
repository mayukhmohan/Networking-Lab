/**
*argv[1] server_ipaddress
*argv[2] portno
*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

void *writeHandler(void *);
void error(const char *);
void *readHandler(void *);

int main(int argc, char *argv[])
{
	if(argc<2)
	{
		fprintf(stderr,"Usage %s hostname port\n",argv[0]);
		exit(0);
	}

	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	pthread_t client_reader, client_writer;
	int *cli_skt_fd = (int *)malloc(sizeof(int));
		
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockfd<0)
		error("\nError opening socket\n");
	
	
	/*int gethostname(char *hostname, size_t size);
	The arguments are simple: hostname is a pointer to an array of chars that will contain the hostname upon the function's return, and size is the length in bytes of the hostname array.*/
	server = gethostbyname(argv[1]);
	
	if(server == NULL)
		fprintf(stderr,"\nNo such host\n");
	
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	
	/*int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
	sockfd is our friendly neighborhood socket file descriptor, as returned by the socket() call, serv_addr is a struct sockaddr containing the destination port and IP address, and addrlen is the  	 length in bytes of the server address structure.*/
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
		error("\nConnection Failed\n");
	

	*cli_skt_fd = sockfd;
	if(pthread_create((pthread_t *)&client_reader,NULL,readHandler,(void *)cli_skt_fd) < 0){
			printf("\nError Reader thread!!\n");
	}
	if(pthread_create((pthread_t *)&client_writer,NULL,writeHandler,(void *)cli_skt_fd) < 0){
			printf("\nError Writer thread!!\n");
	}
	else{
		printf("\nConnected to %s: %d\n",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port));
	}
	//while(1);
	pthread_join(client_writer, NULL);
	pthread_join(client_reader, NULL);
	//close(sockfd);
	return 0;
}

void error(const char *msg)
{
	perror(msg);
	exit(0);
}
void *readHandler(void *sockfd){
	int client_sock = *(int*)sockfd;
	int n,i;
	char buffer[256];
	while(1){
		bzero(buffer, 256);
		n = read(client_sock, buffer, 255);
		/*Terminating condition*/
		i = strncmp("Bye", buffer, 3);
		if(i==0)
			break;
		if(n<0)
		{
			error("\nError on reading.\n");
		}
		else
		{
			//printf("\nServer: %s\n", buffer);	
			printf("\nServer: \n");
			i = 0;
			if(buffer[i]<57)
			{
				printf("From %d:",buffer[i]-48);
				i++;
			}
			while(buffer[i]!='\0')
			{
				putchar(buffer[i]);
				i++;
			}
		}
	}
	close(client_sock);
	//pthread_exit(0);
	exit(0);
}
void *writeHandler(void *sockfd){
	int client_sock = *(int*)sockfd;
	char buffer[256],ch;	
	int n,i;
	bzero(buffer,256);
	//printf("CLIENT:");
	/*while(fgets(buffer,255,stdin) !=NULL){
		if((n=write(client_sock,buffer,strlen(buffer))) < 0){
			error("Write error socket thread\n");
		}
		else{
			printf("%s\n","Data sent\n");
			bzero(buffer,256);
		}
	}*/
	while(1)
	{
		printf("\nCLIENT:");
		bzero(buffer,256);
		//scanf("%s",&buffer);
		i=0;
		while((ch = getchar())!=EOF)
		{
			buffer[i] = ch;
			i++;
		}
		n=write(client_sock,buffer,strlen(buffer));
		i = strncmp("Bye", buffer, 3);		
		if(i==0)
			break;
		if(n < 0){
			error("\nWrite error socket thread\n");
		}
		else{
			printf("\nData sent\n");
			bzero(buffer,256);
		}
	}
	close(client_sock);
	//pthread_exit(0);
	exit(0);
}
