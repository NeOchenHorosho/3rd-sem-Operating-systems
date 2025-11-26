#include <iostream>
#include <unistd.h>
#include <fstream>
#include <spawn.h>
#include <string>
#include <cstring> 
#include <vector>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h> 

const int MAX_MESSAGE_SIZE = 20;
const int MAX_RECORD_NUM = 100;
const int MAX_SENDERS_NUM = 10;
const char* shm_name = "/shm_name"; 
const char* free_recs_sem_name = "/free_recs_sem"; 
const char* full_recs_sem_name = "/full_recs_sem";

struct Info
{
    int records_num;
    int head; 
    int tail; 
    bool stop_flag; 
    pthread_barrier_t init_barrier;
    pthread_mutex_t shm_mutex;
};

struct Message
{
    char message[MAX_MESSAGE_SIZE+1];    
};