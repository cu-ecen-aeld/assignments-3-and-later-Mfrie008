#include "aesdsocket.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <syslog.h>

#include <signal.h>

#define BUF_SZ		1024
#define WRITE_FILE	"/var/tmp/aesdsocketdata"

volatile sig_atomic_t runServer = true;

void signal_handler(int signo)
{
	printf("%s\n", "Signal was received!");
	syslog(LOG_INFO, "Caught signal, exiting\n");
	runServer = false;
}

int main(int argc, char **argv)
{
	struct sigaction sigAction;
	memset(&sigAction, 0, sizeof(sigAction));
	sigAction.sa_handler = signal_handler;
	sigaction(SIGINT, &sigAction, NULL);
	sigaction(SIGTERM, &sigAction, NULL);
	printf("Registered sigaction\n");

	char msgBuf[BUF_SZ];
	char host[NI_MAXHOST];
	int opt = 1;
	
	FILE* writefilep; 
	size_t numWritten = 0;
	
	// Open file for appending
	writefilep = fopen(WRITE_FILE, "a+");
	if(writefilep == 0)
	{
		printf("Can't open file\n");
		return -1;
	}
	
	// Get binding information
	struct addrinfo hints;
	struct addrinfo *servinfo;
	
	memset(&hints, 0 , sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	int infoRc = getaddrinfo(NULL, "9000", &hints, &servinfo);
	if(infoRc != 0)
	{
		return -1;
	}
	printf("Got address info\n");

	// Create socket, bind, free malloc'd addrinfo
	int socketFd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	int bindRc = bind(socketFd, servinfo->ai_addr, servinfo->ai_addrlen);
	freeaddrinfo(servinfo);
	int listenRc = listen(socketFd, SOMAXCONN);
	
	int flags = fcntl(socketFd, F_GETFL, 0);
	fcntl(socketFd, F_SETFL, flags | O_NONBLOCK);
	
	if(socketFd == -1 || bindRc != 0 || listenRc != 0)
	{
		close(socketFd);
		return -1;
	}
	printf("Created socket, running server\n");
	
	// Run server	
	while(1)
	{	
		if(!runServer)
		{
			printf("%s\n", "Closing down server");
			close(socketFd);
			
			fflush(writefilep);
			if(fclose(writefilep) == 0)
			{
				int remStat = remove(WRITE_FILE);
				printf("Remove stat: %d\n", remStat);
			}
			break;
		}
		
		// Connect
		struct sockaddr_storage clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);	
		int clientFd = accept(socketFd, (struct sockaddr *)&clientAddr, &clientAddrLen);

		if(clientFd != -1)
		{
			getnameinfo((struct sockaddr *)&clientAddr, clientAddrLen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);

			printf("Accepted connection from %s\n", host);
			syslog(LOG_INFO, "Accepted connection from %s\n", host);
		}
		else
		{
			continue;
		}
		
		// Process packet until \n detected
		memset(msgBuf, 0, sizeof(msgBuf));
		while(1)
		{
			ssize_t numRecvd = recv(clientFd, (void*)msgBuf, BUF_SZ, 0);
			if(numRecvd > 0)
			{
				printf("Got %ld bytes: %s", numRecvd, msgBuf);
				
				// Write to text file
				numWritten = 0;
				while(numWritten < numRecvd)
				{
					numWritten += fwrite(msgBuf + numWritten, 1, numRecvd - numWritten, writefilep);
				}
				fflush(writefilep);
				
				char *searchPtr = strchr(msgBuf, '\n');
				if(searchPtr != NULL)
				{
					int bytesRead;
					
					fflush(writefilep);
					rewind(writefilep);
					while((bytesRead = fread(msgBuf, 1, BUF_SZ, writefilep)) > 0)
					{
						send(clientFd, msgBuf, bytesRead, 0);
						// Note: may need to handle partial send
					}
					
					close(clientFd);
				
					printf("Closed connection from %s\n", host);
					syslog(LOG_INFO, "Closed connection from %s\n", host);
					break;
				}
			}
		}
	}
	
	return 0;
}
