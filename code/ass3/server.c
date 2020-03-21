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

struct socketData {
	struct sockaddr_in cli_addr;
};

void *writeHandler(int *);
void error(const char *);
void *readHandler(int *);
int count = 0,sockfd;
struct socketData* sdArray;

int main(int argc, char *argv[])
{

	if(argc<2)
	{
		fprintf(stderr,"Port not provided. Program terminated\n");
		exit(1);
	}

	//variables for socket return, accept return
	int portno, n, arr[20],i=0;
	sdArray = (struct socketData*)malloc(20 * sizeof(struct socketData));
	
	struct sockaddr_in serv_addr;
	struct hostnet *server;
	pthread_t serv_reader,serv_writer;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		error("\nError opening socket\n");
	
	bzero((char *)&serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
		
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno); 
	

	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
		error("\nBinding Failed.\n");
	}
	
	listen(sockfd,10);

	pthread_create((pthread_t *)&serv_writer,NULL,writeHandler,arr);	
	
	pthread_create((pthread_t *)&serv_reader,NULL,readHandler,arr);
	pthread_join(serv_writer, NULL);
	pthread_cancel(serv_reader);

	/*while(arr[i] = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)){
		
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
	}*/

	close(sockfd);
	return 0;
}

/*void *readHandler(void* skt_des_rd){	
	char buffer[256];
	int cli_sd = *(int *)skt_des_rd;
	int n,i,temp;
	while(1)
	{
		n = read(cli_sd, buffer, 255);
		i = strcmp("Bye", buffer);	
		if(i==0)
			break;
		if(n < 0){
			error("\nError on reading\n");
		}
		else
		{		
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
}*/

void *readHandler(int *arr)
{
	fd_set readsd;
	char buffer[256];
	int max,temp,i,j,flag = 0,n;
	int cli_sd;
	socklen_t clilen;
	struct sockaddr_in cli_addr;

	while(1)
	{
		FD_ZERO(&readsd);
		FD_SET(sockfd, &readsd);
		max = sockfd;

		for(i = 0; i < count; i++) 
		{
			if(arr[i] == 0)
				continue;
			FD_SET(arr[i], &readsd);
			if(arr[i] > max)
				max = arr[i];
		}

		temp = select(max + 1, &readsd, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &readsd)) 
		{
			clilen = sizeof(cli_addr);
			temp = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
			if(temp < 0)
			{
				error("Error on Accept");
				continue;
			}
	
			struct socketData* p = (struct socketData*)malloc(sizeof(struct socketData));
			arr[count] = temp;
			p->cli_addr = cli_addr;
			sdArray[count] = *p;
			count = (count+1)%20;	

			printf("\n%s: %d joined the chat with socket fd: %d and index: %d\n",inet_ntoa(p->cli_addr.sin_addr),p->cli_addr.sin_port,arr[count-1],count);
		}
		for(i = 0; i < count; i++) 
		{
			if(arr[i] == 0)
				continue;
			if(FD_ISSET(arr[i], &readsd)) 
			{ 
				//readSocket(i + 1);
				char buffer[256];
			    cli_sd = arr[i];
				n = read(cli_sd, buffer, 255);
				j = strcmp("Bye", buffer);	
				

				if(j==0)
					flag = 1;	
				

				if(n < 0)
				{
					error("\nError on reading\n");
				}
				else
				{		
					//printf("\nClient %d: \n", cli_sd-3);
					printf("\n%s: %d sfd: %d index: %d\n",inet_ntoa((sdArray+i)->cli_addr.sin_addr),(sdArray+i)->cli_addr.sin_port,arr[i],i+1);
					j = 0;
					while(buffer[j]!='\0')
					{
						putchar(buffer[j]);
						j++;
					}
					temp = buffer[0]-48;
					temp = temp+3;
					if(temp==cli_sd || (buffer[0]>57))
					{
						n = write(cli_sd, buffer, strlen(buffer));
					}
					else
					{
						n = write(temp, buffer, strlen(buffer));
					}
					if(n < 0)
					{
						error("\nError on Writing\n");
					}
				}
				bzero(buffer,256);
				if(flag == 1)
				{
					//printf("\nClient %d left chat\n", cli_sd-3);
					printf("\n%s: %d sfd: %d index: %d left chat.\n",inet_ntoa((sdArray+i)->cli_addr.sin_addr),(sdArray+i)->cli_addr.sin_port,arr[i],i+1);
					arr[i] = 0;
				}
			}
		}
	}
}


void *writeHandler(int *arr){
	char buffer[256],ch;
	bzero(buffer,256);
	int n,i;
	
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

	for(i=0;i<count;i++)
	{	
		write(arr[i],buffer,strlen(buffer));
		close(arr[i]);
	}
	
	exit(0);
}
void error(const char *msg)
{
	perror(msg);
	exit(1);
}
	
