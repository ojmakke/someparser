#ifndef OJTIMER_H
#define OJTIMER_H

#include <stdio.h>
#include <signal.h>
#include <time.h>

//extern void init();

/* Register for a timer, specify the calling function and time to start
   You can register up to 5 timers for now, just becuase */
extern int register_timer(timer_t* timer, int (* function)(void), long milliseconds);

extern void register_threadedTimer(timer_t* timerid,
				  void (* function),
				  long milliseconds);

extern int remove_timer(timer_t timerid);


#endif
