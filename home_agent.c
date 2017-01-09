// home agent file
// file name: home_agent.c
// this will get the packet and forward the packet to care of addr 
// here care of address taken with the help of message which is received from the mobile node

// header file declaration
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>

// main funtion
int main(int argc,char *argv[])
{
	socklen_t  clilen;
	struct sockaddr_in server, client, foreignagent;
	char buffr[1024];
	int num, sock, sq_num;
	time_t current;
	struct tm* dt;
	char time_buffr[128];
	int cop = 0;
	char *p= NULL;

	if (argc < 3) 
	{
		printf(" Syntax: home_agent <care_of_address> <mobile_node_port>\n");
		exit(1);
		}

	// Socket Creation
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) 
	{
		perror(" Opening of the Datagram socket");
		exit(1);
		}
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	
	// Binding the socket with the same port 
	server.sin_port = htons(atoi(argv[2]));
	if(bind(sock, (struct sockaddr *)&server, sizeof(server))) 
	{
		perror("binding socket name");
		exit(1);
		}

	// occupy the details of the foreign agent	
    	foreignagent.sin_family = AF_INET;
    	foreignagent.sin_addr.s_addr = inet_addr(argv[1]);
	while (1) 
	{
		bzero(buffr,sizeof(buffr));
		time(&current);
		dt = localtime(&current);
		strftime(time_buffr, 80, "%H:%M:%S", dt);
	
		// blocking logic. wait until the packet comes.
        		num = recvfrom(sock,buffr,1024,0,(struct sockaddr *)&client,&clilen);
        		if (num == -1 && (errno != EAGAIN || errno != EWOULDBLOCK)) 
			{
				perror("receiver recvfrom");
        			} 
			else if (num != -1) 
			{
				p = strstr(buffr, "REGISTER");
				if (p != NULL) 
				{
					// Register message 
					// Shifting value to the Regsiter
					p = strtok(buffr,":");
					// Shifting value to the port no 
					p = strtok(NULL,":");
					sscanf(p, "%d", &cop);
					printf(" Registration packet received. Time = %s Changing care_of_address to %s/%d\n",time_buffr, argv[1], cop);
					fflush(stdout);
					continue;
					}
				else
				{
					// Packet coming from the data source
					sscanf(buffr, "%d", &sq_num);
					if (cop == 0)
					{
						// If the registration is not deone then the packet will be rejected
						printf("Sequence Number = %d Time = %s Rejected No care_of_address yet\n",sq_num, time_buffr);
						fflush(stdout);
						continue;
						} 
					else 
					{
                        			printf("Sequence Number = %d Time = %s Forwarded to ", sq_num, time_buffr);
						fflush(stdout);
						}
					}
       		 		}
        		
			// Here current regi port will be destination port
        		foreignagent.sin_port = htons(cop);
        		if ((num=sendto(sock,buffr,sizeof(buffr),0,(struct sockaddr *)&foreignagent,sizeof(foreignagent))) < 0) 
			{
        			perror("writing on datagram socket");
        			} 
			else 
			{
				// logic for display the destination address and time
				printf("%s/%d\n",inet_ntoa(foreignagent.sin_addr), ntohs(foreignagent.sin_port));
				fflush(stdout);
        			}
			}
		close(sock);
		return 0;
	}	
