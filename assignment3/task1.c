#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <correct.h>
 
#define SIZE 5
#define NUMB_THREADS 6
#define PRODUCER_LOOPS 2 //should be random in task

/*      
    Tasl 1:
        Three shared memories for the following variables
            Global array, empty_sem, full_sem, and buffer mutex
        Sub-task 1: Make prod_con.c work with shared memory (of variables) and given processes 
            - You can use queue instead of a linked list. Any data structure can be used,
            linked lists are just the most efficient
            - All counting semaphore implementations must use the custom semaphore
            implementations from assignment 2, except for binary semaphores. Those can use
            sem_open 
    Sub-task 2: [Pending](?)
    Task 3: Signal handling and termination
        - Graceful termination 
            Processes and Threads are killed
            Deallocate shared memory
            Destroying semaphores
        - Calling exit(0)
        - What to do
            Make global var (boolean killflag)
*/
 
typedef int buffer_t; //buffer will now contain node structs and not ints
buffer_t buffer[SIZE];
int buffer_index;
int numThreads;
int numProc;
 
pthread_mutex_t buffer_mutex; //must be in shared memory as well
sem_t full_sem;  //must be in shared memory, this will be a struct of three elements from ass2
sem_t empty_sem; //must be in shared memory, this will be a struct of three elements from ass2

//contains processes as well
struct Node {
   char* printData;
   struct node *next;
   int procID;
   int procSize;
};

int linkedNums = 0;
struct Node* head = NULL;
 
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
 
 
void *producer(void *thread_n) {
    int thread_numb = *(int *)thread_n;
    buffer_t value;
    int i=0;
    int semvalue;
    while (i++ < PRODUCER_LOOPS) { //edit this to: "&& (!killflag)" to check for signal signaling
        sleep(rand() % 10);
        value = rand() % 100;
        sem_wait(&full_sem); 
        sem_getvalue(&full_sem, &semvalue);
        printf("Producer %d full_sem value: %d\n", thread_numb, semvalue);
        // sem=0: wait. sem>0: go and decrement it
        /* possible race condition here. After this thread wakes up,
           another thread could aqcuire mutex before this one, and add to list.
           Then the list would be full again
           and when this thread tried to insert to buffer there would be
           a buffer overflow error */
        pthread_mutex_lock(&buffer_mutex); /* protecting critical section */

        printf("linkedNums: %d", linkedNums);
        if (linkedNums == 0) {
            head = (struct Node*)malloc(sizeof(struct Node));
            printf("Input data into print queue: ");
            scanf("%s", head->printData);
            struct Node* node = (struct Node*)malloc(sizeof(struct Node));
            head->next = node;
        }
        else {
            
        }

        insertbuffer(value);
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&empty_sem); // post (increment) emptybuffer semaphore
        sem_getvalue(&empty_sem, &semvalue);
        printf("Producer %d after empty_sem value: %d\n", thread_numb, semvalue);
        printf("Producer %d added %d to buffer\n", thread_numb, value);
    }
    /* add exit(0) for producer process termination from signal handling
        could also run SIGKILL(PID) for the child

        For thread cancelling look at thread_cancel1.c in self-study
    */
    pthread_exit(0);
}
 
void *consumer(void *thread_n) {
    int thread_numb = *(int *)thread_n;
    buffer_t value;
    int i=0;
    int semvalue;
    /*
        While loop will be changed to: while(true) to run infinitely
    */
    while (i++ < PRODUCER_LOOPS) {
        sem_wait(&empty_sem);
        sem_getvalue(&empty_sem, &semvalue);
        printf("Consumer %d empty_sem value: %d\n", thread_numb, semvalue);
        /* there could be race condition here, that could cause
           buffer underflow error */
        pthread_mutex_lock(&buffer_mutex);
        
        value = dequeuebuffer(value);
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&full_sem); // post (increment) fullbuffer semaphore
        sem_getvalue(&full_sem, &semvalue);
        printf("Consumer %d after empty_sem value: %d\n", thread_numb, semvalue);
        printf("Consumer %d dequeue %d from buffer\n", thread_numb, value);
   }
    pthread_exit(0);
}
 
int main(int argc, int **argv) {
    numProc = atoi(argv[1]); numThreads = atoi(argv[2]);

    //exit(0);

    buffer_index = 0;  
 
    pthread_mutex_init(&buffer_mutex, NULL);
    sem_init(&full_sem, // sem_t *sem
             0, // int pshared. 0 = shared between threads of process,  1 = shared between processes
             SIZE); // unsigned int value. Initial value
    sem_init(&empty_sem, 0, 0);
    
    pthread_t thread[numThreads];
    int thread_numb[numThreads];
    int i;
    for (i = 0; i < numThreads; ) {
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
 
    for (i = 0; i < numThreads; i++)
        pthread_join(thread[i], NULL);
 
    pthread_mutex_destroy(&buffer_mutex);
    sem_destroy(&full_sem);
    sem_destroy(&empty_sem);
 
    return 0;
}