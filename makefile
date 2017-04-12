CC=gcc
CFLAGS=-c -Wall
LFLAGS=-lcurses -lrt -lpthread -lpanel -lmosquitto

all: actual

actual: mycan.o lex.yy.o grammer.tab.o autogen.o mydisplay.o ojtimer.o sender.o lib.o
	$(CC) mycan.c lex.yy.c grammer.tab.c autogen.c mydisplay.c ojtimer.c sender.c lib.c $(LFLAGS) -o can

main.o: main.c
	$(CC) $(CFLAGS) main.c

ojtimer.o: ojtimer.c
	$(CC) $(CFLAGS) $(LFLAGS) ojtimer.c

lex.yy.o:
	$(CC) $(CFLAGS) lex.yy.c

grammer.tab.o:
	$(CC) $(CFLAGS) grammer.tab.c

autogen.o:
	$(CC) $(CFLAGS) autogen.c

mydisplay.o:
	$(CC) $(CFLAGS) $(LFLAGS) mydisplay.c

sender.o:
	$(CC) $(CFLAGS) $(LFLAGS) sender.c

lib.o:
	$(CC) $(CFLAGS) $(LFLAGS) lib.c


clean: 
	rm *.o can
