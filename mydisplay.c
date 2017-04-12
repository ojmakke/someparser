#include "mydisplay.h"
#include "sender.h" 
#include "autogen.h"
    
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <panel.h>
#include "autogen.h"

WINDOW *winMain;

PANEL  *panels[2];
int panel_data[2];
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
  start_color();
  noecho();
  curs_set(0);
  // cbreak();
  winMain = create_newin(LINES, COLS, 1, 1);
  winSignals = create_newin(20, 40, LINES/2-10, COLS/2-20);
  panels[0] = new_panel(winMain);
  panels[1] = new_panel(winSignals);
  panel_data[0] = FALSE;
  panel_data[1] = FALSE;
  set_panel_userptr(&panels[0], &panels[1]);
  set_panel_userptr(&panels[1], &panels[0]);

  update_panels();
  hide_panel(panels[1]);
  
  keypad(winMain, TRUE);
  keypad(winSignals, TRUE);
  
  nodelay(winMain, TRUE);
  nodelay(winSignals, TRUE);
  init_pair(2, COLOR_CYAN, COLOR_BLUE);
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  //  wrefresh(winMain);
  
  memset(visibleIds, 0, 65536*sizeof(unsigned int));
  //  mvwprintw(winMain, 1, 1,"Initialized");
  
}


int updateVisibleMessages()
{
  // printAllInfo();
  //return 0;
  //  int line = 1;
  static int mwVisible = 1;
  static int msgBeginIndex = 0;
  mvwprintw(winMain, 0, 50, "Messags: %d", allMessages.messageCount);
 
  int ii = 1;
  int msgIndex = msgBeginIndex;
  int displayedCount = 0;
  static int cursorIndex = 1;
  struct MessageStruct *message;
  struct MessageStruct *selectedMessage;
  while(ii < LINES)
    {
      if(msgIndex >= allMessages.messageCount - 1) break;
      message = allMessages.messages[msgIndex++];

      if(message == NULL) continue;
      if(message->data == NULL) continue;
      if(message->delta > 100000.0 || message->delta < 0.00001)
	{
	  continue;
	}
      int  line = ii++;
      if(line == cursorIndex)
	{
	  selectedMessage = message;
	  wattron(winMain, COLOR_PAIR(2));
	  {
	    int kk;
	    if(message->signalNumber > 0 && message->signals != NULL)
	      {
		struct SignalStruct * signal;
		for(kk = 0; kk < message->signalNumber; kk++)
		  {
		    signal = message->signals[kk];
		    mvwprintw(winSignals, kk+1, 1, "%s\t\t\t", signal->signalName);
		    unsigned char* buf;
		    
		    buf = littleFromBigEndian(message->data,
					      signal->bitStart,
					      signal->length);
		    mvwprintw(winSignals,kk+1, 25, ": %d\t\t\t", *buf);
		    free(buf);
		  }
	      }
	    for(kk = message->signalNumber; kk < 20; kk++)
	      {
		mvwprintw(winSignals,kk,1, "\t\t\t\t\t\t\t");
	      }
	  }
	}
      else
	{
	  wattron(winMain, COLOR_PAIR(1));
	}
      mvwprintw(winMain, line, 5, "%4.1f\t\t ", message->delta);
      mvwprintw(winMain, line, 20,"%s\t\t\t", message->messageName);
      mvwprintw(winMain, line, 50, "%4d\t\t", message->id);
      mvwprintw(winMain, line, 57, "%3d\t\t", message->length);
      int jj;
      
      for(jj = 0; jj < message->length; jj++)
	{
	  mvwprintw(winMain, line, 65+jj*4, "%3d ", message->data[jj] ); 
	}
      //      return 0;
      //      line++;
      displayedCount++;
    }

  int ch;
  if(mwVisible == 1)
    ch = wgetch(winMain);
  else
    ch = wgetch(winSignals);

  
  // int ch = wgetch(winMain);
  //int ch;
  
  mvwprintw(winMain, cursorIndex, 2, " ");
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
  if(ch == KEY_DOWN && cursorIndex < displayedCount)
    {
      if(cursorIndex >= 10*LINES/12)
	{
	  msgBeginIndex++; // Move all messages when cursor is in middle
	}
      else
	{
	  cursorIndex++;
	}
      if(displayedCount + 1 < LINES)
	{
	   wclear(winMain);
	}
    }
  if(ch == KEY_UP && cursorIndex > 1)
    {
      if(cursorIndex > 4)
	{
	  cursorIndex--;
	}
      else if(msgBeginIndex > 0)
	{
	  msgBeginIndex--;
	}
      else
	{
	  cursorIndex--;
	}
    }

  if(ch == 'c')
    {
      wclear(winMain);
    }
  if(ch == 'u')
    {
      // u is for up
      struct MessageStruct up;
      memset(&up, 0, sizeof(up));
      up.data = (unsigned char*)malloc(8);
      up.id = 866;
      up.length = 8;
      up.data[1] = 20;
      sendCan(&up);
    }
  if(ch == 's' && mwVisible == 1)
    {
      //     hide_panel(panels[0]);
      show_panel(panels[1]);
      update_panels();
      mwVisible = 0;
    }
  else if(ch == 's' && mwVisible == 0)
    {
      hide_panel(panels[1]);
      update_panels();
      mwVisible = 1;
    }

  mvwprintw(winMain,cursorIndex,2, ">");
  doupdate();
  // wrefresh(winMain);
  //update_panels();
  //doupdate();
  return 0;
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

