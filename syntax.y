%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "node.h"
    #include "lex.yy.c"
    extern Infonode* syntaxTree;
    extern int yylineno;
    extern void yyerror(const char *msg);
    extern ErrorFlags ErrorF;
%}

%union{
    Infonode* type_treenode;
}
%token <type_treenode> INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE
%type <type_treenode> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT UMINUS
%left DOT LB RB LP RP
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
Program : ExtDefList {
        $$ = createInfoNode("Program", "  ", @$.first_line);
        addNode($$, $1);
        syntaxTree = $$;
    }
    ;
ExtDefList : ExtDef ExtDefList {
        $$ = createInfoNode("ExtDefList", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
    }
    |  { $$ = createInfoNode("DefList", "`", @$.first_line); }
    ;
ExtDef : Specifier ExtDecList SEMI {
        $$ = createInfoNode("ExtDef", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | Specifier SEMI {
        $$ = createInfoNode("ExtDef", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
    }
    | Specifier FunDec CompSt {
        $$ = createInfoNode("ExtDef", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | Specifier FunDec SEMI {
        $$ = createInfoNode("ExtDef", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | error SEMI{
        ErrorF.errorB = 1;
    }
    ;

ExtDecList : VarDec {
        $$ = createInfoNode("ExtDecList", "  ", @$.first_line);
        addNode($$, $1);
    }
    | VarDec COMMA ExtDecList {
        $$ = createInfoNode("ExtDecList", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    ;
Specifier : TYPE {
        $$ = createInfoNode("Specifier", "  ", @$.first_line);
        addNode($$, $1);
    }
    | StructSpecifier {
        $$ = createInfoNode("Specifier", "  ", @$.first_line);
        addNode($$, $1);
    }
    ;
StructSpecifier : STRUCT OptTag LC DefList RC {
        $$ = createInfoNode("StructSpecifier", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
        addNode($$, $4);
        addNode($$, $5);
    }
    | STRUCT Tag {
        $$ = createInfoNode("StructSpecifier", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
    }
    ;
OptTag : ID {
        $$ = createInfoNode("OptTag", "  ", @$.first_line);
        addNode($$, $1);
    }
    |  { $$ = createInfoNode("OptTag", "`", @$.first_line); }
    ;
Tag : ID {
        $$ = createInfoNode("Tag", "  ", @$.first_line);
        addNode($$, $1);
    }
    ;

VarDec : ID {
        $$ = createInfoNode("VarDec", "  ", @$.first_line);
        addNode($$, $1);
    }
    | VarDec LB INT RB {
        $$ = createInfoNode("VarDec", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
        addNode($$, $4);
    }
    ;
FunDec : ID LP VarList RP {
        $$ = createInfoNode("FunDec", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
        addNode($$, $4);
    }
    | ID LP RP {
        $$ = createInfoNode("FunDec", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    ;
VarList : ParamDec COMMA VarList {
        $$ = createInfoNode("VarList", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | ParamDec {
        $$ = createInfoNode("VarList", "  ", @$.first_line);
        addNode($$, $1);
    }
    ;
ParamDec : Specifier VarDec {
        $$ = createInfoNode("ParamDec", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
    }
    ;

CompSt : LC DefList StmtList RC {
        $$ = createInfoNode("CompSt", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
		addNode($$, $4);
    }
    | LC error RC {
        ErrorF.errorB = 1;
    }
    ;
StmtList : Stmt StmtList {
        $$ = createInfoNode("StmtList", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
    }
    |  { $$ = createInfoNode("StmtList", "`", @$.first_line); }
    ;
Stmt : Exp SEMI {
        $$ = createInfoNode("Stmt", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
    }
    | CompSt {
        $$ = createInfoNode("Stmt", "  ", @$.first_line);
        addNode($$, $1);
    }
    | RETURN Exp SEMI {
        $$ = createInfoNode("Stmt", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
        $$ = createInfoNode("Stmt", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
        addNode($$, $4);
        addNode($$, $5);
    }
    | IF LP Exp RP Stmt ELSE Stmt {
        $$ = createInfoNode("Stmt", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
        addNode($$, $4);
        addNode($$, $5);
        addNode($$, $6);
        addNode($$, $7);
    } 
    | WHILE LP Exp RP Stmt {
        $$ = createInfoNode("Stmt", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
        addNode($$, $4);
        addNode($$, $5);
    }
    | error SEMI {
        ErrorF.errorB = 1;
    }
    ;

DefList : Def DefList {
        $$ = createInfoNode("DefList", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
    }
    |  { $$ = createInfoNode("DefList", "`", @$.first_line); }
    ;
Def : Specifier DecList SEMI {
        $$ = createInfoNode("Def", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | error SEMI{
        ErrorF.errorB = 1;
    }
    ;
DecList : Dec {
        $$ = createInfoNode("DecList", "  ", @$.first_line);
        addNode($$, $1);
    }
    | Dec COMMA DecList {
        $$ = createInfoNode("DecList", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    ;
Dec : VarDec {
        $$ = createInfoNode("Dec", "  ", @$.first_line);
        addNode($$, $1);
    }
    | VarDec ASSIGNOP Exp {
        $$ = createInfoNode("Dec", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    ;

Exp : Exp ASSIGNOP Exp {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | Exp AND Exp {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | Exp OR Exp {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | Exp RELOP Exp {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | Exp PLUS Exp {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | Exp MINUS Exp {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | Exp STAR Exp {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | Exp DIV Exp {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | LP Exp RP {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | MINUS Exp %prec UMINUS {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
    }
    | NOT Exp {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
    }
    | ID LP Args RP {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
        addNode($$, $4);
    }
    | ID LP RP {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | Exp LB Exp RB {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
        addNode($$, $4);
    }
    | Exp DOT ID {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | ID {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
    }
    | INT {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
    }
    | FLOAT {
        $$ = createInfoNode("Exp", "  ", @$.first_line);
        addNode($$, $1);
    }
    | LP error RP {
        ErrorF.errorB = 1;
    }
    ;

Args : Exp COMMA Args {
        $$ = createInfoNode("Args", "  ", @$.first_line);
        addNode($$, $1);
        addNode($$, $2);
        addNode($$, $3);
    }
    | Exp {
        $$ = createInfoNode("Args", "  ", @$.first_line);
        addNode($$, $1);
    }
    ;

%%
void yyerror(const char* msg){
    fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}