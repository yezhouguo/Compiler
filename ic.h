#ifndef IC
#define IC

#include "node.h"
#include <stdio.h>

#define MAX_LINE 1000
typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;

struct Operand_ {
	enum {TMP, VARIABLE, CONSTANT, ADDRESS, FUNC, LABEL, O_ARRAY} kind;
	union {
		int var_no;
		int value;
		char* name;
	} u;
};

struct InterCode_ {
	enum { I_LABEL, I_FUNC, ASSIGNII, ADD, \
    SUB, MUL, DIV, I_ADDRESS, ASSIGNIP, ASSIGNPI, GOTO, \
    IFGOTO, RETURN, DEC, ARGV, ARGA, \
    CALL, PARAM, READ, WRITE } kind;
	union {
		struct { Operand op; } oneop;
		struct { Operand left, right; } assign;
		struct { Operand result, op1, op2; } twoop;
		struct { Operand op1, op2, op3; char* relop; } threeop;
		struct { Operand op1; int size; } dec;
		struct { Operand op1, op2; } call;
	} u;
};

char* printO(Operand op);
char* printIC(InterCode ic);
void translate_Program(Infonode *now,char* irFile);
void translate_ExtDefList(Infonode *now);
void translate_ExtDef(Infonode *now);
void translate_FunDec(Infonode *now);
void translate_CompSt(Infonode *now);
void translate_StmtList(Infonode *now);
void translate_Stmt(Infonode *now);
void translate_DefList(Infonode *now);
void translate_Def(Infonode *now);
void translate_DecList(Infonode *now);
void translate_Dec(Infonode *now);
int translate_Exp(Infonode *now, int *place);
int translate_Args(Infonode *now, int *place);
void translate_Cond(Infonode *now, int label_true, int label_false);

#endif
