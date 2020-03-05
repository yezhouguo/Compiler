#include "symbol.h"

FieldList varTable[TABLE_SIZE];
FunctionList funcTable[TABLE_SIZE];

int SemanticErrors[ERR_SIZE];
int psError;
int unComplFunc;

unsigned int hash_pjw(char* name)
{
    if(!name) return 0;
    unsigned int val=0, i;
    for(; *name; ++name)
    {
        val=(val << 2) + *name;
        if(i=val & ~0x3fff)
            val=(val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

void pushError(int errorNum)
{
    SemanticErrors[psError]=errorNum;
    psError++;
}
int typeEqual(Type a,Type b)
{
    //if(a->kind==b->kind && a->kind==ARRAY)
    //{
        Type p=a->u.array.elem;
        Type q=b->u.array.elem;
    //}
    if(a->kind!=b->kind) return 1;
    else switch(a->kind)
    {
        case BASIC:
            if(a->u.basicType!=b->u.basicType) return 1;
            else return 0;
            break;
        case ARRAY:
            while(p->kind==ARRAY && q->kind==ARRAY)
            {
                p=p->u.array.elem;
                q=q->u.array.elem;
            }
            return typeEqual(p,q);
            break;
        case STRUCTURE:
            return strcmp(a->u.structure->name,b->u.structure->name);
            break;
        default: return 1; break;
    }
}
FunctionList checknameFunc(char* name)
{
    // int hashTar=hash_pjw(name);
    // if(funcTable[hashTar] == NULL)
    //     return funcTable[hashTar];
    // else
    // {
    //     int checker=hashTar;
    //     while(checker!=hashTar-1)
    //     {
    //         if(funcTable[checker]==NULL) return NULL;
    //         else if(strcmp(funcTable[checker]->name,name)==0) return 0;
    //     }
    // }
    int hashTar=hash_pjw(name);
    int checker=hashTar;
    while(checker!=hashTar-1)
    {
        if(funcTable[checker]==NULL) return NULL;
        else if(strcmp(funcTable[checker]->name,name)==0) return funcTable[checker];
        checker=(checker+1)%TABLE_SIZE;
    }
    return NULL;
}
FieldList checknameVar(char* name)
{
    //if(varTable[105]) printf("105%d\n",varTable[105]->type->u.basicType);
    int hashTar=hash_pjw(name);
    int checker=hashTar;
    while(checker!=hashTar-1)
    {
        if(varTable[checker]==NULL) return NULL;
        else if(strcmp(varTable[checker]->name,name)==0) return varTable[checker];
        checker=(checker+1)%TABLE_SIZE;
    }
    return NULL;
}
int addVar(FieldList var)
{
    int hashTar=hash_pjw(var->name);
    if(varTable[hashTar] == NULL)
    {
        varTable[hashTar]=var;
        return 0;
    }
    else
    {
        //FieldList tmp=varTable[hashTar];
        int checker=hashTar;
        //if(varTable[105]) printf("105%d\n",varTable[105]->type->u.basicType);
        while(checker!=hashTar-1)//back to the beginning
        {
            if(varTable[checker]==NULL)//found an place
            {
                varTable[checker]=var;
                return 0;
            }
            else if(strcmp(var->name,varTable[checker]->name)==0) 
            {
                //ERR redefinition
                pushError(3);
                return -1;
            }          
            checker=(checker+1)%TABLE_SIZE;
        }
    }
    return -1;
}
int defineFunc(FunctionList func,int define)
{
    int hashTar=hash_pjw(func->name);
    if(funcTable[hashTar] == NULL)
    {
        //undefined
        FieldList p=func->tail;
        while(p)
        {
            if(p->name!=NULL && checknameVar(p->name)!=NULL) return -1;//existed param ???
            //addVar(p);
            p=p->tail;
        }
        p=func->tail;
        while(p)
        {
            addVar(p);
            p=p->tail;
        }    
        funcTable[hashTar]=func;
        if(define) funcTable[hashTar]->state=DEFINED;
        else 
        {
            funcTable[hashTar]->state=DECLARED;
            unComplFunc++;
        }
        return 0;
    }
    else
    {
        //FunctionList tmp=funcTable[hashTar];
        int checker=hashTar;
        while(checker!=hashTar-1)//back to the beginning
        {
            if(funcTable[checker]==NULL)//found an place
            {
                FieldList p=func->tail;
                while(p)
                {
                    if(checknameVar(p->name)!=NULL) return -1;//existed param ???
                    //addVar(p);
                    p=p->tail;
                }
                p=func->tail;
                while(p)
                {
                    if(p->name==NULL) break;
                    addVar(p);
                    p=p->tail;
                }    
                funcTable[hashTar]=func;
                if(define) funcTable[hashTar]->state=DEFINED;
                else 
                {
                    funcTable[hashTar]->state=DECLARED;
                    unComplFunc++;
                }
                return 0;
            }
            else if(strcmp(func->name,funcTable[checker]->name)==0) 
            {
                if(funcTable[checker]->state==DECLARED)//declared but not defined
                {
                    //check ret type
                    if(typeEqual(func->retType,funcTable[checker]->retType)!=0)
                    {
                        //ERR rettype mismatch
                        return -2;
                    }
                    else//check params
                    {
                        FieldList p,q;
                        p=func->tail;
                        q=funcTable[checker]->tail;
                        while(p!=NULL && q!=NULL)
                        {
                            if(typeEqual(p->type,q->type)!=0) return -2;//ERR param mismatch
                            p=p->tail;
                            q=q->tail;
                        }
                        if(p!=NULL || q!=NULL)
                        {
                            //ERR diffrent param num
                            return -2;
                        }
                    }
                    //ok to re-declare
                    if(define) 
                    {
                        funcTable[checker]->state=DEFINED;
                        unComplFunc--;
                    }
                    return 0;
                }
                else
                {
                    //ERR redefinition                    
                    return -3;
                }
            }
            checker=(checker+1)%TABLE_SIZE;
        }
    }
    return -1;
}

FieldList checkVar(FieldList var)
{
    int hashTar=hash_pjw(var->name);
    if(varTable[hashTar] == NULL)//undefined
    {
        //pushError(1);
    }
    else
    {
        FieldList tmp;
        int checker=hashTar;
        while(checker!=hashTar-1)//back to the beginning
        {
            tmp=varTable[checker];
            if(tmp!=NULL)//found something
            {
                if(strcmp(tmp->name,var->name)==0)//var?
                {
                    if(typeEqual(var->type,tmp->type)==0)
                    {
                        //this one?
                        return tmp;
                    }
                    else// ERR type mismatch
                    {            
                        pushError(10);//??? TODO
                        return NULL;
                    }
                }
                //name is not the target,next one
                checker=(checker+1)%TABLE_SIZE;
            }
            else  //missing(undefined)
            {
                
                return NULL;
            }
        }
    }
    return NULL;
}
FunctionList checkFunc(FunctionList var)
{
    int hashTar=hash_pjw(var->name);
    if(funcTable[hashTar] == NULL)//ERR undefined
    {
        pushError(1);
    }
    else
    {
        FunctionList tmp;
        int checker=hashTar;
        while(checker!=hashTar-1)//back to the beginning
        {
            tmp=funcTable[checker];
            if(tmp!=NULL)//found something
            {
                if(strcmp(tmp->name,var->name)==0)//var?
                {
                    if(tmp->state==DECLARED)
                    {
                        //ERR using undefined but declared func
                        return NULL;
                    }
                    else if(typeEqual(var->retType,tmp->retType)==0)//check rettype
                    {
                        FieldList p,q;
                        p=var->tail;
                        q=funcTable[checker]->tail;
                        while(p!=NULL && q!=NULL)
                        {
                            if(typeEqual(p->type,q->type)!=0) return NULL;
                            //ERR param mismatch
                            p=p->tail;
                            q=q->tail;
                        }
                        if(p!=NULL || q!=NULL)
                        {
                            //ERR diffrent param num
                            return NULL;
                        }
                        else return tmp;
                    }
                    else// ERR type mismatch
                    {            
                        pushError(10);//??? TODO
                        return NULL;
                    }
                }
                //name is not the target,next one
                checker=(checker+1)%TABLE_SIZE;
            }
            else  //missing(undefined),error
            {
                pushError(1);
                return NULL;
            }
        }
    }
    return NULL;
}
void addReadWrite()
{
    FunctionList read=(FunctionList)malloc(sizeof(struct FunctionList_));
    read->name="read";
    read->state=DEFINED;
    read->retType=(Type)malloc(sizeof(struct Type_));
    read->retType->kind=BASIC;
    read->retType->u.basicType=INT;
    read->linenum=-1;
    read->tail=NULL;
    defineFunc(read,1);

    FunctionList write=(FunctionList)malloc(sizeof(struct FunctionList_));
    write->name="write";
    write->state=DEFINED;
    write->retType=(Type)malloc(sizeof(struct Type_));
    write->retType->kind=BASIC;
    write->retType->u.basicType=INT;
    write->linenum=-1;
    FieldList writeArg=(FieldList)malloc(sizeof(struct FieldList_));;
    writeArg->tail=NULL;
    writeArg->name=NULL;
    writeArg->type=(Type)malloc(sizeof(struct Type_));
    writeArg->type->kind=BASIC;
    writeArg->type->u.basicType=INT;
    write->tail=writeArg;
    defineFunc(write,1);
}