#include <iostream>
#include <string.h>     // bzero
#include <sys/socket.h> // unused?
#include <netinet/in.h>
#include <unistd.h>     // read function

#define PORT 8080

using 
namespace std;


int listenfd, connfd;
struct sockaddr_in server_addr;
void checkConnection();

int main(int argc, char const *argv[])
{
    int valread;
    int addrlen = sizeof(server_addr);
    /* code */
    bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	checkConnection();

    if ((connfd = accept(listenfd, (struct sockaddr *)&server_addr, 
                       (socklen_t*)&addrlen))<0)
    {
        perror("Accept Error");
        exit(EXIT_FAILURE);
    }
    char buffer[1024];
    valread = read( connfd , buffer, 1024);
    printf("%s\n",buffer );
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