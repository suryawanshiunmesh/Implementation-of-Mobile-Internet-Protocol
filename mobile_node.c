// mobile node file
// file name: mobile_node.c
// it will receives the packet from the Foreign Agent & send the regi. packet to the home agent
// this process will execute at every 5 secs. 
// this program will terminate after 100 secs.

// header file declaration
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

// main function
int main(int argc,char *argv[])
{
	socklen_t clilen;
	struct sockaddr_in server, client, homeagent;
	char buffr[1024];
	int num, sock, sq_num;
	time_t current;
	struct tm* dt;
	char time_buffr[128];
	struct timeval current_t, past_t, start_t;
	int current_f_p, base_f_p, no_fa, offset_f_p = 0;

	if (argc < 6) 
	{
		printf(" Syntax: mobile_node <mobile_node_port> <home_agent_address> <foreign_agent_ip> <foreign_agent_base_port> <total_no _of_foreign_agents>\n");
		exit(1);
		}

	//Socket creation
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) 
	{
		perror(" Opening of the Datagram socket");
		exit(1);
		}

	// logic for non blocking
	int flags = fcntl(sock, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(sock, F_SETFL, flags);

	// Assign the name for socket
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(atoi(argv[1]));
	if(bind(sock, (struct sockaddr *)&server, sizeof(server))) 
	{
		perror(" Binding the Socket name");
		exit(1);
		}
	
	//Occupy the details of the home agent for the regi. purpose
	homeagent.sin_family = AF_INET;
	homeagent.sin_addr.s_addr = inet_addr(argv[2]);
	homeagent.sin_port = htons(atoi(argv[1]));

	// by using the foreign base port linking is done
	no_fa = atoi(argv[5]);
	base_f_p = atoi(argv[4]);

	gettimeofday(&current_t, NULL);
	memcpy(&past_t, &current_t, sizeof(struct timeval));
	memcpy(&start_t, &current_t, sizeof(struct timeval));
	while (1) 
	{
		bzero(buffr,sizeof(buffr));
		time(&current);
		dt = localtime(&current);
		strftime(time_buffr, 80, "%H:%M:%S", dt);
		gettimeofday(&current_t, NULL);
		if (current_t.tv_sec - start_t.tv_sec >= 100) 
		{
			printf(" 100 Second Completed..Terminating the Program..\n");
			break;
			}
		
		// logic for sending the regi. message.
		if(current_t.tv_sec - past_t.tv_sec >= 5) 
		{
			offset_f_p %= no_fa; // care of address will be creating by using the offset and base foreign port
			current_f_p = base_f_p+offset_f_p;
			sprintf(buffr, "REGISTER:%d",current_f_p);
			if ((num=sendto(sock,buffr,sizeof(buffr),0,(struct sockaddr *)&homeagent,sizeof(homeagent))) < 0) 
			{
				perror("writing on datagram socket");
				} 
			else 	
			{
				printf("Registration sent.  Time = %s  New care-of address = %s/%d\n",time_buffr, argv[3],current_f_p);
				fflush(stdout);
				}
			offset_f_p++;
			memcpy(&past_t, &current_t, sizeof(struct timeval));
			continue;
			}
		
		num = recvfrom(sock,buffr,1024,0,(struct sockaddr *)&client,&clilen);
		if (num == -1 && (errno != EAGAIN || errno != EWOULDBLOCK)) 
		{
			perror("receiver recvfrom");
			}
		else if (num > 0) 
		{
			sscanf(buffr, "%d", &sq_num);
			if (ntohs(client.sin_port) != current_f_p)
                        {
			    	printf("Sequence Number = %d Time = %s FA = %s/%d Rejected\n",sq_num, time_buffr, inet_ntoa(client.sin_addr), ntohs(client.sin_port));
				}
		 	else
			{
				printf("Sequence Number = %d Time = %s FA = %s/%d Accepted\n",sq_num, time_buffr, inet_ntoa(client.sin_addr), ntohs(client.sin_port));
				}
			fflush(stdout);
			}
		// logic is implemented for the proper utlilization of the CPU
		usleep(100000);
		}
	close(sock);
	return 0;
	}
