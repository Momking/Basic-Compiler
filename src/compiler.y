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
#include <stdlib.h>
#include <string.h>
#include "../include/symbol.h"
#include "../include/ast.h"

int yylex();
void yyerror( char* );
void checkFun(node*, DataType);

#define YYDEBUG 1
extern int yydebug;
#define YY_DECL int yylex()
#define YY_FLEX_DEBUG 1
extern int yy_flex_debug; 

%}

%union {
    int intVal;
    char *idVal;
	char idChar;
	float floatVal;
	node* node;
}


%token BEG END BREAK
%token T_INT T_BOOL T_STRING T_FLOAT T_CHAR
%token<intVal> WRITE READ
%token DECL ENDDECL
%token<idVal> VAR 
%token<intVal>NUM
%token<floatVal>FLOAT
%token<idVal>STR
%token<idChar>CHAR
%token IF THEN ELSE ENDIF
%token LOGICAL_AND LOGICAL_NOT LOGICAL_OR
%token EQUALEQUAL LESSTHANOREQUAL GREATERTHANOREQUAL NOTEQUAL
%token FOR
%token WHILE
%token T F
%token MAIN RETURN

%left LOGICAL_OR
%left LOGICAL_AND
%left LOGICAL_NOT
%left EQUALEQUAL LESSTHANOREQUAL GREATERTHANOREQUAL NOTEQUAL '<' '>'
%left '+' '-'
%left '*' '/' '%'
%left BREAK

%type<node> Gid assign_stmt expr var_expr str_expr Gdecl Gdecl_list Gdecl_sec Glist stmt_list statement Prog write_stmt cond_stmt while_stmt Gdim dim_expr;

%%

	Prog: Gdecl_sec stmt_list 				 {  $$ = createNode("Prog", "start", NULL, $1, 0, NULL); 	$1->sibling = createNode("stmtBody", "stmtList", NULL, $2, 0, NULL);	print_tree($$);	}
		| Gdecl_sec BEG stmt_list END		 {	$$ = createNode("Prog", "start", NULL, $1, 0, NULL); 	$1->sibling = createNode("stmtBody", "stmtList", NULL, $3, 0, NULL);	print_tree($$); }
    	| stmt_list 						 {  $$ = createNode("Prog", "start", NULL, $1, 0, NULL);	print_tree($$);	}
		| BEG stmt_list END					 {	$$ = createNode("Prog", "start", NULL, $2, 0, NULL); 	print_tree($$);	}
    ;
		
	Gdecl_sec:	DECL Gdecl_list ENDDECL      {  $$ = createNode("decl", "decl", NULL, $2, 0, NULL);	}
		;
		
	Gdecl_list:								 {	$$ = NULL; 					}
			| 	Gdecl Gdecl_list			 { 	$$ = $1; $1->child = $2;	}
		;
		
	Gdecl 	:	T_INT Glist ';'			     { 	$$ = createNode("INT", "intDec", $2, NULL, 0, NULL);		checkFun($2, TYPE_INT); 	}
		|		T_FLOAT Glist ';'			 { 	$$ = createNode("FLOAT", "floatDec", $2, NULL, 0, NULL);	checkFun($2, TYPE_FLOAT); 	}
		|		T_CHAR Glist ';'			 { 	$$ = createNode("CHAR", "charDec", $2, NULL, 0, NULL);		checkFun($2, TYPE_CHAR); 	}
		|		T_STRING Glist ';'			 { 	$$ = createNode("STRING", "strDec", $2, NULL, 0, NULL);		checkFun($2, TYPE_STRING); 	}
		|		T_BOOL Glist ';'			 {	$$ = createNode("BOOL", "intDec", $2, NULL, 0, NULL);		checkFun($2, TYPE_BOOL);  	}
		;
		
	Glist 	:	Gid							 {  $$ = $1;	 				}
		|	Gid ',' Glist 					 {  $$ = $1; $1->sibling = $3;  }
		;
	
	Gid	:	VAR								 {  $$ = createNode($1, "var", NULL, NULL, 0, NULL);   	}
		|	VAR Gdim						 {  $$ = createNode($1, "array", NULL, $2, 0, NULL);   	}
		;

	Gdim:									 {	$$ = NULL; 											}
		|	'[' NUM ']' Gdim				 {	$$ = createNode("NUM", "int", $4, NULL, $2, NULL); 	}

	stmt_list:	/* NULL */					 {  $$ = NULL;				 			}
		|	statement stmt_list				 {	$$ = $1; $1->sibling = $2;  		}
		|	error ';' 						 {    	 				 				}
		;

	statement:	assign_stmt  ';'			 { 	$$ = $1;	}
		|	write_stmt ';'					 { 	$$ = $1;  	}
		|	cond_stmt 						 { 	$$ = $1;	}
		|   while_stmt						 {	$$ = $1;	}
		|   BREAK ';'					 	 {	$$ = createNode("break", "break", NULL, NULL, 0, NULL); }
		;

	write_stmt:	WRITE '(' expr ')' 			 {  $$ = createNode("WRITE", "write", NULL, $3, 0, NULL);   }
		| WRITE '(''"' str_expr '"'')'       { 	$$ = createNode("WRITE", "write", NULL, $4, 0, NULL);   }
		;
	
	assign_stmt:							 { 	$$ = NULL;	}
		|	var_expr '=' expr 		 		 { 	$$ = createNode("assign", "assign", NULL, $1, 0, NULL); $1->sibling = $3;	}
		;

	cond_stmt:	IF expr THEN stmt_list ENDIF 										{
																						$$ = createNode("condIf", "if", NULL, $2, 0, NULL);
																						$2->sibling = createNode("stmtList", "stmtList", NULL, $4, 0, NULL);
																					}
		|	IF expr THEN stmt_list ELSE stmt_list ENDIF 							{ 	
																						$$ = createNode("condIfElse", "ifelse", NULL, $2, 0, NULL);
																						node* s = createNode("stmtList", "stmtList", NULL, $6, 0, NULL);
																						$2->sibling = createNode("stmtList", "stmtList", s, $4, 0, NULL);
		 																			}
	    |   FOR '(' assign_stmt  ';'  expr ';'  assign_stmt ')' '{' stmt_list '}'   {   
																						if($3 == NULL) { $$ = createNode("condFor", "for", NULL, $5, 0, NULL); }
																						else { $$ = createNode("condFor", "for", NULL, $3, 0, NULL); $3->sibling = $5; }

																						if($7 == NULL) { $5->sibling = createNode("stmtList", "stmtList", NULL, $10, 0, NULL); }
																						else { $7->sibling = createNode("stmtList", "stmtList", NULL, $10, 0, NULL); $5->sibling = $7; }
		 																			}
		;

	while_stmt:  WHILE expr	'{' stmt_list '}'										{   
																						$$ = createNode("while", "while", NULL, $2, 0, NULL);	
																						$2->sibling = $4;
																					}

	expr	:	NUM 						{ 	$$ = createNode("NUM", "int", NULL, NULL, $1, NULL);  				}
		|	'-' NUM							{  	$$ = createNode("NUM", "int", NULL, NULL, -$2, NULL);				}
		|	FLOAT							{	$$ = createNode("FLOAT", "float", NULL, NULL, $1, NULL);			}
		|	'-' FLOAT						{	$$ = createNode("FLOAT", "float", NULL, NULL, -$2, NULL);			}
		|	STR								{	$$ = createNode($1, "string", NULL, NULL, 0, $1);					}
		|	CHAR							{	char temp[2] = {$1, '\0'}; $$ = createNode(temp, "char", NULL, NULL, 0, temp);}
		|	T								{	$$ = createNode("T", "int", NULL, NULL, 1, NULL);					}
		|	F								{	$$ = createNode("F", "int", NULL, NULL, 0, NULL);					}
		|	var_expr						{ 	$$ = $1;															}
		|	'(' expr ')'					{  	$$ = $2;															}
		|	expr '+' expr 					{ 	$$ = createNode("+", "op", NULL, $1, 0, NULL); $1->sibling = $3;	}
		|	expr '-' expr	 				{ 	$$ = createNode("-", "op", NULL, $1, 0, NULL); $1->sibling = $3; 	}
		|	expr '*' expr 					{ 	$$ = createNode("*", "op", NULL, $1, 0, NULL); $1->sibling = $3;	}
		|	expr '/' expr 					{ 	$$ = createNode("/", "op", NULL, $1, 0, NULL); $1->sibling = $3;	}
		|	expr '%' expr 					{  	$$ = createNode("%", "op", NULL, $1, 0, NULL); $1->sibling = $3;	}	
		|	expr '<' expr					{ 	$$ = createNode("<", "op", NULL, $1, 0, NULL); $1->sibling = $3;	}
		|	expr '>' expr					{  	$$ = createNode(">", "op", NULL, $1, 0, NULL); $1->sibling = $3;	}
		|	expr GREATERTHANOREQUAL expr	{ 	$$ = createNode(">=", "op", NULL, $1, 0, NULL); $1->sibling = $3; 	}
		|	expr LESSTHANOREQUAL expr		{  	$$ = createNode("<=", "op", NULL, $1, 0, NULL); $1->sibling = $3;	}
		|	expr NOTEQUAL expr				{ 	$$ = createNode("!=", "op", NULL, $1, 0, NULL); $1->sibling = $3; 	}
		|	expr EQUALEQUAL expr			{ 	$$ = createNode("==", "op", NULL, $1, 0, NULL); $1->sibling = $3; 	}
		|	LOGICAL_NOT expr				{ 	$$ = createNode("!", "op", NULL, $2, 0, NULL); 						}
		|	expr LOGICAL_AND expr			{ 	$$ = createNode("&&", "op", NULL, $1, 0, NULL); $1->sibling = $3;	}
		|	expr LOGICAL_OR expr			{ 	$$ = createNode("||", "op", NULL, $1, 0, NULL); $1->sibling = $3;	}
		;

	str_expr :  VAR                 		{   $$ = createNode($1, "var", NULL, NULL, 0, NULL);					}
                | str_expr VAR      		{ 	$$ = $1; $1->sibling = createNode($2, "var", NULL, NULL, 0, NULL);	}
				| VAR dim_expr				{	$$ = createNode($1, "array", NULL, $2, 0, NULL);					}
                ;
	
	var_expr:	VAR							{   $$ = createNode($1, "var", NULL, NULL, 0, NULL);			}
		|	VAR dim_expr					{   $$ = createNode($1, "array", NULL, $2, 0, NULL);        	}
		;

	dim_expr:								{	$$ = NULL;					}
		|	'[' expr ']' dim_expr			{	$$ = $2; $2->sibling = $4; 	}
%%

void yyerror ( char  *s) {
   fprintf (stderr, "%s\n", s);
}

void checkFun(node* a, DataType t){
	node* n = a;

	if(a == NULL) return;

	while(n != NULL)
	{
		if(n->child == NULL)
		{
			sUpdate(n->varName, t, 0, NULL);
		}
		else
		{
			node* temp = n->child;
			int* dimension = malloc(sizeof(int) * 10);
			memset(dimension, -1, sizeof(int) * 10);
			int i = 0;

			while(temp != NULL && i < 10)
			{
				dimension[i++] = temp->data.intValue;
				temp = temp->sibling;
			}

			sUpdate(n->varName, t, 1, dimension);

			free(dimension);
		}

		n = n->sibling;
	}
	
}

int main(){
	//yydebug = 1;
	//yy_flex_debug = 1;
	yyparse();
	printSym();
	return 0;
}
