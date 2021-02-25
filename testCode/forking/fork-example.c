// What happens to threads in a process when fork() is called?

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>           /* for PRIxPTR */
#include <pthread.h>

// On Linux we need these for waitpid()
#include <sys/types.h>
#include <sys/wait.h>

// On Solaris, this cleans up format warnings from GCC
#define PID (int) getpid()


// process identity, for output only
char *me = "parent";    // or, "child"


/* function prototype for thread creation */
void * thread_func(void * arg);

/* data for threads */
struct thread_struct {
  pthread_t tid;        // system-assigned thread identifier
  int num;              // our own thread number
  int in;               // input data
  int out;              // output data
};

struct thread_struct bogus = { 0, 0, 0, -1 };

pthread_attr_t attr;    // thread attributes, not used here

struct thread_struct in[2];

void thread_generator(int low, int high, char *identifier)
{
  for (int i = low; i < high; i++)
    {
      in[i].tid = (pthread_t) (-1);     // assigned later
      in[i].num = i;
      in[i].in = i+1;
      in[i].out = -1;

      printf("[pid %d] %s %s: pthread_create(%d): trying\n",
        PID, me, identifier, in[i].num);

      // pthread_create(&in[i].tid, &attr, thread_func, &in[i]);
      int ret = pthread_create(&in[i].tid, NULL, thread_func, &in[i]);
        // in[i].tid = identifier of new thread
        // the new thread executes thread_func(&in[i])
        // pthread_create() returns an int, equal to 0 or error number

      if (ret == 0)
        {
          printf("[pid %d] %s %s: pthread_create(%d): ok\n",
            PID, me, identifier, in[i].num);
        }
      else
        {
          printf("[pid %d] %s %s: pthread_create(%d): failed: %s\n",
            PID, me, identifier, in[i].num, strerror(ret));
        }
    }
}

void thread_collector(int low, int high, char *identifier)
{
  for (int i = low; i < high; i++)
    {
      printf("[pid %d] %s %s: pthread_join(%d): trying\n",
        PID, me, identifier, in[i].num);

      void *out = &bogus;
      int ret = pthread_join(in[i].tid, &out);
        // out = exit status from pthread_exit()
        // pthread_join() returns an int, equal to 0 or error number

      if (ret == 0)
        {
          printf("[pid %d] %s %s: pthread_join(%d): ok, exit status = %p, out = %d\n",
            PID, me, identifier, in[i].num, out, in[i].out);
        }
      else
        {
          printf("[pid %d] %s %s: pthread_join(%d): failed: %s, exit status = %p, out = %d\n",
            PID, me, identifier, in[i].num, strerror(ret), out, in[i].out);
        }
    }
}


int main(int argc, char * argv[])
{
  printf("[pid %d] %s: &bogus = %p\n", PID, me, &bogus);
  for (int i = 0; i < 2; i++)
    { printf("[pid %d] %s: &in[%d] = %p\n", PID, me, i, &in[i]); }

  for (int i = 0; i < 2; i++)
    {
      in[i].tid = (pthread_t) (-1);     // assigned later
      in[i].num = i;
      in[i].in = -1;
      in[i].out = -1;
    }

  // create two threads
  thread_generator(0, 2, "main pre-fork");

  pid_t pid = fork();
  if (pid < 0)
    {
      printf("[pid %d] %s: main, fork(): failed: %s\n",
        PID, me, strerror(errno));
    }
  else if (pid == 0)
    {
      me = "child";
      printf("[pid %d] %s: main, parent is %d\n",
        PID, me, (int) getppid());
    }
  else
    {
      printf("[pid %d] %s: main, child is %d\n",
        PID, me, (int) pid);
    }

  for (int i = 0; i < 2; i++)
    { printf("[pid %d] %s: in[%d].tid = %" PRIxPTR "\n", PID, me, i, (uintptr_t) in[i].tid); }

  // join with two threads
  thread_collector(0, 2, "main");

  if (pid == 0)
    {
      printf("[pid %d] %s: main, sleeping 1\n", PID, me);
      sleep(1);
      printf("[pid %d] %s: main, exiting\n", PID, me);
      exit(0);
    }

  if (pid > 0)
    {
      printf("[pid %d] %s: main, waiting\n", PID, me);
      waitpid(pid, NULL, 0);
    }

  printf("[pid %d] %s: main, exiting\n", PID, me);
  return 0;
}


/* standard format for Pthreads */
void * thread_func(void * arg)
{
  struct thread_struct *p = arg;

  printf("[pid %d] %s: hello, world, this is thread %d, ID %" PRIxPTR " %" PRIxPTR "\n",
    PID, me, p->num, (uintptr_t) p->tid, (uintptr_t) pthread_self());

  printf("[pid %d] %s: thread %d, sleeping %d\n",
    PID, me, p->num, p->in);
  sleep(p->in);

  // do some work, so we can see if the thread actually ran
  p->out = 1;

  printf("[pid %d] %s: thread %d, exiting\n",
    PID, me, p->num);
  pthread_exit(arg);    // the thread's exit status
    // void return
}

