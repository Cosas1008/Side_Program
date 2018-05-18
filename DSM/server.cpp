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
#include <sstream>
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
bool checkKeyAndValue(string key, string value);
bool isExist(string key);
bool compareNat(const std::string&, const std::string&);

int *cmd_status;
int *total_cmd_count;
int *fail_cmd_count;

std::string toUpper(std::string s){
    for(int i = 0;i < (int) s.length(); i++)
	{
		s[i]=toupper(s[i]);
	}
    return s;
}


// sort struct
struct mapSortCmp{
	bool operator()(string str1, string str2){
		return compareNat(str1, str2);
	}
};

// compareNat and toUpper are refered from 
// https://stackoverflow.com/questions/13856975/how-to-sort-file-names-with-numbers-and-alphabets-in-order-in-c
bool compareNat(const std::string& a, const std::string& b){
    if (a.empty())
        return true;
    if (b.empty())
        return false;
    if (std::isdigit(a[0]) && !std::isdigit(b[0]))
        return true;
    if (!std::isdigit(a[0]) && std::isdigit(b[0]))
        return false;
    if (!std::isdigit(a[0]) && !std::isdigit(b[0]))
    {
        if (a[0] == b[0])
            return compareNat(a.substr(1), b.substr(1));
        return (toUpper(a) < toUpper(b));
        //toUpper() is a function to convert a std::string to uppercase.
    }

    // Both strings begin with digit --> parse both numbers
    std::istringstream issa(a);
    std::istringstream issb(b);
    int ia, ib;
    issa >> ia;
    issb >> ib;
    if (ia != ib)
        return ia < ib;

    // Numbers are the same --> remove numbers and recurse
    std::string anew, bnew;
    std::getline(issa, anew);
    std::getline(issb, bnew);
    return (compareNat(anew, bnew));
}

// Shared Memory
int create_share_mem(key_t key, size_t size, int **shm_addr) 
{
    int shmid = shmget(key, size, 0666 | IPC_CREAT);

	*shm_addr = (int*) shmat(shmid, NULL, 0);

    return shmid;
}

void clean_share_mem(int* shm_addr, int shm_id) {
	shmdt(shm_addr);
	shmctl(shm_id , IPC_RMID , NULL);
}

int main(int argc, char *argv[], char *envp[])
{
	int n;
	char buffer[MAXLINE];
	char *pch;
	map<string, string, mapSortCmp> mResult;
	
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
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	checkConnection();

	// point to the shared memory address
	int cmd_status_id = create_share_mem(CMD_STATUS, sizeof(int), &cmd_status);
	int total_cmd_count_id = create_share_mem(TOTAL_CMD_COUNT, sizeof(int), &total_cmd_count);
	int fail_cmd_count_id = create_share_mem(FAIL_CMD_COUNT, sizeof(int), &fail_cmd_count);
	
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
			else {
				// flush database
				redisReply *r = (redisReply *) redisCommand(redis, "flushall");
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
				// checkKeyAndValue(key,value);
				if (isExist(key) || (std::stoi(value) < 0 )) {	// std::stoi is the C++11 function
					*fail_cmd_count += 1;
					cout << "key failed " << endl;
					// strcpy(rindex,"err!\n");
					// write(connfd, rindex, strlen(rindex)); // reply to client index
					// *cmd_status = atoi(cmd_idx);
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
					// strcpy(rindex,"err!\n");
					// write(connfd, rindex, strlen(rindex)); // reply to client index
					// *cmd_status = atoi(cmd_idx);
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
					// strcpy(rindex,"err!\n");
					// write(connfd, rindex, strlen(rindex)); // reply to client index
					// *cmd_status = atoi(cmd_idx);
					*cmd_status = atoi(cmd_idx);
					continue;
				}	
				
				snprintf(redisCmd, sizeof(redisCmd), "decrby %s %s", key.c_str(), value.c_str());
				redisReply *r = (redisReply *) redisCommand(redis, redisCmd);
				freeReplyObject(r);
				// cout << "decrby: " <<  key.c_str() << " " << value.c_str() << " " << r->integer << endl; // debug
				
				// get return string
				if (r->integer < 0) {
					snprintf(redisCmd, sizeof(redisCmd), "incrby %s %s", key.c_str(), value.c_str());
					r = (redisReply *) redisCommand(redis, redisCmd);
					freeReplyObject(r);
					*fail_cmd_count += 1;
					cout << "key failed " << endl;
					// strcpy(rindex,"err!\n");
					// write(connfd, rindex, strlen(rindex)); // reply to client index
					// *cmd_status = atoi(cmd_idx);
					*cmd_status = atoi(cmd_idx);
					continue;
				}
				
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
				
				if (!isExist(key1) || !isExist(key2) || (std::stoi(value.c_str()) < 0) || key1.compare(key2) == 0) {
					strcpy(rindex,"err!\n");
					cout << "key failed" << endl;
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
					*fail_cmd_count += 1;
					// strcpy(rindex,"err!\n");
					// continue;
				}
				else {
					snprintf(redisCmd, sizeof(redisCmd), "incrby %s %s", key2.c_str(), value.c_str());
					r = (redisReply *) redisCommand(redis, redisCmd);
					
				}
				
				freeReplyObject(r);
			}
			else if (!strcmp(cmd, "end")) {
				cout << "In end" <<endl;
				redisReply *r = (redisReply *) redisCommand(redis, "keys *");
				for (int i = 0; i < r->elements; i++) {
					char redisCmd[MAXLINE];
					char key[MAXLINE];

					snprintf(redisCmd, sizeof(redisCmd), "get %s", r->element[i]->str);
					strcpy(key, r->element[i]->str);

					redisReply *r = (redisReply *) redisCommand(redis, redisCmd);
					
					mResult[key] = r->str;
				}
				freeReplyObject(r);

				// std::sort(mResult.begin(), mResult.end(), compareNat);

				for(map<string, string>::iterator it = mResult.begin(); it != mResult.end(); it++) {
					char r[MAXLINE];
					snprintf(r, sizeof(r), "%s : %s\n", (*it).first.c_str(), (*it).second.c_str());
					//cout << (*it).first << " : " << (*it).second << endl;
					write(connfd, r, strlen(r));
				}
				
				// calculate the success rate
				int total = *cmd_status;
				int fail = *fail_cmd_count;
				int success = total - fail;
			
				char r2[MAXLINE];
				snprintf(r2, sizeof(r2), "\nsuccess rate : (%d/%d)\n", success, total);
				write(connfd, r2, strlen(r2));
				// cout << endl << "success rate : (" << *cmd_status - *fail_cmd_count << "/" << *cmd_status << ")" << endl;
				*cmd_status = 0;
				*total_cmd_count = 0;
				*fail_cmd_count = 0;
				break;
			}

			*cmd_status = atoi(cmd_idx);
			
			//strcat(rindex, "\n");
			//write(connfd, rindex, strlen(rindex)); // reply to client index
			//printf("afeter write\n");
		}
		// clear shared memory
		// *cmd_status = 0;
		// *fail_cmd_count = 0;
		
		mResult.clear();

		close(connfd);
	}
	clean_share_mem(cmd_status, cmd_status_id);
	clean_share_mem(total_cmd_count, total_cmd_count_id);
	clean_share_mem(fail_cmd_count, fail_cmd_count_id);
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

bool checkKeyAndValue(string key, string value)
{
	if (isExist(key) || (std::stoi(value) < 0)){
		*fail_cmd_count += 1;
		cout << "key failed " << endl;
		// strcpy(rindex,"err!\n");
		// write(connfd, rindex, strlen(rindex)); // reply to client index
		// *cmd_status = atoi(cmd_idx);
		return false;
	}
	return true;
}

bool isExist(string key) 
{
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