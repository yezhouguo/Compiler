#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "node.h"

#define INT 1
#define FLOAT 2
#define TABLE_SIZE 16384
#define ERR_SIZE 20

extern Infonode* syntaxTree;
extern ErrorFlags ErrorF;

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct StructList_* StructList;
typedef struct FunctionList_* FunctionList;
struct Type_
{
    enum {BASIC,ARRAY,STRUCTURE} kind;//what kind
    union 
    {
        int basicType;//int or float?
        struct{Type elem;int size;} array;//is an array, elem name[size] elem can be a struct or array
        StructList structure;//is an struct 
    } u;
};
struct FieldList_
{
    char* name;
    Type type;
    int val;
    unsigned char assigned;
    FieldList tail;
};
struct StructList_
{
    /*struct A{
        sth
    }
    name = a sth= tail
    */
    char* name;//
    FieldList tail;
};
struct FunctionList_
{
    Type retType;
    char* name;
    FieldList tail;
    enum {UNDEFINED,DECLARED,DEFINED} state;
    int linenum;
};

int typeEqual(Type a,Type b);
FunctionList checknameFunc(char* name);
FieldList checknameVar(char* name);
int addVar(FieldList var);
int defineFunc(FunctionList var,int define);
FieldList checkVar(FieldList var);
FunctionList checkFunc(FunctionList var);
void addReadWrite();
#endif