/*
 *   This file is part of SIL Compiler.
 *
 *  SIL Compiler is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SIL Compiler is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SIL Compiler.  If not, see <http://www.gnu.org/licenses/>.
 */

%{
#include <stdio.h>
#include <string.h>

#include "../include/symbol.h"
#include "../include/ast.h"

#include "../bin/y.tab.h"

int 	Lineno;
int     wflag;
#define YY_DECL int yylex()
#define YY_FLEX_DEBUG 1

%}

%option noyywrap
digit	[0-9]
letter	[a-zA-Z]

%%

main	                 	    { return MAIN; }	
					
return 			                { return RETURN; }

begin				            { return BEG; }

end			                    { return END; }	

read				            { return READ; }

write							{ return WRITE; }

integer			                { return T_INT; }

string                          { return T_STRING; }

boolean				            { return T_BOOL; }

integer							{ return T_INT; }

float                           { return T_FLOAT; }

char                            { return T_CHAR; }

if				                { return IF; }

then				            { return THEN; }	

else				            { return ELSE; }	

endif				            { return ENDIF; }

for 				            { return FOR; }	

while                           { return WHILE; }

break                           { return BREAK; }

"=="				            { return EQUALEQUAL; }	

"<="				            { return LESSTHANOREQUAL; }

">="				            { return GREATERTHANOREQUAL; }

"!="				            { return NOTEQUAL; }

AND				                { return LOGICAL_AND; }	

OR				                { return LOGICAL_OR; }

NOT			                    { return LOGICAL_NOT; }

true			                { return T; }

false			                { return F; }

decl						    { return DECL; }

enddecl							{ return ENDDECL; }

{digit}+["."]{digit}+           { yylval.floatVal = atof(yytext); return FLOAT; }

{digit}+			            { yylval.intVal = atoi(yytext); return NUM; }

[']{letter}[']                  { yylval.idChar = yytext[1]; return CHAR; }

["][^\n]*["]                    { yylval.idVal = strdup(yytext + 1); 
                                  yylval.idVal[strlen(yytext) - 2] = '\0';  
                                  return STR; }

{letter}({letter}|{digit})*	    { yylval.idVal = strdup(yytext); return VAR; }

[-+*/=()><;,\{\}\[\]\%]         { return *yytext; }
			
["]                             { return '"'; }

'\n'                            {  }

[ \t]		    

.				               { printf("Error !!!"); exit(1); }		

%%

