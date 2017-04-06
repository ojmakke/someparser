#include "mydisplay.h"
#include "autogen.h"

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

WINDOW *winMain;
WINDOW *winSignals;

unsigned int visibleIds[65536]; /* Some ugly magic number */
typedef struct
{
  int columnOffset;
  int rowOffset;
  int currentColumn;
  int currentRow;
} CursorLocation;

CursorLocation cursorLocation;

WINDOW *create_newin(int height, int width, int starty, int startx)
{
  WINDOW *localWin;
  localWin = newwin(height, width, starty, startx);
  box(localWin, 0,0);
  wrefresh(localWin);
  memset(&cursorLocation, 0, sizeof(cursorLocation));
  return localWin;
}

void destroy_win(WINDOW *win)
{
  wborder(win, ' ', ' ' , ' ', ' ', ' ' , ' ' , ' ' , ' ');
  wrefresh(win);
  delwin(win);
}

void makeCANWin()
{
  initscr();
  noecho();
  refresh();
  
  // cbreak();
  winMain = create_newin(LINES, COLS, 1, 1);
  keypad(winMain, TRUE);
  nodelay(winMain, TRUE);
  
  init_pair(1, COLOR_CYAN, COLOR_BLUE);
  
  memset(visibleIds, 0, 65536*sizeof(unsigned int));
  //  mvwprintw(winMain, 1, 1,"Initialized");
  
}


int updateVisibleMessages()
{
  // printAllInfo();
  //return 0;
  //  int line = 1;
  static int msgBeginIndex = 0;
  static int loop = 0;
  mvwprintw(winMain, 0, 50, "Messags: %d %d", allMessages.messageCount, loop++);
 
  int ii = 1;
  int msgIndex = msgBeginIndex;

  while(ii < LINES)
    {
      if(msgIndex >= allMessages.messageCount - 1) break;
      struct MessageStruct *message = allMessages.messages[msgIndex++];

      if(message == NULL) continue;
      if(message->data == NULL) continue;
      if(message->delta > 100000.0 || message->delta < 0.00001)
	{
	  continue;
	}
      int  line = ii++;
      mvwprintw(winMain, line, 1, "%4.1f ", message->delta);
      mvwprintw(winMain, line, 15,"%s", message->messageName);
      mvwprintw(winMain, line, 45, "%4d", message->id);
      mvwprintw(winMain, line, 52, "%3d", message->length);
      int jj;
      
      for(jj = 0; jj < message->length; jj++)
	{
	  mvwprintw(winMain, line, 60+jj*4, "%3d ", message->data[jj] ); 
	}
      wrefresh(winMain);
      //      return 0;
      //      line++;
    }

  
  int ch = wgetch(winMain);
  mvwprintw(winMain, 0, 0, "%d %d", ch);
  wrefresh(winMain);
  
  if(ch == KEY_F(1))
    {
      endwin();
      exit(0);
    }
  if(ch == KEY_F(10) || ch == 'x')
    {
      endwin();
      exit(0);
    }
  
  return 0;
}

/* Uses the global allMessages structure to update the message at index */
void updateFromAllMessages(struct MessageStruct *message)
{
 
  // wprintw(winMain, "CAN Parser");
  static int line = 1;
  if(line > COLS) line = 1;
  if(message->delta < 10000.0)
    mvwprintw(winMain, line, 1, "%4.1f ", message->delta);
  mvwprintw(winMain, line, 15,"%s", message->messageName);
  mvwprintw(winMain, line, 45, "%4d", message->id);
  mvwprintw(winMain, line, 52, "%3d", message->length);

  int ii;
  for(ii = 0; ii < message->length; ii++)
    mvwprintw(winMain, line, 60+ii*4, "%3d ", message->data[ii] );

  line++;
  
  int ch = getch();
  if(ch != ERR)
    {
      printw("Returning exiting");
      delwin(winMain);
      exit(0);
    }
  if(ch == KEY_F(1))
    {
      endwin();
      exit(0);
    }
  
  wrefresh(winMain);

  return;
}


/* This is for threaded display */
void* start_display(void *arg)
{
  char* ret;
  makeCANWin();
  while(1)
    updateVisibleMessages();
  /* Not supposed to return */
  

  return ret;
}

