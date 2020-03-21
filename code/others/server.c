#include<stdio.h>
#include<sys/types.h> //needed for socket.h and in.h
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h> //atoi()
#include<string.h>
#include<unistd.h>
#include<netdb.h>

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int factorial(int n){
	if(n==0)
		return 1;
	return n * factorial(n-1);
}

int main(int argc, char *argv[])
{

	if(argc<2)
	{
		fprintf(stderr,"Port not provided. Program terminated\n");
		exit(1);
	}

	int sockfd, newsockfd, portno, n;
	char buffer[255];
	struct sockaddr_in serv_addr, cli_addr;
	struct hostnet *server;
	socklen_t clilen;

	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		error("Error opening socket");
	
	bzero((char *)&serv_addr, sizeof(serv_addr));//clear serv_addr
	portno = atoi(argv[1]);
	
		
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno); // host to network short
	
	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
		error("Binding Failed.");
	}
	
	
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	
	newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	
	if(newsockfd<0){
		error("Error on Accept");
	}
	
	while(1){
		bzero(buffer,256);
		n = read(newsockfd, buffer, 255);
		if(n = 0)
			error("Error on reading");
		printf("CLient: %s fatorial is %d\n", buffer, factorial(atoi(buffer)));
		bzero(buffer,256);
		
		fgets(buffer, 255, stdin);
		
		n = write(newsockfd, buffer, strlen(buffer));
		if(n < 0)
			error("Error on Writing");
		
		int i = strncmp("Bye", buffer, 3);
		
		if(i==0)
			break;	
	}
	close(newsockfd);
	close(sockfd);
	return 0;
}
