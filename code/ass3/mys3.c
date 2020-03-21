/**
*argv[1] portno
*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<signal.h>
#include <pthread.h>

void *writeHandler(int *);
void error(const char *);
void *readHandler(void *);
int count;

int main(int argc, char *argv[])
{

	if(argc<2)
	{
		fprintf(stderr,"Port not provided. Program terminated\n");
		exit(1);
	}

	//variables for socket return, accept return
	int sockfd, newsockfd, portno, n, arr[20],i=0;
	int *cli_sd;
	
	/**
	struct sockaddr_in {
		short int sin_family; //int Address family, AF_INET
		unsigned short int sin_port; //Port number  
		struct in_addr sin_addr; //Internet address
		unsigned char sin_zero[8]; //Same size as struct sockaddr
	};
	*/
	struct sockaddr_in serv_addr, cli_addr;
	struct hostnet *server;
	socklen_t clilen;
	pthread_t serv_reader,serv_writer;

	/*
	int socket(int domain, int type, int protocol); But what are these arguments? They allow you to say what kind of socket you want (IPv4 or IPv6, stream or datagram, and TCP or UDP). It used to be 		people would hardcode these values, and you can absolutely still do that. ( domain is PF_INET or PF_INET6 , type is SOCK_STREAM or SOCK_DGRAM , and protocol can be set to 0 to
	choose the proper protocol for the given type . Or you can call getprotobyname() to look up the protocol you want, “tcp” or “udp”.)
	*/

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		error("\nError opening socket\n");
	
	bzero((char *)&serv_addr, sizeof(serv_addr));//clear serv_addr
	portno = atoi(argv[1]);
	
		
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno); // host to network short
	

	/*
	struct sockaddr {
		unsigned short sa_family; // address family, AF_xxx
		char sa_data[14]; // 14 bytes of protocol address
	};
	int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
	sockfd is the socket file descriptor returned by socket() . my_addr is a pointer to a struct sockaddr that contains information about your address, namely, port and IP address. addrlen is the
	length in bytes of that address.
	*/
	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
		error("\nBinding Failed.\n");
	}
	
	/*
	int listen(int sockfd, int backlog);
	sockfd is the usual socket file descriptor from the socket() system call. backlog is the number of connections allowed on the incoming queue.
	*/
	listen(sockfd,10);


	/*
	int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	sockfd is the listen() ing socket descriptor. Easy enough. addr will usually be a pointer to a local struct sockaddr_storage . This is where the information about the incoming connection will
	go (and with it you can determine which host is calling you from which port). addrlen is a local integer variable that should be set to sizeof(struct sockaddr_storage) before its address is passed 		to accept() . accept() will not put more than that many bytes into addr . If it puts fewer in, it'll change the value of addrlen to reflect that.
	*/
	pthread_create((pthread_t *)&serv_writer,NULL,writeHandler,arr);
	clilen = sizeof(cli_addr);	
	while(arr[i] = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)){
		
		cli_sd = (int *)malloc(sizeof(int));
		*cli_sd = arr[i];
		i++;
		count = i;

		if(pthread_create((pthread_t *)&serv_reader,NULL,readHandler,(void *)cli_sd) < 0){
			printf("\nERROR!!Thread is not created....\n");
		}
		else{
			printf("\n%s: %d joined the chat with socket fd: %d and index: %d\n",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port),arr[i-1],i);
		}
	}

	if(newsockfd<0){
		error("\nError on Accept\n");
	}

	close(newsockfd);
	close(sockfd);
	return 0;
}

void *readHandler(void* skt_des_rd){	
	//var for msg 
	char buffer[256];
	int cli_sd = *(int *)skt_des_rd;
	int n,i,temp;
	while(1)
	{
		/*receiving from client*/
		/*int recv(int sockfd, void *buf, int len, int flags);
		sockfd is the socket descriptor to read from, buf is the buffer to read the information into, len is
		the maximum length of the buffer, and flags can again be set to 0*/
		n = read(cli_sd, buffer, 255);
		/*terminating condition*/
		i = strcmp("Bye", buffer);	
		if(i==0)
			break;
		if(n < 0){
			error("\nError on reading\n");
		}
		else
		{		
			//printf("\nClient %d: %s\n", cli_sd-3, buffer);
			//printf("\n%s: %d socket fd: %d $ %s\n",inet_ntoa(skt_des_rd->cli_addr.sin_addr),ntohs(skt_des_rd->cli_addr.sin_port),cli_sd);
			printf("\nClient %d: \n", cli_sd-3);
			i = 0;
			while(buffer[i]!='\0')
			{
				putchar(buffer[i]);
				i++;
			}
			temp = buffer[0]-48;
			temp = temp+3;
			if(temp==cli_sd || (buffer[0]>57))
			{
				n = write(cli_sd, buffer, strlen(buffer));
			}
			else
			{
				/*for(i=1;i<strlen(buffer);i++)
					buffer[i-1] = buffer[i];*/
				n = write(temp, buffer, strlen(buffer));
			}
			if(n < 0){
				error("\nError on Writing\n");
			}
		}
		bzero(buffer,256);
	}
	printf("\nClient %d left chat\n", cli_sd-3);
	close(cli_sd);
	pthread_exit(0);
	//exit(0);
}
void *writeHandler(int *arr){
	//var for msg 
	char buffer[256],ch;
	bzero(buffer,256);
	int n,i;

	/*while(fgets(buffer,255,stdin) !=NULL){
		printf("client number:");
		scanf("%d",&n);
		n = n-1;
		/*sending to client*/
		/*int send(int sockfd, const void *msg, int len, int flags);
		sockfd is the socket descriptor you want to send data to (whether it's the one returned by socket() or the one you got with accept() .) msg is a pointer to the data you want to send, and 			len is the length of that data in bytes. Just set flags to 0.*/
	/*	if((n=write(arr[n],buffer,strlen(buffer))) < 0){
			error("Write error socket thread\n");
		}
		else{
			printf("%s\n","Data sent\n");
			bzero(buffer,256);
		}
	}*/
	
	while(1)
	{
		printf("\nSERVER:");
		bzero(buffer,256);
		i=0;
		while((ch = getchar())!=EOF)
		{
			buffer[i] = ch;
			i++;
		}
		//scanf("%s",&buffer);
		printf("\nRES:%s\n",buffer);
		i = strncmp("Bye", buffer, 3);		
		if(i==0)
			break;
		printf("\nclient number:");
		scanf("%d",&n);
		n = n-1;
		n=write(arr[n],buffer,strlen(buffer));
		if(n < 0){
			error("\nWrite error socket thread\n");
		}
		else{
			printf("\nData sent\n");
			bzero(buffer,256);
		}
	}
	//printf("\nEnter number of clients:");
	//scanf("%d",&n);
	for(i=0;i<count;i++)
	{	
		write(arr[i],buffer,strlen(buffer));
		close(arr[i]);
	}
	
	//pthread_exit(0);
	exit(0);
}
void error(const char *msg)
{
	perror(msg);
	exit(1);
}
	
