/**
*argv[1] server_ipaddress
*argv[2] portno
*/
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
	exit(0);
}

int factorial(int n){
	if(n==0)
		return 1;
	return n * factorial(n-1);
}


int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	char buffer[256];
	struct sockaddr_in serv_addr;
	struct hostent *server;

	if(argc<2)
	{
		fprintf(stderr,"Usage %s hostname port\n",argv[0]);
		exit(0);
	}
		
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0)
		error("Error opening socket");
	
	server = gethostbyname(argv[1]);
	if(server == NULL)
		fprintf(stderr,"No such host\n");
	
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
		error("Connection Failed");
	
	while(1){
		bzero(buffer, 255);
		fgets(buffer, 255, stdin);
		n = write(sockfd, buffer, strlen(buffer));
		
		if(n<0)
			error("Error on writing");
		
		bzero(buffer, 255);
		
		n = read(sockfd, buffer, 255);
		if(n < 0)
			error("Error on reading.");
			
		printf("Server: %s factorial is %d\n", buffer, factorial(atoi(buffer)));
		
		int i = strncmp("Bye", buffer, 3);
		if(i==0)
			break;
	}
	close(sockfd);
	return 0;
}
