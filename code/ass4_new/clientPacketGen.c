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
struct timeval tv;
uint32_t initiation;

void error(const char*);
uint32_t getTimeStamp(void);
void getInitialTime(void);
void fillBuffer(char*,int,int,int);


int main(int argc, char *argv[])
{
	int init,sockfd, portno, seq, P = 0, ttl, TTL, totalLength, numPkts, sTime, rTime,counter = 0;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int i;
	FILE *fp;

	if(argc<3)
	{
		error("\nCommand Line arguements are insufficient!!\n");
	}
		
	for(i=0;i<3;i++)
	{
		P = 0;
		counter = 0;
		

		while(counter<10)
		{
			if(i==0)
			{
				TTL = 2;
				fp = fopen("p_rtt2.csv", "a");
			}
			else if(i==1)
			{
				TTL = 8;
			    fp = fopen("p_rtt8.csv", "a");
			}
			else if(i==2)
			{
				TTL = 16;
				fp = fopen("p_rtt16.csv", "a");
			}
			portno = atoi(argv[2]);
			//P = atoi(argv[3]);
			P += 100 ;
			//TTL = atoi(argv[3]);
			//numPkts = atoi(argv[5]);
			numPkts = 50;
			totalLength = P+7;
			char buffer[totalLength];

			sockfd = socket(AF_INET, SOCK_DGRAM, 0);
			if(sockfd<0)
				error("\nError opening socket\n");
			
			server = gethostbyname(argv[1]);
			if(server == NULL)
				fprintf(stderr,"\nNo such host\n");
			
			bzero((char *)&serv_addr, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
			serv_addr.sin_port = htons(portno);
			int addr_len = sizeof(serv_addr);	


			//FILE* fp1 = fopen("i_rtt.csv", "w");
			//FILE* fp = fopen("p_rtt16.csv", "a");

			getInitialTime();
			for(seq=0;seq<numPkts;seq++)
			{
				bzero(buffer, totalLength-1);
				ttl = TTL;
				init = 0;
				while(ttl != 0) {
					fillBuffer(buffer, seq, 0, 2);
					fillBuffer(buffer, ttl, 6, 1);
					if(init == 0) {
						init = 1;
						sTime = getTimeStamp();
						fillBuffer(buffer, sTime, 2, 4);
					}
					sendto(sockfd, buffer, totalLength, 0, (struct sockaddr*)&serv_addr, addr_len);
					recvfrom(sockfd, buffer, totalLength, 0, (struct sockaddr*)&serv_addr, &addr_len);
					ttl = buffer[6] - 1;
					//buffer[6] -= 1;
					//printf("\n%s: %d :SEQ_NO -> %d%d,TTL -> %d\n",inet_ntoa(serv_addr.sin_addr),serv_addr.sin_port,buffer[0],buffer[1],buffer[6]);
				}
				rTime = getTimeStamp();
				//fprintf(fp1, "%d, %d\n", i, rTime - sTime);
				fprintf(fp, "%d, %d\n", P, rTime - sTime);
			}
			fclose(fp);
			//fclose(fp1);
			close(sockfd);
			counter++;
		}
	}

	
	printf("\nDONE!!\n");
	return 0;
}

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

uint32_t getTimeStamp() 
{
    gettimeofday(&tv, NULL);
    return ((uint32_t)tv.tv_sec - initiation) * (uint32_t)1000000 + (uint32_t)tv.tv_usec;
}

void getInitialTime() 
{
	gettimeofday(&tv, NULL);
	initiation = (uint32_t)tv.tv_sec;
}

void fillBuffer(char* buffer, int val, int pos, int bytelen) 
{
	int i;
	for(i = pos + bytelen - 1; i >= pos; i--) 
	{
		buffer[i] = val % 256;
		val /= 256;
	}
}