%{
#include <stdio.h>
#include "grammer.tab.h"
#include "autogen.h"
#include <ncurses.h>


  // declare variables for signals, because they are broken into two tokens
  unsigned int bitStart;
  unsigned int length;
  float offset;
  float scale;
  unsigned char isSigned;
  unsigned char isLittleEndian;
  char *signalName;

%}


%union {
   float fp;
   long int4;
   char *str;

};


%token MSGBEGIN
%token <fp> NUMBER
%token PIPE
%token AT
%token DOTCOLON
%token SIGBEGIN
%token <str> STRING
%token SPACE
%token LEFTBRACKET
%token RIGHTBRACKET
%token LEFTPARAN
%token RIGHTPARAN
%token <str> PLUSMINUS
%token QUTOATION
%token COMMA
%token NOTHING

%token VERSION NS BS BU SG CM BA VAL

%type <fp> number
%type <str> strings
%%

all: 
	| all somethingelse
	| all message


message: msgb number strings DOTCOLON NUMBER strings { 
	printf("Message is %s Baud is %d\n", $3, (int) $2);
	addMessage($3, $6, $2, $5);
 }
       | message signal

number: NUMBER { $$ = $1; }

msgb : MSGBEGIN

signal: signalfixed signalconfig
	{
		
	}

strings: STRING
	| strings STRING
        | strings COMMA


signalfixed: SG strings DOTCOLON NUMBER PIPE NUMBER AT NUMBER PLUSMINUS LEFTPARAN NUMBER COMMA NUMBER RIGHTPARAN LEFTBRACKET NUMBER PIPE NUMBER
	{
	  
		signalName = $2;
		bitStart = (int) $4;
		length = (int) $6;
		scale = $11;
		offset = $13;
		isSigned = (*$9 == '+'? 1 : 0);
		isLittleEndian = $8;
		
		addSignal(signalName, bitStart, length, offset, scale, isSigned, isLittleEndian);
	}	 


signalconfig: RIGHTBRACKET strings
             | RIGHTBRACKET strings COMMA strings
               


somethingelse: NOTHING {printf("Found something else\n");}
	     
	     
	  	   
	
%%
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

main(int argc, char *argv[])
{
  init();
  
  yyin = fopen(argv[1], "r");
  printf("yyin is %d and argv1 is %s\n", yyin, argv[1]);
  yyparse();
  fclose(yyin); 
  sortById();
  // printAllInfo();

// printf("Testing _________");
  //printMessageInfo(getMessage(1970));
  //printMessageInfo(getMessage(2008));
  //  printMessageInfo(getMessage(2015));

  //  test();

  argc--;
  argv = &argv[1];
  runCan(argc, argv);
  
  clean();
}

yyerror(char *s)
{
	fprintf(stderr, "error I found: %s at line %d\n", s, yylineno);
	fprintf(stderr, "error is %s\n", yytext);
}
