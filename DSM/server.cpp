#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <map>
#include "readline.h"
#include "hiredis/hiredis.h"

using namespace std;

redisContext *redis;

bool isExist(string key) {
	char cmd[MAXLINE];
	int result;
	
	snprintf(cmd, sizeof(cmd), "exists %s", key.c_str());
	redisReply *r = (redisReply *) redisCommand(redis, cmd);

	if (r->integer == 1) {
		freeReplyObject(r);
		return true;
	}
	else {
		freeReplyObject(r);
		return false;
	} 
}
int listenfd, connfd;
struct sockaddr_in server_addr;
void checkConnection();

int main(int argc, char *argv[], char *envp[])
{
	int n;
	// int failCnt = 0, totalCnt = 0;
	char buffer[MAXLINE];
	char *pch;
	map<string, string> mResult;
	
	if(argc != 2) {
		printf("Usage: ./server <port>\n");
		exit(0);
	}
	
	// connect to redis
	redis = (redisContext *) redisConnect("127.0.0.1", 6379);
	if ((redis == NULL) || (redis->err)) {
		cout << "Fail to connect..." << endl;
		return -1;
	}
	
	// flush database
	redisReply *r = (redisReply *) redisCommand(redis, "flushall");
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	checkConnection();
	
	while (1) {
		connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);
		
		while((n = readline(connfd, buffer, MAXLINE)) > 0) {
			char cmd[100] = {0}, key[100] = {0}, value[100] = {0};
			char rindex[100] = "okay!";
			buffer[n] = 0;
			// totalCnt += 1;
			
			if((pch = strtok(buffer, " \n"))) {
				strcpy(cmd, pch);
			}
			cout << argv[1] << " : " << cmd << endl;
			if (!strcmp(cmd, "init")) {
				char redisCmd[MAXLINE];
				char reply[MAXLINE];
				string key, value;
				if((pch = strtok(NULL, " \n"))) {
					key = string(pch);
					transform(key.begin(), key.end(), key.begin(), ::tolower);
				}
				if((pch = strtok(NULL, " \n"))) {
					value = string(pch);
				}

				if (isExist(key) || (std::stoi(value) < 0 )) {	// std::stoi is the C++11 function
					// failCnt += 1;
					cout << "key failed " << endl;
					strcpy(rindex,"err!\n");
					write(connfd, rindex, strlen(rindex)); // reply to client index
					continue;
				}
				snprintf(redisCmd, sizeof(redisCmd), "set %s %s", key.c_str(), value.c_str());
				redisReply *r = (redisReply *) redisCommand(redis, redisCmd);
				freeReplyObject(r);
			}
			else if (!strcmp(cmd, "save")) {
				char redisCmd[MAXLINE];
				string key, value;
				if((pch = strtok(NULL, " \n"))) {
					key = string(pch);
					transform(key.begin(), key.end(), key.begin(), ::tolower);
				}
				if((pch = strtok(NULL, " \n"))) {
					value = string(pch);
				}

				if (!isExist(key) || (std::stoi(value) < 0)){
					// failCnt += 1;
					strcpy(rindex,"err!\n");
					write(connfd, rindex, strlen(rindex)); // reply to client index
					continue;
				}		

				snprintf(redisCmd, sizeof(redisCmd), "incrby %s %s", key.c_str(), value.c_str());
				
				redisReply *r = (redisReply *) redisCommand(redis, redisCmd);
				freeReplyObject(r);
			}
			else if (!strcmp(cmd, "load")) {
				char redisCmd[MAXLINE];
				string key, value;
				
				if((pch = strtok(NULL, " \n"))) {
					key = string(pch);
					transform(key.begin(), key.end(), key.begin(), ::tolower);
				}
				if((pch = strtok(NULL, " \n"))) {
					value = string(pch);
				}

				if (!isExist(key) || (std::stoi(value) < 0)) {
					// failCnt += 1;
					strcpy(rindex,"err!\n");
					write(connfd, rindex, strlen(rindex)); // reply to client index
					continue;
				}	
				
				snprintf(redisCmd, sizeof(redisCmd), "decrby %s %s", key.c_str(), value.c_str());
				redisReply *r = (redisReply *) redisCommand(redis, redisCmd);
				freeReplyObject(r);

				if (r->integer < 0) {
					snprintf(redisCmd, sizeof(redisCmd), "incrby %s %s", key.c_str(), value.c_str());
					r = (redisReply *) redisCommand(redis, redisCmd);
					freeReplyObject(r);
					// failCnt += 1;
					strcpy(rindex,"err!\n");
					write(connfd, rindex, strlen(rindex)); // reply to client index
					continue;
				}
			}
			else if (!strcmp(cmd, "remit")) {
				char redisCmd[MAXLINE];
				string key1, key2, value;
				
				if((pch = strtok(NULL, " \n"))) {
					key1 = string(pch);
					transform(key1.begin(), key1.end(), key1.begin(), ::tolower);
				}
				if((pch = strtok(NULL, " \n"))) {
					key2 = string(pch);
					transform(key2.begin(), key2.end(), key2.begin(), ::tolower);
				}
				if((pch = strtok(NULL, " \n"))) {
					value = string(pch);
				}
				
				if (!isExist(key1) || !isExist(key2) || (std::stoi(value.c_str()) < 0)) {
					strcpy(rindex,"err!\n");
					// failCnt += 1;
					write(connfd, rindex, strlen(rindex)); // reply to client index
					continue;
				}	
				
				snprintf(redisCmd, sizeof(redisCmd), "decrby %s %s", key1.c_str(), value.c_str());
				redisReply *r = (redisReply *) redisCommand(redis, redisCmd);
				
				if (r->integer < 0) {
					snprintf(redisCmd, sizeof(redisCmd), "incrby %s %s", key1.c_str(), value.c_str());
					r = (redisReply *) redisCommand(redis, redisCmd);
					freeReplyObject(r);
					strcpy(rindex,"err!\n");
					// failCnt += 1;
				}
				else {
					snprintf(redisCmd, sizeof(redisCmd), "incrby %s %s", key2.c_str(), value.c_str());

					r = (redisReply *) redisCommand(redis, redisCmd);
				}
				
				freeReplyObject(r);
			}
			else if (!strcmp(cmd, "end")) {
				redisReply *r = (redisReply *) redisCommand(redis, "keys *");
				for (int i = 0; i < r->elements; i++) {
					char redisCmd[MAXLINE];
					char key[MAXLINE];

					snprintf(redisCmd, sizeof(redisCmd), "get %s", r->element[i]->str);
					strcpy(key, r->element[i]->str);

					redisReply *r = (redisReply *) redisCommand(redis, redisCmd);
					
					mResult[key] = r->str;
				}
				
				for(map<string, string>::iterator it = mResult.begin(); it != mResult.end(); it++) {
					char r[MAXLINE];
					snprintf(r, sizeof(r), "%s : %s\n", (*it).first.c_str(), (*it).second.c_str());
					//cout << (*it).first << " : " << (*it).second << endl;
					write(connfd, r, strlen(r));
				}
				// calculate the success rate
				// char r2[MAXLINE];
				// snprintf(r2, sizeof(r2), "\nsuccess rate : (%d/%d)\n", totalCnt - failCnt, totalCnt);
				// write(connfd, r2, strlen(r2));
				//cout << endl << "success rate : (" << totalCnt - failCnt << "/" << totalCnt << ")" << endl;
				freeReplyObject(r);
				break;
			}

			strcat(rindex, "\n");
			write(connfd, rindex, strlen(rindex)); // reply to client index
			//printf("afeter write\n");
		}
		mResult.clear();
		close(connfd);
	}
	
	redisFree(redis);
}


void checkConnection()
{
	if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket Error");
		exit(errno);
	}

	if( bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		perror("Bind Error");
		exit(errno);
	}

	if( listen(listenfd, 20) < 0 ) {
		perror("Listen Error");
		exit(errno);
	}
}