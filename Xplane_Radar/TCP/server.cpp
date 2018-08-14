#include <iostream>
#include <stdlib.h>     // exit
#include <stdio.h>      // perror
#include <string.h>     // bzero
#include <cerrno>       // errno
#include <sys/socket.h> // unused?
#include <netinet/in.h>
#include <unistd.h>     // read function
#include "readline.h"
#define LENGTH 255
using namespace std;


int listenfd, connfd;
struct sockaddr_in server_addr;
void checkConnection();
void openbmpImage(char * );

int main(int argc, char const *argv[])
{
    char buffer[MAXLINE];
    char *pch;
    int valread;
    int addrlen = sizeof(server_addr);
    /* code */
    bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
    // server_addr.sin_port = PORT;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	checkConnection();
    while(1){
        connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);
        // cout << "Connected" << endl;
        while((valread = readline(connfd, buffer, MAXLINE)) > 0){
            char filepwd[LENGTH] = {0};
            buffer[valread] = 0;

            cout << "In buffer : " << buffer << endl;
            // get file pwd
            if((pch = strtok(buffer, " \r\n"))){
                strcpy(filepwd, pch);
            }

            if(!strcmp(filepwd, "0")){
                // block do nothing
                cout << "server idle\n" << endl;
            }else{
                // open bmp file
                cout << "server open: " << filepwd << endl;
                // openbmpImage(filepwd);
            }
            
            cout << "filepwd is : " << filepwd << endl;
        }

        close(connfd);
    }
    if ((connfd = accept(listenfd, (struct sockaddr *)&server_addr, 
                       (socklen_t*)&addrlen))<0)
    {
        perror("Accept Error");
        exit(EXIT_FAILURE);
    }
    return 0;
}

void checkConnection()
{
    int opt = 1;

    // Creating socket file descriptor
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 8080
    // Prevents error such as: “address already in use”
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Set sock opt failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 8080
	if( bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		perror("Bind Error");
		exit(errno);
	}
	if( listen(listenfd, 20) < 0 ) {
		perror("Listen Error");
		exit(errno);
	}

}

void openbmpImage(char * arr)
{
    char temp[LENGTH];
    strcpy(temp,arr);
    printf("Received File : %s", temp);
}