%{
#include <stdio.h>
#include "grammer.tab.h"


int valid = 0;


%}



%%
" BO_ "		valid = 0;
"BO_ "		{ valid = 1; return MSGBEGIN;}
"BA_ "		valid = 0;
"VAL_"		valid = 0;
"CM_"		valid = 0;

[-]?[0-9]+[.]?[0-9]+? 		{ 
				  if(valid == 1)
					{
						yylval.fp =atof(yytext);
						return NUMBER;
					}
				}

":" 		{ 
			if(valid == 1){
				return DOTCOLON; }
		}


"NS_ :"	 	{ valid = 0; }
"BS_:"		{ valid = 0; }
"BU_:" 		{ valid = 0; }
"SG_"	 	{ if(valid == 1){ return SG; }}
"|"		{ if(valid == 1) {return PIPE; }}
"\("		{ if(valid == 1) {return LEFTPARAN; }}
"\)" 		{ if(valid == 1) {return RIGHTPARAN; }}
"\[" 		{ if(valid == 1) {return LEFTBRACKET; }}
"\]"		{ if(valid == 1) {return RIGHTBRACKET; }}
","		{ if(valid == 1) {return COMMA; }}
[+-]		{ if(valid == 1) {yylval.str = strdup(yytext); return PLUSMINUS; }}
"@"		{ if(valid == 1) {return AT; }}
[a-zA-Z"][a-zA-Z0-9_/^.\-=%$!#"]* {	if(valid ==1 ) { yylval.str = strdup(yytext); return STRING; }}
. 		

%%
int yywrap()
{
	return 1;
}
