/* file chopsticks.c
   version: $Revision$
   last modified by: $Author$ on $Date$
   purpose:

   Demonstrate how to construct a monitor using POSIX mutex
   and condition variables.  Also demonstrates how to prevent
   deadlock.
*/

#define _XOPEN_SOURCE 500
#define _REENTRANT
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include "chopsticks.h"

#define LEFT (i+NTHREADS-1)%NTHREADS /* philo to the left */
#define RIGHT (i+1)%NTHREADS         /* philo to the right */
#define THINKING 1                   /* assign values to states */
#define HUNGRY 2
#define EATING 3

/* local state and synchronization variables */

pthread_cond_t  CV[NTHREADS];    /* one per philosopher */
pthread_mutex_t M;               /* mutual exclusion for the monitor */
int             state[NTHREADS]; /* state of each philosopher */

/* internal functions */

int 
update_state (int i) {
  if (state[i] == HUNGRY
   && state[LEFT] != EATING
   && state[RIGHT] != EATING) {
     state[i] = EATING;
     pthread_cond_signal (&CV[i]);
  }
  return 0;
}

/* exported ("entry") functions */

void
chopsticks_init () {
  int i;
  pthread_mutex_init (&M, NULL);
  for (i = 0; i < NTHREADS; i++) {
    pthread_cond_init (&CV[i], NULL);
    state[i] = THINKING;
  }
}

void
chopsticks_take (int i) {
   pthread_mutex_lock (&M);      /* enter cs, lock mutex */
   state[i] = HUNGRY;            /* set philosopher's state to HUNGRY */
   update_state(i);              /* update_state philosopher */
   while (state[i] == HUNGRY)    /* loop while philosopher is hungry */
     pthread_cond_wait (&CV[i],&M);
   pthread_mutex_unlock (&M); /* exit cs, unlock mutex */
}

void
chopsticks_put (int i) {
  pthread_mutex_lock (&M);       /* enter cs, lock mutex */
  state[i] = THINKING;
  update_state (LEFT);           /* update_state neighbors */
  update_state (RIGHT);
  pthread_mutex_unlock (&M);     /* exit cs, unlock mutex */
}

