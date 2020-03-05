
%option yylineno

%{
	#include <stdlib.h>
    #include <stdio.h>
    //#include "main.h"
    #include "syntax.tab.h"
    extern ErrorFlags ErrorF;
    int yycolumn = 1;
	#define YY_USER_ACTION \
	yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;
%}
digit [0-9]
octdigit [0-7]
hexdigit [0-9a-fA-F]
OCT 0(0|[0-7]{octdigit}*)
HEX 0x(0|[1-9a-fA-F]{hexdigit}*)
letter [_a-zA-Z]
ID {letter}({letter}|{digit}){0,31}
INT 0|([1-9]{digit}*){0,31}
FLOAT {digit}+\.{digit}+
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS \-
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
SPACE [ \t\r\n]*

%%
{SPACE} { }
{INT} {yylval.type_treenode = createInfoNode("INT",yytext,yylineno); return INT;} 
{FLOAT} {yylval.type_treenode = createInfoNode("FLOAT",yytext,yylineno); return FLOAT;}
{OCT} {int oct =strtol(yytext,NULL,0); sprintf(yytext,"%d",oct); yylval.type_treenode = createInfoNode("INT",yytext,yylineno); return INT;}
{HEX} {int hex =strtol(yytext,NULL,0); sprintf(yytext,"%d",hex); yylval.type_treenode = createInfoNode("INT",yytext,yylineno); return INT;}
{SEMI} {yylval.type_treenode = createInfoNode("SEMI","  ",yylineno); return SEMI;}
{COMMA} {yylval.type_treenode = createInfoNode("COMMA","  ",yylineno); return COMMA;}
{ASSIGNOP} {yylval.type_treenode = createInfoNode("ASSIGNOP","  ",yylineno);return ASSIGNOP;}
{RELOP} {yylval.type_treenode = createInfoNode("RELOP",yytext,yylineno);return RELOP;}
{PLUS} {yylval.type_treenode = createInfoNode("PLUS","  ",yylineno); return PLUS;}
{MINUS} {yylval.type_treenode = createInfoNode("MINUS","  ",yylineno); return MINUS;}
{STAR} {yylval.type_treenode = createInfoNode("STAR","  ",yylineno);return STAR;}
{DIV} {yylval.type_treenode = createInfoNode("DIV","  ",yylineno); return DIV;}
{AND} {yylval.type_treenode = createInfoNode("AND","  ",yylineno); return AND;}
{OR} {yylval.type_treenode = createInfoNode("OR","  ",yylineno); return OR;}
{DOT} {yylval.type_treenode = createInfoNode("DOT","  ",yylineno);return DOT;}
{NOT} {yylval.type_treenode = createInfoNode("NOT","  ",yylineno);return NOT;}
{TYPE} {yylval.type_treenode = createInfoNode("TYPE",yytext,yylineno); return TYPE;}
{LP} {yylval.type_treenode = createInfoNode("LP","  ",yylineno);return LP;}
{RP} {yylval.type_treenode = createInfoNode("RP","  ",yylineno); return RP;}
{LB} {yylval.type_treenode = createInfoNode("LB","  ",yylineno); return LB;}
{RB} {yylval.type_treenode = createInfoNode("RB","  ",yylineno); return RB;}
{LC} {yylval.type_treenode = createInfoNode("LC","  ",yylineno); return LC;}
{RC} {yylval.type_treenode = createInfoNode("RC","  ",yylineno); return RC;}
{STRUCT} {yylval.type_treenode = createInfoNode("STRUCT","  ",yylineno);return STRUCT;}
{RETURN} {yylval.type_treenode = createInfoNode("RETURN","  ",yylineno); return RETURN;}
{IF} {yylval.type_treenode = createInfoNode("IF","  ",yylineno); return IF;}
{ELSE} {yylval.type_treenode = createInfoNode("ELSE","  ",yylineno); return ELSE;}
{WHILE} {yylval.type_treenode = createInfoNode("WHILE","  ",yylineno); return WHILE;}
{ID} {yylval.type_treenode = createInfoNode("ID",yytext,yylineno); return ID;}
. {ErrorF.errorA=1; printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);}

%%