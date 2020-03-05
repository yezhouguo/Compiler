#ifndef SYMBOL_H
#define SYMBOL_H
#include "symbol.h"

Type Exp(Infonode* now,int* onlyRightValue);
int Args(Infonode* now,FunctionList thisFunc);
Type Specifier(Infonode* now);
Type StructSpecifer(Infonode* now);
FieldList DefList(Infonode* now);
FieldList Def(Infonode* now);
FieldList DecList(Infonode* now,Type givenType);
FieldList Dec(Infonode* now,Type givenType);
FieldList VarDec(Infonode* now,Type givenType);
void CompSt(Infonode* now);
void StmtList(Infonode* now);
void Stmt(Infonode* now);
void FunDec(Infonode* now,int define);
FieldList VarList(FieldList head,Infonode* now);
FieldList ParamDec(Infonode* now);
void Program(Infonode* now);
void ExtDefList(Infonode* now);
void ExtDef(Infonode* now);
void ExtDecList(Infonode* now,Type givenType);
#endif