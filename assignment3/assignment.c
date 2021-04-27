#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "correct1.h"
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
 
#define SIZE 25
#define NUMB_THREADS 6
#define PRODUCER_LOOPS 2
 
typedef int buffer_t;
buffer_t buffer[SIZE];
int buffer_index;
 
pthread_mutex_t* buffer_mutex;

int fullSemID, emptySemID, semValue, itemArrayID;

int mutexID;

int childID, threadID;

int* numProd;
int* numCon;
int numConID;
int numProdID;

pid_t* pids;
pthread_t* threads;

typedef struct node {
   int printData;
   int available;
   pid_t procID;
   sem_t mutex;
} Node;

Node* items;

int* counter;

countSem* fs;
countSem* es;

void signalHandler(int sign_num) {
    printf("Ctrl+C detected. Termination will now begin\n");

    for(int i=0; i<numCon; i++) {
        pthread_cancel(numCon); 
    }

    shmdt(fs);
    shmctl(fullSemID, IPC_RMID, NULL);

    shmdt(es);
    shmctl(emptySemID, IPC_RMID, NULL);

    shmdt(items);
    shmctl(itemArrayID, IPC_RMID, NULL);

    exit(0);
}

void initializeList() {
    for (int i=0; i<25; i++) {
        items[i].available = 1;
    }
}

void addtoList(pid_t* child) {
    int i = 0;
    while(items[i].available != 1) {
        i++;
    }
    sleep(rand() % 10);
    sem_wait(&items[i].mutex);
    items[i].available = 0;
    items[i].printData = genRand();
    sem_post(&items[i].mutex);
}

pid_t* readItem() {
    int i = 0;
    while(items[i].available != 1) {
        i++;
    }
    sleep(rand() % 10);
    sem_wait(&items[i].mutex);
    items[i].available = 1;
    printf("Printer Output: %d", items[i].printData);
    sem_post(&items[i].mutex);
    return items[i].procID;
}

int quantityDone() {
    int count = 0;
    for (int i=0; i<25; i++) {
        if (items[i].available == 1) {
            count++;
        }
    } return count;
}

int genRand() {
    int upper = 25; int lower = 0;
    int randNum = (rand() % (upper - lower + 1)) + lower;
}
 
void *producer(pid_t* child) {
    numProdID = shmget(5472, sizeof(int), IPC_CREAT | 0666);
    numProd = (int*)shmat(numProdID, NULL, 0);
    int numProducers  = &numProd;
    
    fullSemID = shmget(0572, sizeof(countSem), IPC_CREAT | 0666);
    fs = (countSem*)shmat(fullSemID, NULL, 0);
    
    emptySemID = shmget(0573, sizeof(countSem), IPC_CREAT | 0666);
    es = (countSem*)shmat(fullSemID, NULL, 0);

    int itemArrayID = shmget(7578, sizeof(Node)*25, IPC_CREAT | 0666);
    items = (Node*) shmat(itemArrayID, NULL, 0);

    int counterID = shmget(7578, sizeof(Node)*25, IPC_CREAT | 0666);
    items = (Node*) shmat(itemArrayID, NULL, 0); 

    buffer_t value;

    int i=0;
    int numProdLoopID = shmget(5472, sizeof(int), IPC_CREAT | 0666);
    int* producer_loop = (int*)shmat(numProdLoopID, NULL, 0);
    producer_loop = genRand();

    while (i++ < producer_loop) {
        sleep(rand() % 10);
        value = rand() % 100;
        
        semWait(fs);

        addtoList(child);
        printf("Producer %d added %d to buffer: ", child, producer_loop);
        
        semPost(es);

    }
}
 
void *consumer(void *thread_n) {
    int numProdLoopID = shmget(5472, sizeof(int), IPC_CREAT | 0666);
    int* producer_loop = (int*)shmat(numProdLoopID, NULL, 0);

    numProdID = shmget(5472, sizeof(int), IPC_CREAT | 0666);
    numProd = (int*)shmat(numProdID, NULL, 0);
    int numProducers  = &numProd;

    fullSemID = shmget(0572, sizeof(countSem), IPC_CREAT | 0666);
    countSem *fs = (countSem*)shmat(fullSemID, NULL, 0);
    
    emptySemID = shmget(0573, sizeof(countSem), IPC_CREAT | 0666);
    countSem *es = (countSem*)shmat(fullSemID, NULL, 0);
    
    int itemArrayID = shmget(7578, sizeof(Node)*25, IPC_CREAT | 0666);
    items = (Node*) shmat(itemArrayID, NULL, 0);

    int childID = shmget(7879, sizeof(pid_t)*numProducers, IPC_CREAT | 0666);
    pids = (pid_t*) shmat(childID, NULL, 0);

    int mutexID = shmget(7885, sizeof(pthread_mutex_t), IPC_CREAT | 0666);
    buffer_mutex = (pthread_mutex_t*) shmat(mutexID, NULL, 0);

    numConID = shmget(0572, sizeof(int), IPC_CREAT | 0666);
    numCon = (int*)shmat(fullSemID, NULL, 0);
    int numConsumers  = &numCon;
    
    int thread_numb = *(int *)thread_n;
    buffer_t value;

    int i=0;
    sleep(1);
    while (quantityDone != 0) {
        
        semWait(es); 
        pthread_mutex_lock(buffer_mutex);

        pid_t* child = readItem();
        printf("Consumer %d added %d to buffer: ", thread_n, child, producer_loop);

        pthread_mutex_unlock(buffer_mutex);
        semPost(fs); 
   }
    pthread_exit(0);
}
 
int main(int argc, int **argv) {
    buffer_index = 0; int i = 0;
    numProd = atoi(argv[1]);
    numCon = atoi(argv[2]);
    
    numConID = shmget(9872, sizeof(int), IPC_CREAT | 0666);
    numCon = (int*)shmat(numConID, NULL, 0);
    int numConsumers  = &numCon;

    numProdID = shmget(5472, sizeof(int), IPC_CREAT | 0666);
    numProd = (int*)shmat(numProdID, NULL, 0);
    int numProducers  = &numProd;

    fullSemID = shmget(0572, sizeof(countSem), IPC_CREAT | 0666);
    fs = (countSem*)shmat(fullSemID, NULL, 0);

    emptySemID = shmget(0573, sizeof(countSem), IPC_CREAT | 0666);
    es = (countSem*)shmat(emptySemID, NULL, 0);

    itemArrayID = shmget(7578, sizeof(Node)*25, IPC_CREAT | 0666);
    items = (Node*) shmat(itemArrayID, NULL, 0);

    int childID = shmget(7879, sizeof(pid_t)*numProducers, IPC_CREAT | 0666);
    pids = (pid_t*) shmat(childID, NULL, 0);

    int mutexID = shmget(7885, sizeof(pthread_mutex_t), IPC_CREAT | 0666);
    buffer_mutex = (pthread_mutex_t*) shmat(mutexID, NULL, 0);

    signal(SIGINT, signalHandler);

    for (i=0; i<numProd; i++) {
        pids[i] = fork();
        if(pids[0] == 0) {
            producer(pids[i]);
        }
    }

    int threadID = shmget(7878, sizeof(pthread_t)*numConsumers, IPC_CREAT | 0666);
    threads = (pthread_t*) shmat(threadID, NULL, 0);

    initializeList();

    pthread_mutex_init(buffer_mutex, NULL);
    semInit(fs, SIZE);
    semInit(es, 0); 

    int thread_numb[numConsumers];
    for (i = 0; i < numCon; ) {
        thread_numb[i] = i;
        i++;
        thread_numb[i] = i;
        // playing a bit with thread and thread_numb pointers...
        pthread_create(&threads[i], // pthread_t *t
                       NULL, // const pthread_attr_t *attr
                       consumer, // void *(*start_routine) (void *)
                       &thread_numb[i]);  // void *arg
        i++;
    }
 
    for (i = 0; i < NUMB_THREADS; i++)
        pthread_join(threads[i], NULL);

        for(int i=0; i<numCon; i++) {
        pthread_cancel(numCon); 
    }

    shmdt(fs);
    shmctl(fullSemID, IPC_RMID, NULL);

    shmdt(es);
    shmctl(emptySemID, IPC_RMID, NULL);

    shmdt(items);
    shmctl(itemArrayID, IPC_RMID, NULL);

    exit(0);
 
    pthread_mutex_destroy(buffer_mutex);
    sem_destroy(&fs->gate); sem_destroy(&fs->mutex);
    sem_destroy(&es->gate); sem_destroy(&es->mutex);
 
    return 0;
}