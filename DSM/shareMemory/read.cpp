#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
using namespace std;


char *create_share_mem(key_t key, size_t size) {
	int shm_id;
 	// shmget returns an identifier in shmid
    int shmid = shmget(key, size, 0666 | IPC_CREAT);
 
	return  (char*) shmat(shmid, NULL, 0);
}

void clean_share_mem(char* shm_addr, int shm_id) {
	shmdt(shm_addr);
	shmctl(shm_id , IPC_RMID , NULL);
}

int main()
{
    // ftok to generate unique key
    key_t key = 1234;
 
    // shmget returns an identifier in shmid
    int shmid = shmget(key,1024,0666|IPC_CREAT);
 
    // shmat to attach to shared memory
    char *str = (char*) shmat(shmid,(void*)0,0);
 
    printf("Data read from memory: %s\n",str);
     
    //detach from shared memory 
    shmdt(str);
   
    // destroy the shared memory
    shmctl(shmid,IPC_RMID,NULL);
    
    return 0;
}