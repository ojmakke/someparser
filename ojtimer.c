#include "ojtimer.h"

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>

/* Maximum callbacks */
#define MAXCB 5
/* delcare variable as array of pointers to function returning int */
int (* __ojcallbacks[MAXCB])();
timer_t __ojtimerid[MAXCB]; /* TimerID associated with callback function */
static int __ojIsInit;

/* Main timer handler */
void timer_handler(int sig, siginfo_t *si, void *uc)
{

  //printf("Caught signal %d\n", sig);
  
  timer_t *tidp;
  tidp = si->si_value.sival_ptr;
  //printf("Time rid is %lu\n", *tidp);
  {
    int ii;
    for(ii = 0; ii < MAXCB; ii++)
      {
	if(__ojtimerid[ii] == 0) continue;
	if(__ojtimerid[ii] == *tidp && __ojcallbacks[ii] != NULL)
	  {
	    printf("Timer id is %lu and index = %d \n", *tidp, ii);
	    __ojcallbacks[ii]();
	  }
      }
  }
  //printf("*sival_ptr = 0x%lx\n", (long) *tidp);
  // signal(sig, SIG_IGN);
  //print_siginfo(si);
}

int remove_timer(timer_t timerid)
{
  int ii;
  int ret = -1;
  for(ii = 0; ii < MAXCB; ii++)
    {
      if(__ojtimerid[ii] == timerid)
	{
	  __ojtimerid[ii] = 0;
	  __ojcallbacks[ii] = NULL;
	  ret = 0;
	  /* Add remove timer using SIG_IGN, and stop timer */
	}
    }
  return ret;
}


int ojTimerInit()
{
  int ii;
  //  printf("initializing\n");
  for(ii = 0; ii < MAXCB; ii++)
    {
      __ojcallbacks[ii] = NULL;
      __ojtimerid[ii] = 0;
    }
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = timer_handler;
  sigemptyset(&sa.sa_mask);
  int result = sigaction(SIGRTMIN, &sa, NULL);
  return result;
}

void register_threadedTimer(timer_t* timerid,
			    void (*function),
			    long milliseconds)
{
  struct sigevent sig;
    if(!__ojIsInit)
    {
      ojTimerInit();
      __ojIsInit = 1;
    }

  memset(&sig, 0, sizeof(struct sigevent));
  sig.sigev_notify = SIGEV_THREAD;
  sig.sigev_notify_function = function;
  sig.sigev_value.sival_int = SIGRTMIN+1;
 
  timer_create(CLOCK_MONOTONIC, &sig, timerid);
   
  //struct itimerspec in;
  struct itimerspec spec;
  long seconds = milliseconds / 1000; /* this is int divion, no remainder */
  long nseconds = (milliseconds - (seconds * 1000))*1000000;
  spec.it_interval.tv_nsec = nseconds;
  spec.it_interval.tv_sec = seconds;
  spec.it_value.tv_nsec = nseconds;
  spec.it_value.tv_sec = seconds;

  if(timer_settime(*timerid,0, &spec, NULL) == -1)
    {
      fprintf(stderr, "Error setting timer\n");
      //  return -1;
      return;
    }
  // printf("Threaded timer is created\n");
}

int register_timer(timer_t* timerid, int (* func)(void), long milliseconds)
{
  if(!__ojIsInit)
    {
      ojTimerInit();
      __ojIsInit = 1;
    }

  /* See if there is available timer */
  int ii, index;
  index = MAXCB + 1;
  for(ii = 0; ii < MAXCB; ii++)
    {
      if(__ojcallbacks[ii] == NULL && __ojtimerid[ii] == 0)
	{
	  index = ii;
	  break;
	}
    }
  if(index == MAXCB + 1)
    {
      fprintf(stdout, "Error. timers are full\n");
      return -1;
    }
  printf("Index is %d\n", index);

  struct sigevent sevp;
  sevp.sigev_notify = SIGEV_SIGNAL;
  sevp.sigev_signo = SIGRTMIN;
  sevp.sigev_value.sival_ptr = timerid;
  printf("Created timerid %lu\n", *timerid);
  if(timer_create(CLOCK_MONOTONIC, &sevp, timerid) == -1)
    {
      fprintf(stderr, "Error creating timer\n");
      return -1;
    }

  struct itimerspec spec;
  long seconds = milliseconds / 1000; /* this is int divion, no remainder */
  long nseconds = (milliseconds - (seconds * 1000))*1000000;
  spec.it_interval.tv_nsec = nseconds;
  spec.it_interval.tv_sec = seconds;
  spec.it_value.tv_nsec = nseconds;
  spec.it_value.tv_sec = seconds;

  if(timer_settime(*timerid,0, &spec, NULL) == -1)
    {
      fprintf(stderr, "Error setting timer\n");
      return -1;
    }

  /* Now store in globals */
  __ojtimerid[index] = *timerid;
  __ojcallbacks[index] = func;
  printf("Stored in index %d\n", index);
  return 0;
}


void handleCtrlC()
{
  printf("Terminating\n");
  fflush(stdout);
  exit(1);
}
