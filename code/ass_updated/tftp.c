#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h> 
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/time.h>
#include<errno.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<signal.h>

const char* MODE="octet";

#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERR 5

void error(char *);
void upload(struct hostent *, char *);
void download(struct hostent *, char *);


struct sockaddr_in serv_addr;
int sockfd, addr_len;


int main(int argc , char **argv)
{
	char ops[200] , filename[200];
	int portno;
	struct hostent *server;

	if(argc<2)
	{
		error("\nCommand Line arguements are insufficient!!\n");
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd<0)
		error("\nError opening socket\n");
	
	server = gethostbyname(argv[1]);
	if(server == NULL)
		fprintf(stderr,"\nNo such host\n");
	
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(69);
	addr_len = sizeof(serv_addr);

	while(1)
	{
		printf("\n*****************TFTP-CLIENT*****************\n");
		printf("\n---MENU---\n");
		printf("\n(*)bye----(*)wr----(*)rd\n");
		printf("\nYOUR OPTION: ");
		scanf("%s",&ops);
		if(strcmp(ops,"bye")==0)
		{
			break;
		}
		
		if(strcmp(ops,"wr")==0)
		{
			printf("\nFilename to upload: ");
			scanf("%s",&filename);
			upload(server, filename);
		}

		if(strcmp(ops,"rd")==0)
		{
			printf("\nFilename to download: ");
			scanf("%s",&filename);
			download(server, filename);
		}

	}
}

void error(char *s)
{
    perror(s);
    exit(1);
}




void upload(struct hostent *hostname ,char *filename)
{
	//At first WRQ request to ftp server
	/*

          2 bytes    string   1 byte     string   1 byte
          -----------------------------------------------
   RRQ/  | 01/02 |  Filename  |   0  |    Mode    |   0  |
   WRQ    -----------------------------------------------
	
	*/
	FILE *fp;
	fp = fopen(filename, "r");
	int len = strlen(filename);
	if(fp==NULL)
	{
		error("\nError in opening file\n");
	}

	char msg[516], data[516];
	bzero(msg, 516);
	msg[0]=0x0; //WRQ opcode
	msg[1]=WRQ; //WRQ opcode

	strcpy(msg+2, filename);
	msg[2+len+1] = 0x0; // 0 byte
	strcpy(msg+2+len+1, MODE); //mode field contains the string "netascii", "octet", or "mail" 
	msg[2+len+strlen(MODE)+1] = 0x0; // 0 byte
	int req_len=2+len+1+strlen(MODE)+1; 

	if (sendto(sockfd, msg, req_len , 0 , (struct sockaddr *) &serv_addr, addr_len)==-1)
    {
            error("\nError in sending!!\n");
    }


    printf("\nSuccessfully sent WRQ\n");


    int n,i,ack;
    int bno=0;
    struct sockaddr_in r_addr;
    int alen= sizeof(r_addr);

    /*
				       2 bytes    2 bytes       n bytes
          			   ---------------------------------
   				DATA  | 03    |   Block #  |    Data    |
          		       ---------------------------------
				
				 		 2 bytes    2 bytes
          				 --------------------
   				  ACK   | 04    |   Block #  |
          				 --------------------

          		 2 bytes  2 bytes        string    1 byte
          		 ----------------------------------------
   		  ERROR | 05    |  ErrorCode |   ErrMsg   |   0  |
                 ----------------------------------------

    */
    
    while(1)
    {
    	for(i=0;i<5;i++)
    	{
    		bzero(data,516);
		    if (recvfrom(sockfd, data, 516, 0, (struct sockaddr *)&r_addr, &alen) == -1)
		    {
		        error("\nError in recieving!!\n");
		    }

		    ack = (data[2]<<8) + data[3];

		    if((data[1]==ERR) || (ack == bno-1))
		    {
		    	printf("\nError sending blocknum %d...\n", bno);
		    	if (sendto(sockfd, msg, 516 , 0 , (struct sockaddr *)&serv_addr, addr_len)==-1)
			    {
			            error("\nError in sending!!\n");
			    }
		    }
		    else
		    	break;
    	}

    	if(i>=5)
    	{
    			printf("\nCould not send File, Unsuccessful!!\n" );
    			return ;
    	}
    	
    	printf("\nWrite Connection established\n");

	    printf("\nACK received for block number %d.\n", bno);
	    
	    bno++;
    	bzero(msg, 516);
    	
    	msg[1]=DATA;
    	msg[2]=bno>>8;
		msg[3]=bno%(0xff+1);

		//char newMsg[512];
		//fscanf(fp,"%s",newMsg);
		//strncpy(msg + 4, newMsg, 512);
    	n = fread(msg+4, 1, 512, fp);
    	printf("Sending block %d of %d bytes.\n", bno,n);
    	if (sendto(sockfd, msg, n+4 , 0 , (struct sockaddr *) &r_addr, alen)==-1)
	    {
	            error("\nError in sending!!\n");
	    }
	    

    	
    	if(n<512)
    		break;
    }
    fclose(fp);
    printf("\nFile Uploaded!!.\n");
}

void download(struct hostent *hostname ,char *filename)
{
	FILE *fp;
	fp = fopen(filename, "w");
	int len = strlen(filename);

	// RRQ to port 69
	char msg[516], data[516];
	bzero(msg, 516);
	msg[0]=0x0; //RRQ opcode
	msg[1]=RRQ; //RRQ opcode
	strcpy(msg+2, filename);
	msg[2+len+1] = 0x0; // 0 byte
	strcpy(msg+2+len+1, MODE); //mode field contains the string "netascii", "octet", or "mail" 
	msg[2+len+strlen(MODE)+1] = 0x0; // 0 byte
	int req_len=2+len+1+strlen(MODE)+1; 


	/*
				       2 bytes    2 bytes       n bytes
          			   ---------------------------------
   				DATA  | 03    |   Block #  |    Data    |
          		       ---------------------------------
				
				 		 2 bytes    2 bytes
          				 --------------------
   				  ACK   | 04    |   Block #  |
          				 --------------------

          		 2 bytes  2 bytes        string    1 byte
          		 ----------------------------------------
   		  ERROR | 05    |  ErrorCode |   ErrMsg   |   0  |
                 ----------------------------------------

    */


	if (sendto(sockfd, msg, 516 , 0 , (struct sockaddr *) &serv_addr, addr_len)==-1)
    {
            error("\nError in sending!!\n");
    }


    int n,bno=1;
    struct sockaddr_in r_addr ;
    int alen= sizeof(r_addr);

    while(1)
    {

    	alen= sizeof(r_addr);
    	bzero(data,516);
    	n = recvfrom(sockfd, data, 516, 0, (struct sockaddr *)&r_addr, &alen);
    	
    	if (n == -1)
	    {
	        error("\nError in recieving!!\n");
	    }
	    n -= 4;
	    if(data[1]==ERR)
	    	error("\nServer transfer failure\n");
	     

	    fwrite(&data[4],1,n,fp);
	    printf("Received block of size n = %d\n", n);

	    bzero(msg,516);
		msg[0]=0x0;
		msg[1]=ACK;
		msg[2]=bno>>8;
		msg[3]=bno%(0xff+1);

		if (sendto(sockfd, msg, 4 , 0 , (struct sockaddr *) &r_addr, alen)==-1)
	    {
	            error("\nError in sending!!\n");
	    }    	
    	printf("Sent ACK for block %d.\n", bno);
		bno++;
	    if(n<512)
	    {
	    	break;
	    }

    }

    fclose(fp);
    printf("File Dowloaded!!\n");

}