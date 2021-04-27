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
#include "correct.h"
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
 
pthread_mutex_t buffer_mutex;
/* initially buffer will be empty.  full_sem
   will be initialized to buffer SIZE, which means
   SIZE number of producer threads can write to it.
   And empty_sem will be initialized to 0, so no
   consumer can read from buffer until a producer
   thread posts to empty_sem */
sem_t full_sem;  /* when 0, buffer is full */
sem_t empty_sem; /* when 0, buffer is empty. Kind of
                    like an index for the buffer */

int fullSemID, emptySemID, semValue;

typedef struct node {
   int printData;
   int free;
   int procID;
   sem_t lock;
} Node;

Node* head;
Node* items;

// void addtoList(Node* head_ref, Node* newNode) {
//     Node* temp = (Node*) malloc(sizeof(Node));
//     Node* last = head_ref;
//     newNode->next = NULL;
//     if (head_ref == NULL) {
//         head_ref = newNode;
//         return;
//     }
//     while(last->next != NULL) {
//         last = last->next;
//     }
//     last->next = newNode;
//     return;
// }

void initializeList() {
    for (int i=0; i<25; i++) {
        items[i].free = 1;
    }
}

void addtoList() {
    int i = 0;
    while(items[i].free == 1) {
        i++;
    }
    sleep(rand() % 10);
    sem_wait(&items[i].lock);
    items[i].free = 0;
    items[i].printData = genRand();
    sem_post(&items[i].lock);
}

// int lengthList(Node* head) {
//     int count;
//     Node* current = (Node*) malloc(sizeof(Node));
//     current = head;
//     while(current != NULL) {
//         count++;
//         current = current->next;
//     }
//     return count;
// }

// Node* retrieveFromList(Node* head) {
//     Node* current = (Node*) malloc(sizeof(Node));
//     Node* temp = (Node*) malloc(sizeof(Node));
//     current = head;
//     while (current != NULL) {
//         current = current->next;
//     }
//     temp = current;
//     free(current);
//     return temp;
// }
 
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
 

int genRand() {
    int upper = 25; int lower = 0;
    int randNum = (rand() % (upper - lower + 1)) + lower;
}
 
void *producer(void *thread_n) {
    printf("Reached producer\n");
    fullSemID = shmget(0572, sizeof(countSem), IPC_CREAT | 0666);
    countSem *fs = (countSem*)shmat(fullSemID, NULL, 0);
    emptySemID = shmget(0573, sizeof(countSem), IPC_CREAT | 0666);
    countSem *es = (countSem*)shmat(fullSemID, NULL, 0);
    // int nodeID = shmget(6578, sizeof(Node), IPC_CREAT | 0666);
    // head = (Node*) shmat(nodeID, NULL, 0);
    int itemArrayID = shmget(7578, sizeof(Node)*25, IPC_CREAT | 0666);
    items = (Node*) shmat(itemArrayID, NULL, 0);

    int thread_numb = *(int *)thread_n;
    buffer_t value;

    printf("Value of fullsem (producer): val: %d gate: %d mutex: %d\n", fs->val, fs->gate, fs->mutex);
    Node* item = (Node*)malloc(sizeof(Node));
    int i=0;
    int producer_loop = genRand();
    while (i++ < producer_loop) {
        sleep(rand() % 10);
        value = rand() % 100;
        semWait(fs);  //sem_wait(&full_sem);

        pthread_mutex_lock(&buffer_mutex); /* protecting critical section */
        insertbuffer(value);
        insert
        pthread_mutex_unlock(&buffer_mutex);

        semPost(es);  //sem_post(&empty_sem);
        printf("Producer %d added %d to buffer\n", thread_numb, value);
    }
    pthread_exit(0);
}
 
void *consumer(void *thread_n) {
    printf("Reached consumer\n");
    fullSemID = shmget(0572, sizeof(countSem), IPC_CREAT | 0666);
    countSem *fs = (countSem*)shmat(fullSemID, NULL, 0);
    emptySemID = shmget(0573, sizeof(countSem), IPC_CREAT | 0666);
    countSem *es = (countSem*)shmat(fullSemID, NULL, 0);
    int nodeID = shmget(6578, sizeof(Node), IPC_CREAT | 0666);
    head = (Node*) shmat(nodeID, NULL, 0);
    int itemArrayID = shmget(7578, sizeof(Node)*25, IPC_CREAT | 0666);
    items = (Node*) shmat(itemArrayID, NULL, 0);
    
    int thread_numb = *(int *)thread_n;
    buffer_t value;

    //printf("Value of fullsem (consumer): val: %d gate: %d mutex: %d\n", fs->val, fs->gate, fs->mutex);

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
    
    fullSemID = shmget(0572, sizeof(countSem), IPC_CREAT | 0666);
    countSem *fs = (countSem*)shmat(fullSemID, NULL, 0);
    emptySemID = shmget(0573, sizeof(countSem), IPC_CREAT | 0666);
    countSem *es = (countSem*)shmat(fullSemID, NULL, 0);

    int nodeID = shmget(6578, sizeof(Node), IPC_CREAT | 0666);
    Node* head = (Node*) shmat(nodeID, NULL, 0);

    int itemArrayID = shmget(7578, sizeof(Node)*25, IPC_CREAT | 0666);
    items = (Node*) shmat(itemArrayID, NULL, 0);
    
    // Node* current = (Node*) malloc(sizeof(Node)*25);
    // current = head;
    // current->printData = genRand();
    // current->next = NULL;

    // printf("Reached this point 1\n");
    
    // for (int i=0; i<10; i++) {
    //     Node* node = (Node*) malloc(sizeof(Node));
    //     node->printData = i;
    //     addtoList(head, node);
    // }

    // int count = lengthList(head); 
    int i = 0;

    // Node* iterate = (Node*) malloc(sizeof(Node)*25);
    // iterate = head;
    // while (iterate != NULL) {
    //     printf("Node %d Data: %d\n", i, iterate->printData);
    //     i++;
    //     iterate = iterate->next;
    // }
    //printf("Reached this point 2\n");

    pthread_mutex_init(&buffer_mutex, NULL);
    semInit(fs, SIZE);
    semInit(es, 0);  //sem_init(&empty_sem)

    printf("Reached this point 3\n");

    pthread_t thread[NUMB_THREADS];
    int thread_numb[NUMB_THREADS];
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
    //sem_destroy(&fs->B1); sem_destroy(&fs->B2);
 
    return 0;
}