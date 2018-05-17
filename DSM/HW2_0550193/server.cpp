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

#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_CMD_NUM 10000
#define CMD_STATUS 1111
#define TOTAL_CMD_COUNT 2222
#define FAIL_CMD_COUNT 3333

using namespace std;


// Global variable
redisContext *redis;
int listenfd, connfd;
struct sockaddr_in server_addr;
void checkConnection();


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


// Shared Memory
int *create_share_mem(key_t key, size_t size) 
{
    int shmid = shmget(key, size, 0666 | IPC_CREAT);

    return (int*) shmat(shmid, NULL, 0);
}

void clean_share_mem(char* shm_addr, int shm_id) {
	shmdt(shm_addr);
	shmctl(shm_id , IPC_RMID , NULL);
}

int main(int argc, char *argv[], char *envp[])
{
	int n;
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

	int *cmd_status = create_share_mem(CMD_STATUS, sizeof(int));
	int *total_cmd_count = create_share_mem(TOTAL_CMD_COUNT, sizeof(int));
	int *fail_cmd_count = create_share_mem(FAIL_CMD_COUNT, sizeof(int));

	*cmd_status = 0;
	*total_cmd_count = 0;
	*fail_cmd_count = 0;
	
	while (1) {
		connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);
		
		while((n = readline(connfd, buffer, MAXLINE)) > 0) {
			char cmd[100] = {0}, key[100] = {0}, value[100] = {0}, cmd_idx[100] = {0};
			char rindex[100] = "okay!";
			buffer[n] = 0;
			// totalCnt += 1;
		
			if((pch = strtok(buffer, " \n"))) {
				strcpy(cmd_idx, pch);
			}
			if((pch = strtok(NULL, " \r\n"))) {
				strcpy(cmd, pch);
			}
		   	
			// block
			if (strcmp(cmd_idx, "1")) {
				while (*cmd_status != atoi(cmd_idx)-1) {
					// block until previous command finished
				}
			}
			cout << argv[1] << " : " << cmd_idx << " " << cmd << endl;

			if (!strcmp(cmd, "init")) {
				char redisCmd[MAXLINE];
				char reply[MAXLINE];
				string key, value;
				if((pch = strtok(NULL, " \r\n"))) {
					key = string(pch);
					transform(key.begin(), key.end(), key.begin(), ::tolower);
				}
				if((pch = strtok(NULL, " \r\n"))) {
					value = string(pch);
				}

				if (isExist(key) || (std::stoi(value) < 0 )) {	// std::stoi is the C++11 function
					*fail_cmd_count += 1;
					cout << "key failed " << endl;
					strcpy(rindex,"err!\n");
					// write(connfd, rindex, strlen(rindex)); // reply to client index
					*cmd_status = atoi(cmd_idx);
					continue;
				}
				snprintf(redisCmd, sizeof(redisCmd), "set %s %s", key.c_str(), value.c_str());
				redisReply *r = (redisReply *) redisCommand(redis, redisCmd);
				freeReplyObject(r);
			}
			else if (!strcmp(cmd, "save")) {
				char redisCmd[MAXLINE];
				string key, value;
				if((pch = strtok(NULL, " \r\n"))) {
					key = string(pch);
					transform(key.begin(), key.end(), key.begin(), ::tolower);
				}
				if((pch = strtok(NULL, " \r\n"))) {
					value = string(pch);
				}

				if (!isExist(key) || (std::stoi(value) < 0)){
					*fail_cmd_count += 1;
					cout << "key failed " << endl;
					strcpy(rindex,"err!\n");
					// write(connfd, rindex, strlen(rindex)); // reply to client index
					*cmd_status = atoi(cmd_idx);
					continue;
				}		

				snprintf(redisCmd, sizeof(redisCmd), "incrby %s %s", key.c_str(), value.c_str());
				
				redisReply *r = (redisReply *) redisCommand(redis, redisCmd);
				freeReplyObject(r);
			}
			else if (!strcmp(cmd, "load")) {
				char redisCmd[MAXLINE];
				string key, value;
				
				if((pch = strtok(NULL, " \r\n"))) {
					key = string(pch);
					transform(key.begin(), key.end(), key.begin(), ::tolower);
				}
				if((pch = strtok(NULL, " \r\n"))) {
					value = string(pch);
				}

				if (!isExist(key) || (std::stoi(value) < 0)) {
					*fail_cmd_count += 1;
					cout << "key failed " << endl;
					strcpy(rindex,"err!\n");
					// write(connfd, rindex, strlen(rindex)); // reply to client index
					*cmd_status = atoi(cmd_idx);
					continue;
				}	
				
				snprintf(redisCmd, sizeof(redisCmd), "decrby %s %s", key.c_str(), value.c_str());
				redisReply *r = (redisReply *) redisCommand(redis, redisCmd);
				cout << "decrby: " <<  key.c_str() << " " << value.c_str() << " " << r->integer << endl; // debug

				// get return string
				if (r->integer < 0) {
					snprintf(redisCmd, sizeof(redisCmd), "incrby %s %s", key.c_str(), value.c_str());
					r = (redisReply *) redisCommand(redis, redisCmd);
					freeReplyObject(r);
					*fail_cmd_count += 1;
					strcpy(rindex,"err!\n");
					cout << "key failed " << endl;
					// write(connfd, rindex, strlen(rindex)); // reply to client index
					*cmd_status = atoi(cmd_idx);
					continue;
				}
				freeReplyObject(r);
			}
			else if (!strcmp(cmd, "remit")) {
				char redisCmd[MAXLINE];
				string key1, key2, value;
				
				if((pch = strtok(NULL, " \r\n"))) {
					key1 = string(pch);
					transform(key1.begin(), key1.end(), key1.begin(), ::tolower);
				}
				if((pch = strtok(NULL, " \r\n"))) {
					key2 = string(pch);
					transform(key2.begin(), key2.end(), key2.begin(), ::tolower);
				}
				if((pch = strtok(NULL, " \r\n"))) {
					value = string(pch);
				}
				
				if (!isExist(key1) || !isExist(key2) || (std::stoi(value.c_str()) < 0)) {
					strcpy(rindex,"err!\n");
					cout << "key failed " << endl;
					*fail_cmd_count += 1;
					// write(connfd, rindex, strlen(rindex)); // reply to client index
					*cmd_status = atoi(cmd_idx);
					continue;
				}	
				
				snprintf(redisCmd, sizeof(redisCmd), "decrby %s %s", key1.c_str(), value.c_str());
				redisReply *r = (redisReply *) redisCommand(redis, redisCmd);
				
				if (r->integer < 0) {
					snprintf(redisCmd, sizeof(redisCmd), "incrby %s %s", key1.c_str(), value.c_str());
					r = (redisReply *) redisCommand(redis, redisCmd);
					freeReplyObject(r);
					strcpy(rindex,"err!\n");
					*fail_cmd_count += 1;
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
				char r2[MAXLINE];
				
				snprintf(r2, sizeof(r2), "\nsuccess rate : (%d/%d)\n", *cmd_status - *fail_cmd_count, *cmd_status);
				write(connfd, r2, strlen(r2));
				// cout << endl << "success rate : (" << *cmd_status - *fail_cmd_count << "/" << *cmd_status << ")" << endl;
				freeReplyObject(r);
				break;
			}

			*cmd_status = atoi(cmd_idx);
			
			//strcat(rindex, "\n");
			//write(connfd, rindex, strlen(rindex)); // reply to client index
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