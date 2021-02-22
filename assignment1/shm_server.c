#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h> /* for exit */


/* 
 * shm-server - not sure of the origin of code segment.
 * Old source, not sure of the origin
 *  possibly: David Marshalls course : http://www.cs.cf.ac.uk/Dave/C/CE.html
 * or Steve Holmes : http://www2.its.strath.ac.uk/courses/c/
 */

/*
* TL;DR: of shm_server.c
* The server creates a shared memory and then generates a 1D array consisting of the alphabet. 
* It then waits for a "*" in the alphabet. Once the star is present, it will quit.
* The client code will add that star into the alphabet
*/

//shared memory cannot be larger than 27 bytes
#define SHMSZ     27


int 
main()
{
    char c;
    int shmid;
    key_t key;
    char *shm, *s;

    /*
     * We'll name our shared memory segment
     * This shared memory segment should not be the same key as anyone else on the server
     * If there is any error with the key value, change it
     * "5678".
     */
    key = 5678;

    /*
     * Create the segment.
     */
    if( (shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0 )
    {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if( (shm = shmat(shmid, NULL, 0)) == (char *) -1 )
    {
        perror("shmat");
        exit(1);
    }

    /*
     * Now put some things into the memory for the
     * other process to read.
     */
    s = shm;

    for( c = 'a'; c <= 'z'; c++ ) //the char variable "c" is incremented for each iteration add it to the next position of the s string
        *s++ = c; /* post fix */ //"*s++" increments the position of the pointer and then sets the given position the value of the char variable "c"
    *s = (char) NULL; //appends a \0 to the string to allow string manipulation and recognition

    /*
     * Finally, we wait until the other process 
     * changes the first character of our memory
     * to '*', indicating that it has read what 
     * we put there.
     */
    while( *shm != '*' )
        sleep(1);

    return 0;
}
