// Foreign agent file
// file name: foreign_agent.c
// foreign agent will act as a router and it weill send the packets coming from the home agent to mobile node.

// header files declaration 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

//main function
int main(int argc,char *argv[])
{
	socklen_t  clilen;
	struct sockaddr_in server, client, mobilenode;
	char buffr[1024];
	int num, sock, sq_num;
	time_t current;
	struct tm* dt;
	char time_buffr[128];

	if (argc < 4) 
	{
		printf(" Syntax: foreign_agent <foreign_agent_care_of_port> <mobile_node_ip> <mobile_node_port>\n");
		exit(1);
		}
	
	// Socket creation
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) 
	{
		perror(" Opening of the Datagram socket");
		exit(1);
		}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(atoi(argv[1]));
	if(bind(sock, (struct sockaddr *)&server, sizeof(server)))
        {
		perror(" Binding the Socket name");
		exit(1);
		}

	// occupy the deatils of the mobile node to forward it.
    	mobilenode.sin_family = AF_INET;
    	mobilenode.sin_addr.s_addr = inet_addr(argv[2]);
    	mobilenode.sin_port = htons(atoi(argv[3]));
	while (1) 
	{
		bzero(buffr,sizeof(buffr));
		time(&current);
		dt = localtime(&current);
		strftime(time_buffr, 80, "%H:%M:%S", dt);
		num = recvfrom(sock,buffr,1024,0,(struct sockaddr *)&client,&clilen);
		if (num == -1 && (errno != EAGAIN || errno != EWOULDBLOCK)) 
		{
			perror("receiver recvfrom");
			} 
		else if (num != -1) 
		{
			sscanf(buffr, "%d", &sq_num);
			printf("Sequence Number = %d Time = %s Forwarded to ", sq_num, time_buffr);
			fflush(stdout);
			}
	
		if ((num=sendto(sock,buffr,sizeof(buffr),0,(struct sockaddr *)&mobilenode,sizeof(mobilenode))) < 0) 
		{
			perror("writing on datagram socket");
			}
		else 
		{
			printf("%s/%d\n",inet_ntoa(mobilenode.sin_addr), ntohs(mobilenode.sin_port));
			fflush(stdout);
			}
		}
	close(sock);
	return 0;
	}

