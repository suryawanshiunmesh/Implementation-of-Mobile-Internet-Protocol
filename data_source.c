// Datasource file
// filename: data_source.c
// purpose of the file is creating seq number and seding the packets to the home agent

//Declaration of the header files
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

//main function
int main(int argc,char *argv[])
{
	struct sockaddr_in server, client;
	struct hostent *hst, *gethostbyname();
	int sock, sq_num=1, num;
	char buffr[64];
	time_t current;
	struct tm* dt;
	
	if (argc < 3) 
	{
		printf(" Syntax: datasource <home_agent_ip> <mobile_node_port> \n");
		exit(1);
		}
	
	// Socket Creation
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0)
        {
		perror(" Opening of the datagram socket");
		exit(1);
		}

	// Binding the address of the client
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = INADDR_ANY;
	client.sin_port = 0;
	if (bind(sock, (struct sockaddr *)&client, sizeof(client)) < 0) 
	{
		perror(" Binding Socket Name");
		exit(1);
		}			

	server.sin_family = AF_INET;
	hst = gethostbyname(argv[1]);
	if(hst == 0) 
	{
		printf("%s: Unknown Host\n",argv[1]);
		exit(2);
		}

	bcopy(hst->h_addr, &server.sin_addr, hst->h_length);
	server.sin_port = htons(atoi(argv[2]));
	while(1) 
	{
		// Packet Creation
		time(&current);
		sprintf(buffr, "%d", sq_num);
		dt = localtime(&current);
		if ((num=sendto(sock,buffr,sizeof(buffr),0,(struct sockaddr *)&server,sizeof(server))) < 0) 
		{
			perror(" Writing on Datagram Socket");
			} 
		else
		{
			strftime(buffr, 80, "%H:%M:%S", dt);
			printf("Sequence Number = %d Time = %s Dest = %s/%s\n", sq_num, buffr, argv[1], argv[2]);
			fflush(stdout);
			}
		sq_num++;
		sleep(1);
	}
	close(sock);
	return 0;
}
