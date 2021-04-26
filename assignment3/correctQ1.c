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
//#include "correct.h"
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
 
#define SIZE 30
#define NUMB_THREADS 6
#define PRODUCER_LOOPS 2

int dataSize, numProducers, numConsumers, totalJobsDone;

//starting clock
struct timespec start, stop;

//used to clean up threads
pthread_t *cthread;

//shared memory objects
int *totalJobs, *writePtr, *readPtr;

int shmid, shmid1, shmid2, shmid3, shmid4, shmid5, shmid6, shmid7;

//Pid of all children
pid_t allChildren;

//The global buffer struct
typedef struct {
    int filesize, childId, free, reading;
    sem_t lock;
} buildingBlock;

buildingBlock *globalBuffer;
 
typedef int buffer_t;
buffer_t buffer[SIZE];
int buffer_index;

typedef struct node {
   int printData;
   struct node* next;
   int procID;
} Node;

countSem *full_sem, *empty_sem, *fs, *es;
 
pthread_mutex_t buffer_mutex;

typedef struct {
    int val;
    sem_t gate;
    sem_t mutex;
} countSem;

void myPost(countSem *cSem) {
    sem_wait(&cSem->mutex);
    cSem->val += 1;
    if (cSem->val == 0) {
        sem_post(&cSem->gate);
    } sem_post(&cSem->mutex);
}

void myWait(countSem *cSem) {
    
}

/* InsertBuffer (int datasize = file database), (int id = childID),
(localpoint = where to add too) */
void insertBuffer(int datasize, int id, int localpoint) {
    /* sets the place on the buffer to = 1 meaning its not free
    and something is in it */
    globalBuffer[localpoint].free = 1;
    sem_wait(&globalBuffer[localpoint].lock);

    globalBuffer[localpoint].filesize = datasize;
    globalBuffer[localpoint].childId = id;
    sem_post(&globalBuffer[localpoint].lock);
}

buildingBlock dequeuebuffer(int readme) {
    sem_wait(&globalBuffer[readme].lock);
     globalBuffer[readme].free = 0;
     sem_post(&globalBuffer[readme].lock);
     return globalBuffer[readme];
}
  
void addtoList(Node* head_ref, Node* newNode) {
    Node* temp = (Node*) malloc(sizeof(Node));
    Node *last = head_ref;
    newNode->next = NULL;
    if (head_ref == NULL) {
        head_ref = newNode;
        return;
    }
    while(last->next != NULL) {
        last = last->next;
    }
    last->next = newNode;
    return;
}

int lengthList(Node* head) {
    int count;
    Node* current = (Node*) malloc(sizeof(Node));
    current = head;
    while(current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void sigintHandler(int sig_num) {
    
}
 
void insertbuffer(buffer_t value) {
    if (buffer_index < SIZE) {
        buffer[buffer_index++] = value;
    } else {
        printf("Buffer overflow\n");
    }
}
 
buffer_t dequeuebuffer() {
    if (buffer_index > 0) {
        return buffer[--buffer_index]; // buffer_index-- would be error!
    } else {
        printf("Buffer underflow\n");
    }
    return 0;
}

void sigintHandler(int sig_num) {
    printf("Ctrl C -- Clean Exit\n");
    sleep(10);

    int i;
    for (i=0; i< numConsumers; i++) {
        pthread_cancel(cthread[i]);
        pthread_join(cthread[i], NULL);
    }

    shmdt(globalBuffer);
    shmdt(full_sem);
    shmdt(empty_sem);
    shmdt(totalJobs);
    shmdt(readPtr);
    shmdt(writePtr);
    shmdt(buffer_mutex);

    shmctl(shmid, IPC_RMID, NULL);
    shmctl(shmid1, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);
    shmctl(shmid3, IPC_RMID, NULL);
    shmctl(shmid4, IPC_RMID, NULL);
    shmctl(shmid5, IPC_RMID, NULL);

    pthread_mutex_destroy(buffer_mutex);

    exit(0);
}
 
void producer(int numJobs, int childID) {
    int i = 0;
    
    //for number of jobs needed to make them
    while (i < numJobs) {
        //size of job
        dataSize = rand() % 900 + 100;

        //check if the global queue has space in it
        semWait(fs);
        insertBuffer(dataSize, childID, *writePtr);
        
        printf("Producer %d added %d to buffer\n", childID, dataSize);

        *writePtr = (*writePtr + 1) % 30;
        semPost(empty_sem);
        
        int mysleep = (rand() % 10);
        sleep(mysleep/10.0);
        i++
    }
    exit(0);
}

//replacement consumer function
void *consumerThread(void *cthread_n) {
    int thread_numb = *(int *) cthread_n;

}
 
void *producer(void *thread_n) {
    int thread_numb = *(int *)thread_n;
    buffer_t value;
    int i=0;
    while (i++ < PRODUCER_LOOPS) {
        sleep(rand() % 10);
        value = rand() % 100;
        semWait(fs);  //sem_wait(&full_sem);

        pthread_mutex_lock(&buffer_mutex); /* protecting critical section */
        insertbuffer(value);
        pthread_mutex_unlock(&buffer_mutex);

        semPost(es);  //sem_post(&empty_sem);
        printf("Producer %d added %d to buffer\n", thread_numb, value);
    }
    pthread_exit(0);
}
 
void *consumer(void *thread_n) {
    int thread_numb = *(int *)thread_n;
    buffer_t value;
    int i=0;
    while (i++ < PRODUCER_LOOPS) {
        semWait(es);  //sem_wait(&empty_sem);
        /* there could be race condition here, that could cause
           buffer underflow error */
        pthread_mutex_lock(&buffer_mutex);
        value = dequeuebuffer(value);
        pthread_mutex_unlock(&buffer_mutex);
        semPost(fs);  //sem_post(&full_sem);
        printf("Consumer %d dequeue %d from buffer\n", thread_numb, value);
   }
    pthread_exit(0);
}
 
int main(int argc, int **argv) {
    buffer_index = 0;

    //Code to use
      
    int numProducers = atoi(argv[1]);
    int numConsumers = atoi(argv[2]);

    //Signal Handle
    signal(SIGINT, sigintHandler);

    //Start clock
    clock_gettime(_POSIX_MONOTONIC_CLOCK, &start);
    

    int fullSemID = shmget(7999, 1024, 0666 | IPC_CREAT);
    int emptySemID = shmget(9999, 1024, 0666 | IPC_CREAT);

    fs = (countSem*)shmat(fullSemID, NULL, 0);
    es = (countSem*)shmat(emptySemID, NULL, 0);
 
    pthread_mutex_init(&buffer_mutex, NULL);
    semInit(fs, SIZE);
    semInit(es, 0);  //sem_init(&empty_sem

    pthread_t thread[NUMB_THREADS];
    int thread_numb[NUMB_THREADS];
    int i;
    for (i = 0; i < NUMB_THREADS; ) {
        thread_numb[i] = i;
        pthread_create(thread + i, // pthread_t *t
                       NULL, // const pthread_attr_t *attr
                       producer, // void *(*start_routine) (void *)
                       thread_numb + i);  // void *arg
        i++;
        thread_numb[i] = i;
        // playing a bit with thread and thread_numb pointers...
        pthread_create(&thread[i], // pthread_t *t
                       NULL, // const pthread_attr_t *attr
                       consumer, // void *(*start_routine) (void *)
                       &thread_numb[i]);  // void *arg
        i++;
    }
 
    for (i = 0; i < NUMB_THREADS; i++)
        pthread_join(thread[i], NULL);
 
    pthread_mutex_destroy(&buffer_mutex);
    sem_destroy(&full_sem);
    sem_destroy(&empty_sem);
 
    return 0;
}