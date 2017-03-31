#include "mydisplay.h"
#include <ncurses.h>

WINDOW *winMain;
WINDOW *winSignals;

void makeCANWin()
{
  initscr();
  printw("CAN Parser");
  refresh();
}


/* Uses the global allMessages structure to update the message at index */
void updateFromAllMessages(int index)
{


  return;
}
