#ifndef MYDISPLAY_H
#define MYDISPLAY_H

#include "autogen.h"
#include <time.h>
#include <unistd.h>
#include <signal.h>

extern void makeCANWin();
extern int updateVisibleMessages();


extern void *start_display(void* arg);
#endif
