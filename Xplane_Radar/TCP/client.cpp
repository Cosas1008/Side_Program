#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "readline.h"

using namespace std;


int connectToServer(char *host, char *port) 
{
	int sockfd, n;
	char buffer[MAXLINE];
	struct sockaddr_in server_addr;

	bzero(&server_addr, sizeof(server_addr) );
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(port));

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
		exit(errno);
	}

	if(inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
		perror("inet_pton error");
		exit(errno);
	}

	if(connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		perror("connect error");
		exit(errno);
	}
	
	return sockfd;
}


int main(int argc, char **argv)
{
	int sockfd, n;
  	int totalCnt = 0, failCnt = 0;
	char buffer[MAXLINE];
	struct sockaddr_in server_addr;

	if(argc != 4) {
		printf("Usage: client <IP addr> <port> <filename>\n");
		return 0;
	}

	// cout << "bmp file name " << argv[3] << endl;

	// strcpy(buffer,argv[3]);
	
	snprintf(buffer, MAXLINE, "%s \n", argv[3]);
	cout << "bmp file name " << buffer << endl;

	
	sockfd = connectToServer(argv[1], argv[2]);
	
	write(sockfd,buffer, strlen(buffer));

	// cout << "Done sending" <<endl;

	close(sockfd);
}