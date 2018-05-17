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
  	char *lastdot, *oFilename;
	char buffer[MAXLINE];
	struct sockaddr_in server_addr;
	ifstream fin;
	ofstream fout;
	string cmd;

	if(argc != 6) {
		printf("Usage: client <IP addr> <port> <port> <port> <filename>\n");
		return 0;
	}
	cout << "Opening file " << argv[5] << endl;
	fin.open(argv[5]);

	char *filename = strtok(argv[5], ".");
	// if(lastdot != NULL){
	// 	strcpy(oFilename,argv[5], ???); // help me!
	// }else{
	// 	strcpy(oFilename,argv[5], sizeof(argv[5]));
	// }

	fout.open(strcat(filename,"_output.txt"));
	if(!fin.is_open()) {
		perror("Reading file error");
		exit(errno);
	}
	if(!fout.is_open()){
		perror("Opening output file error");
		exit(errno);
	}
	
	int server1 = connectToServer(argv[1], argv[2]);
  	// printf("Connect to %s : %s\n", argv[1], argv[2]);
	int server2 = connectToServer(argv[1], argv[3]);
	// printf("Connect to %s : %s\n", argv[1], argv[3]);
  	int server3 = connectToServer(argv[1], argv[4]);
	// printf("Connect to %s : %s\n", argv[1], argv[4]);
 
	while(getline(fin, cmd)) {
		// cout << totalCnt << cmd << endl;	// debug cmd
    	
		if (totalCnt % 3 == 0)
			sockfd = server1;
		else if(totalCnt % 3 == 1)
			sockfd = server2;
		else
			sockfd = server3;
		
		char idx_cmd[MAXLINE];

		snprintf(idx_cmd, MAXLINE, "%d %s\n", totalCnt+1, cmd.c_str());

		write(sockfd,idx_cmd, strlen(idx_cmd));

		if(cmd == "end") {
        	break;
    	}
    	// blocking req/res
	    // while( (n = readline(sockfd, buffer, MAXLINE)) > 0)
	    // {
	    // 	printf("%s\n", buffer);
	    // 	if(!strcmp(buffer, "err!\n")){
	    // 		cout << "err!" << endl;
		//         failCnt += 1;		//fail
		//         break;
		//     }else if(!strcmp(buffer, "okay!\n")) {
		//     	cout << "okay!" << endl;
		//     	break;
		//     }
	    // }
    	totalCnt+= 1;
	}
	cout << "Done sending" <<endl;
	while( (n = readline(sockfd, buffer, MAXLINE)) > 0) 
	{
		buffer[n] = 0;
		fout << buffer;
		cout << buffer;
	}
	// cout << "Total is : " << totalCnt << endl;
	// cout << "success rate : (" << totalCnt - failCnt << "/" << totalCnt << ")" << endl;
	// fout << "success rate : (" << totalCnt - failCnt << "/" << totalCnt << ")" << endl;
	fin.close();
	fout.close();
	close(server1);
	close(server2);
	close(server3);
}