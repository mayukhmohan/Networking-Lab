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

void error(const char *);

int main(int argc, char *argv[])
{

	if(argc<2)
	{
		fprintf(stderr,"Port not provided. Program terminated\n");
		exit(1);
	}

	int portno, i=0, sockfd, addr_len, n;
	char buffer[1307];
	
	struct sockaddr_in serv_addr,cli_addr;
	struct hostnet *server;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
		error("\nError opening socket\n");
	
	bzero((char *)&serv_addr, sizeof(serv_addr));
	bzero((char *)&cli_addr, sizeof(cli_addr));
	portno = atoi(argv[1]);
		
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno); 
	

	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
		error("\nBinding Failed.\n");
	}
	
		
	while(1){
		bzero(buffer,1307);
		if ((n = recvfrom(sockfd, buffer, 1307, 0, (struct sockaddr *)&cli_addr, &addr_len)) == -1) 
		{
			error("\nError on reading\n");
			exit(1);
		}
		//buffer = realloc(buffer, n * sizeof(char));
		buffer[6] -= 1;
		//printf("\n%s: %d :SEQ_NO -> %d%d,TTL -> %d\n",inet_ntoa(cli_addr.sin_addr),cli_addr.sin_port,buffer[0],buffer[1],buffer[6]);
		if ((n = sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *) &cli_addr, addr_len)) == -1) 
		{
			error("Error on writing");
			exit(1);
		}
	}
	close(sockfd);
	return 0;
}

void error(const char *msg)
{
	perror(msg);
	exit(1);
}