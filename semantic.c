#include "semantic.h"
#include <stdio.h>
//git f1ca1af
//Type bufferType;
//FieldList bufferField;
//FunctionList bufferFunction;
struct {Type curFuncRet;int valid;} FuncReturnType;
int unnamedStruct;
int stMbmDef;
extern int unComplFunc;
extern FieldList varTable[TABLE_SIZE];
extern FunctionList funcTable[TABLE_SIZE];

void ErrMsg(int ErrNo,int linenum,char* errInfo)
{
    ErrorF.errorC=1;
    switch (ErrNo)
    {
        case 1: printf("Error type %d at Line %d: Undefined variable \"%s\".\n",ErrNo,linenum,errInfo);
            break;
        case 2: printf("Error type %d at Line %d: Undefined function \"%s\".\n",ErrNo,linenum,errInfo);
            break;
        case 3: printf("Error type %d at Line %d: Redefined variable \"%s\"\n",ErrNo,linenum,errInfo);
            break;
        case 4: printf("Error type %d at Line %d: Redefined function \"%s\"\n",ErrNo,linenum,errInfo);
            break;
        case 5: printf("Error type %d at Line %d: Type mismatched for assignment.\n",ErrNo,linenum);
            break;
        case 6: printf("Error type %d at Line %d: The left-hand side of an assignment must be a variable.\n",ErrNo,linenum);
            break;
        case 7: printf("Error type %d at Line %d: Type mismatched for operands.\n",ErrNo,linenum);
            break;
        case 8: printf("Error type %d at Line %d: Type mismatched for return.\n",ErrNo,linenum);
            break;
        case 9: printf("Error type %d at Line %d: Function \"%s\" is not applicable for arguments.\n",ErrNo,linenum,errInfo);
            break;
        case 10: printf("Error type %d at Line %d: \"%s\" is not an array.\n",ErrNo,linenum,errInfo);
            break;
        case 11: printf("Error type %d at Line %d: \"%s\" is not a function.\n",ErrNo,linenum,errInfo);
            break;
        case 12: printf("Error type %d at Line %d: \"%s\" is not an integer.\n",ErrNo,linenum,errInfo);
            break;
        case 13: printf("Error type %d at Line %d: Illegal use of '.'.\n",ErrNo,linenum);
            break;
        case 14: printf("Error type %d at Line %d: Non-existent field \"%s\".\n",ErrNo,linenum,errInfo);
            break;
        case 151: printf("Error type 15 at Line %d: Redefined field \"%s\".\n",linenum,errInfo);
            break;
        case 152: printf("Error type 15 at Line %d: Initializing a field when defining a structure.\n",linenum);
            break;
        case 16: printf("Error type %d at Line %d: Duplicated name \"%s\".\n",ErrNo,linenum,errInfo);
            break;
        case 17: printf("Error type %d at Line %d: Undefined structure \"%s\".\n",ErrNo,linenum,errInfo);
            break;
        case 18: printf("Error type %d at Line %d: Undefined function \"%s\"\n",ErrNo,linenum,errInfo);
            break;
        case 19: printf("Error type %d at Line %d: Inconsistent declaration of function \"%s\"\n",ErrNo,linenum,errInfo);
            break;
        default:
            break;
    }
    //printf("SEMERR %d\n",ErrNo);
}
Type Exp(Infonode* now,int* onlyRightValue)
{
    Type ret;
    if(strcmp(now->nodechild->name,"Exp")==0)
    {
        if(strcmp(now->nodechild->nodebrothers->name,"LB")==0)
        {
            int jrl=0;
            ret=Exp(now->nodechild,&jrl);
            if(ret->kind!=ARRAY)
            {
                ErrMsg(10,now->nodechild->line,now->nodechild->nodechild->value);      //only Node ID carries the name of variable
                return NULL;
            }
            Type arrDim=Exp(now->nodechild->nodebrothers->nodebrothers,&jrl);
            if(!arrDim) return NULL;
            if (arrDim->kind==BASIC && arrDim->u.basicType==INT)//                
                return ret->u.array.elem;
            else
            {
                ErrMsg(12,now->nodechild->line,now->nodechild->nodebrothers->nodebrothers->nodechild->value);
                return NULL;
            }
        }
        else if(strcmp(now->nodechild->nodebrothers->name,"DOT")==0)
        {
            int jrl=0;
            Type Stct=Exp(now->nodechild,&jrl);
            if(!Stct) return NULL;
            if(Stct->kind!=STRUCTURE)
            {
                ErrMsg(13,now->nodechild->line,now->nodechild->nodechild->value);
                return NULL;
            }
            else
            {
                FieldList stctMbm=Stct->u.structure->tail;
                char waitMbm[32];
                strcpy(waitMbm,now->nodechild->nodebrothers->nodebrothers->value);
                FieldList finder=stctMbm;
                while(finder)
                {
                    if(strcmp(finder->name,waitMbm)==0)
                    {
                        ret=(Type)malloc(sizeof(struct Type_));
                        memcpy(ret,finder->type,sizeof(struct Type_));
                        return ret;
                    }
                    finder=finder->tail;
                }
                if(!finder)
                {
                    ErrMsg(14,now->nodechild->line,now->nodechild->nodebrothers->nodebrothers->value);
                    return NULL;
                }
            }
        }
        else
        {//= + - * /
            int orvl=0,orvr=0; //onlyRightValue of left/right side
            Type tl=Exp(now->nodechild,&orvl);
            Type tr=Exp(now->nodechild->nodebrothers->nodebrothers,&orvr);
            //struct assign?
            if(!tl || !tr) return NULL;
            char op[10];
            strcpy(op,now->nodechild->nodebrothers->name);
            if(strcmp(op,"ASSIGNOP")==0)
            {
                *onlyRightValue=1;
                if(orvl==1)
                {
                    ErrMsg(6,now->nodechild->line,now->nodechild->value);
                    return NULL;
                }
                if(typeEqual(tl,tr)==0) return tl;
                else
                {
                    ErrMsg(5,now->nodechild->line,now->nodechild->value);
                    return NULL;
                }
            }
            else
            {
                if(typeEqual(tl,tr)==0 && tl->kind==BASIC)
                {
                    *onlyRightValue=1;
                    if(strcmp(op,"AND")==0 || strcmp(op,"OR")==0)
                    {
                        if(tl->u.basicType==FLOAT)
                        {//5.4 & 6.2
                            ErrMsg(7,now->nodechild->line,now->nodechild->value);
                            return NULL;
                        }
                    }
                }
                else
                {
                    ErrMsg(7,now->nodechild->line,now->nodechild->value);
                    return NULL;
                }
                if(strcmp(op,"RELOP")==0) tl->u.basicType=INT;//5.4 > 3.2 = 1(int)
                // otherwise problem may occur in cases like {if(5.4 > 3.2)}
                return tl;
            }
        }
    }
    else if(strcmp(now->nodechild->name,"LP")==0) 
    {
        *onlyRightValue=1;
        return Exp(now->nodechild->nodebrothers,onlyRightValue);
    }
    else if(strcmp(now->nodechild->name,"MINUS")==0)
    {
        *onlyRightValue=1;
        Type t=Exp(now->nodechild->nodebrothers,onlyRightValue);
        if(!t) return NULL;
        if(t->kind!=BASIC)
        {
            ErrMsg(7,now->nodechild->line,now->nodechild->value);
            return NULL;
        }
        return t;
    }
    else if(strcmp(now->nodechild->name,"NOT")==0)
    {
        *onlyRightValue=1;
        Type t=Exp(now->nodechild->nodebrothers,onlyRightValue);
        if(!t) return NULL;
        if(t->kind==BASIC && t->u.basicType==INT) return t;
        else
        {
            ErrMsg(7,now->nodechild->line,now->nodechild->value);
            return NULL;
        }
    }
    else if(strcmp(now->nodechild->name,"ID")==0)
    {
        if(!now->nodechild->nodebrothers)
        {
            //id is not a func
            FieldList fret=checknameVar(now->nodechild->value);
            if(fret==NULL)
            {
                ErrMsg(1,now->nodechild->line,now->nodechild->value);
                return NULL;
            }
            else return fret->type;
        }
        else if(strcmp(now->nodechild->nodebrothers->nodebrothers->name,"RP")==0)
        {
            //func without param
            FunctionList fcret=checknameFunc(now->nodechild->value);
            if(fcret==NULL)
            {
                if(checknameVar(now->nodechild->value))
                {
                    ErrMsg(11,now->nodechild->line,now->nodechild->value);
                    return NULL;
                }
                else
                {
                    ErrMsg(2,now->nodechild->line,now->nodechild->value);
                    return NULL;
                } 
            }
            else if(fcret->state!=DEFINED)
            {
                ErrMsg(18,now->nodechild->line,now->nodechild->value);
                return NULL;
            }
            else 
            {
                *onlyRightValue=1;
                return fcret->retType;
            }
        }
        else
        {
            //func with param
            FunctionList fcret=checknameFunc(now->nodechild->value);
            if(fcret==NULL)
            {
                if(checknameVar(now->nodechild->value))
                {
                    ErrMsg(11,now->nodechild->line,now->nodechild->value);//linenum of LP
                    return NULL;
                }
                else
                {
                    ErrMsg(2,now->nodechild->line,now->nodechild->value);
                    return NULL;
                } 
            }
            else if(fcret->state!=DEFINED)
            {
                ErrMsg(18,now->nodechild->line,now->nodechild->value);
                return NULL;
            }
            else if(Args(now->nodechild->nodebrothers->nodebrothers,fcret)!=0)
            {
                ErrMsg(9,now->nodechild->line,now->nodechild->value);
                return NULL;
            }
            else 
            {
                *onlyRightValue=1;
                return fcret->retType;
            }
        }
    }
    else if(strcmp(now->nodechild->name,"INT")==0)
    {
        ret=(Type)malloc(sizeof(struct Type_));
        ret->kind=BASIC;
        ret->u.basicType=INT;
        *onlyRightValue=1;
        return ret;
    }
    else if(strcmp(now->nodechild->name,"FLOAT")==0)
    {
        ret=(Type)malloc(sizeof(struct Type_));
        ret->kind=BASIC;
        ret->u.basicType=FLOAT;
        *onlyRightValue=1;
        return ret;
    }
    return NULL;//should not reach here   
}
int Args(Infonode* now,FunctionList thisFunc)
{
    Infonode* p=now->nodechild;
	FieldList param=thisFunc->tail;
    int orv=0;
    Type givenPm=Exp(p,&orv);
    if(!givenPm) return -1;
    while(param && p)
    {
        if(typeEqual(givenPm,param->type)!=0) return -1;
        if(param->tail) param=param->tail;
        else param=NULL;
        if(p->nodebrothers)
        {
            p=p->nodebrothers->nodebrothers;
            givenPm=Exp(p->nodechild,&orv);
            if(!givenPm) return -1;
            p=p->nodechild;
        }
        else p=NULL;
    }
    if(param || p)
        return -1;
    return 0;
}

Type Specifier(Infonode* now) 
{
	Type ret;
	if(strcmp(now->nodechild->name, "TYPE") == 0) {
		ret = (Type)malloc(sizeof(struct Type_));
		ret->kind = BASIC;
		if(strcmp(now->nodechild->value, "int") == 0)
			ret->u.basicType = INT;
		else
			ret->u.basicType = FLOAT;
	}
	else 
		ret = StructSpecifer(now->nodechild);
	return ret;
}
Type StructSpecifer(Infonode* now)
{
    Type ret;
    ret = (Type)malloc(sizeof(struct Type_));
    ret->kind=STRUCTURE;
    if(strcmp(now->nodechild->nodebrothers->name, "Tag") == 0)
    {
        //using a struct with name
        FieldList stru=checknameVar(now->nodechild->nodebrothers->nodechild->value);
        if(stru==NULL)
        {
            ErrMsg(17,now->nodechild->line,now->nodechild->nodebrothers->nodechild->value);
            return NULL;
        }
        else
        {
            if(stru->type->kind!=STRUCTURE)
            {
                ErrMsg(16,now->nodechild->line,now->nodechild->nodebrothers->nodechild->value); //only Node ID carries the name of variable
                return NULL;
            }
            else
                return stru->type;
        }
    }
    else //defining a struct
    {
        Infonode* p=now->nodechild->nodebrothers;
        stMbmDef++;
        ret->u.structure=(StructList)malloc(sizeof(struct StructList_));
        ret->u.structure->tail=DefList(p->nodebrothers->nodebrothers);
        stMbmDef--;;
        if(p->nodechild==NULL)//no name
        {
            sprintf(ret->u.structure->name,"%d",unnamedStruct);
            unnamedStruct++;//to make each struct name diffrent and 
                //because it begin with a number cannot be defined in a normal way
        }
        else
        {
            FieldList stctMbm=checknameVar(p->nodechild->value);
            if(stctMbm)
            {
                ErrMsg(16,now->nodechild->line,now->nodechild->nodebrothers->nodechild->value);
                return NULL;
            }
            else
            {
                stctMbm = (FieldList)malloc(sizeof(struct FieldList_));
                stctMbm->tail=NULL;
                stctMbm->type=(Type)malloc(sizeof(struct Type_));
                stctMbm->name=p->nodechild->value;
                ret->u.structure->name=p->nodechild->value;
                memcpy(stctMbm->type,ret,sizeof(struct Type_)); 
                addVar(stctMbm);
            }
            //strcpy(ret->u.structure->name,p->nodechild->value);
        }
    } 
    return ret;
}
FieldList DefList(Infonode* now)
{
    if(now->nodechild)
    {
        FieldList retDef=Def(now->nodechild),p=retDef;
        while(p && p->tail) p=p->tail;
        if(p) p->tail=DefList(now->nodechild->nodebrothers);
        else retDef=DefList(now->nodechild->nodebrothers);
        return retDef;
    }
    else return NULL;
}
FieldList Def(Infonode* now)
{
    Type givenT=Specifier(now->nodechild);
    if(!givenT) return NULL;
    return DecList(now->nodechild->nodebrothers,givenT);
}
FieldList DecList(Infonode* now,Type givenType)
{
    FieldList retDec=Dec(now->nodechild,givenType);
    //printf("dec%s",retDec->name);
    if(retDec)
    {
        if(checknameVar(retDec->name)==NULL && checknameFunc(retDec->name)==NULL) 
            addVar(retDec);
        else
        {
            if(stMbmDef) ErrMsg(151,now->nodechild->line,retDec->name);
            else ErrMsg(3,now->nodechild->line,retDec->name);
            //return NULL;
        }
    }
    if(now->nodechild->nodebrothers)//more var
    {
        if(retDec) retDec->tail=DecList(now->nodechild->nodebrothers->nodebrothers,givenType);
        else retDec=DecList(now->nodechild->nodebrothers->nodebrothers,givenType);
    }
    else return retDec;
}
FieldList Dec(Infonode* now,Type givenType)
{
    FieldList retVarDec = VarDec(now->nodechild,givenType);   
    if(now->nodechild->nodebrothers)
    {
        //var assign op
        if(stMbmDef)
        {
            ErrMsg(152,now->nodechild->line,"  ");
            return NULL;
        }
        int orv=0;
        Type tr=Exp(now->nodechild->nodebrothers->nodebrothers,&orv);
        if(!tr) return NULL;
        else if(typeEqual(tr,givenType)!=0)
        {
            ErrMsg(5,now->nodechild->line,"  ");
            return NULL;
        }
        else
            return retVarDec;    
    }
    else return retVarDec;  
}
FieldList VarDec(Infonode* now,Type givenType)
{
    if(strcmp(now->nodechild->name,"ID")==0)
    {
        FieldList retVar = (FieldList)malloc(sizeof(struct FieldList_));
		Type varType = (Type)malloc(sizeof(struct Type_));
		memcpy(varType, givenType, sizeof(struct Type_));
		retVar->tail=NULL;
        retVar->name=now->nodechild->value;
		//retVar->name=tmpName;
        retVar->assigned=0;
        retVar->type = varType;
		return retVar;
    }
    else if(strcmp(now->nodechild->name,"VarDec")==0)
    {
        FieldList retVar = VarDec(now->nodechild,givenType);
        if(strcmp(now->nodechild->nodebrothers->nodebrothers->name,"INT")!=0)
        {
            ErrMsg(12,now->nodechild->line,now->nodechild->nodebrothers->nodebrothers->nodechild->value);
            return NULL;
        }
		Type arr = malloc(sizeof(struct Type_));
        //arr->kind = retVar->type->kind;
        //retVar->type->u.array.size = atoi(now->nodechild->nodebrothers->nodebrothers->value);;
		//retVar->type->u.array.elem = arr;
        if (retVar->type->kind != ARRAY) {
			arr->u = retVar->type->u;
            arr->kind=retVar->type->kind;
			retVar->type->kind = ARRAY;
            retVar->type->u.array.size = atoi(now->nodechild->nodebrothers->nodebrothers->value);
            retVar->type->u.array.elem = arr;
		}
        else 
        {
            arr->kind=ARRAY;
			arr->u.array.elem = retVar->type->u.array.elem;
            arr->u.array.size = retVar->type->u.array.size;
            retVar->type->u.array.size = atoi(now->nodechild->nodebrothers->nodebrothers->value);
            retVar->type->u.array.elem = arr;
        }
		return retVar;
    }
}
void CompSt(Infonode* now)
{
    DefList(now->nodechild->nodebrothers);
    StmtList(now->nodechild->nodebrothers->nodebrothers);
}
void StmtList(Infonode* now)
{
    if(now && now->nodechild)
    {
        Stmt(now->nodechild);
        StmtList(now->nodechild->nodebrothers);
    }
    else return;
}
void Stmt(Infonode* now)
{
    int orv=0;
    if(strcmp(now->nodechild->name,"Exp")==0)//exp semi
        Exp(now->nodechild,&orv);
    else if(strcmp(now->nodechild->name,"CompSt")==0)
        CompSt(now->nodechild);
    else if(strcmp(now->nodechild->name,"RETURN")==0)
    {
        Type actRetType=Exp(now->nodechild->nodebrothers,&orv);
        if(!actRetType) return;
        else
        {
            if(FuncReturnType.valid==0) return;
            if(typeEqual(actRetType,FuncReturnType.curFuncRet)!=0)
            {
                ErrMsg(8,now->nodechild->line,now->nodechild->value);
                return;
            }
        }
    }
    else if(strcmp(now->nodechild->name,"WHILE")==0)
    {
        Type whileT=Exp(now->nodechild->nodebrothers->nodebrothers,&orv);
        if(!whileT) return;
        else
        {
            if(whileT->kind!=BASIC || whileT->u.basicType!=INT)
            {
                ErrMsg(7,now->nodechild->line,now->nodechild->value);
                return;
            }
        }
        Stmt(now->nodechild->nodebrothers->nodebrothers->nodebrothers->nodebrothers);
    }
    else if(strcmp(now->nodechild->name,"IF")==0)
    {
        Type ifT=Exp(now->nodechild->nodebrothers->nodebrothers,&orv);
        if(!ifT) return;
        else
        {
            if(ifT->kind!=BASIC || ifT->u.basicType!=INT)
            {
                ErrMsg(7,now->nodechild->line,now->nodechild->value);
                return;
            }
        }
        Infonode* p=now->nodechild->nodebrothers->nodebrothers->nodebrothers->nodebrothers;
        Stmt(p);
        if(p->nodebrothers)
            Stmt(p->nodebrothers->nodebrothers);
    }
}
void FunDec(Infonode* now,int define)
{
    char newFuncName[32];
    strcpy(newFuncName,now->nodechild->value);
    FunctionList candFunc=(FunctionList)malloc(sizeof(struct FunctionList_));
    candFunc->tail=NULL;
    candFunc->name=newFuncName;
    candFunc->name=now->nodechild->value;
    candFunc->retType=(Type)malloc(sizeof(struct Type_));
    memcpy(candFunc->retType,FuncReturnType.curFuncRet,sizeof(struct Type_));
    candFunc->linenum=now->nodechild->line;     
    candFunc->state=UNDEFINED;
    if(strcmp(now->nodechild->nodebrothers->nodebrothers->name,"RP")!=0)//with param
    {    
        FieldList head=(FieldList)malloc(sizeof(struct FieldList_));
        head->tail=NULL;
        FieldList varH=VarList(head,now->nodechild->nodebrothers->nodebrothers);
        varH=head;
        head=head->tail;
        free(varH);
        candFunc->tail=head;
    }
    switch(defineFunc(candFunc,define))
    {
        case 0:return;
        case -1:
            ErrMsg(3,now->nodechild->line,candFunc->name);
            return;
        case -2:
            ErrMsg(19,now->nodechild->line,candFunc->name);
            return;
        case -3:
            ErrMsg(4,now->nodechild->line,candFunc->name);
            return;
        default:return;
    }
}
FieldList VarList(FieldList head,Infonode* now)
{
    if(now->nodechild)
    {
        FieldList retVarL=ParamDec(now->nodechild);
        retVarL->tail=NULL;
        FieldList p=head;
        if(head==NULL) head=retVarL;
        else
        {
            while(p->tail) p=p->tail;
            p->tail=retVarL;
        }
         if(now->nodechild->nodebrothers) VarList(head,now->nodechild->nodebrothers->nodebrothers);
        return retVarL;
    }
    else return NULL;
}
FieldList ParamDec(Infonode* now)
{
    Type ParamT=Specifier(now->nodechild);
    if(!ParamT) return NULL;
    FieldList ParamF=VarDec(now->nodechild->nodebrothers,ParamT);
    ParamF->assigned=7;
    // if(checknameVar(ParamF->name)==0)
    //     addVar(ParamF);
    // else
    // {
    //     ErrMsg(3);
    //     return NULL;
    // }
    return ParamF;
}
void Program(Infonode* now)
{
    memset(varTable, 0, sizeof(varTable));
    memset(funcTable, 0, sizeof(funcTable));
    FuncReturnType.valid=0;
    FuncReturnType.curFuncRet=(Type)malloc(sizeof(struct Type_));
    unnamedStruct=0;
    stMbmDef=0;
    unComplFunc=0;
    addReadWrite();
    ExtDefList(now->nodechild);
    if(unComplFunc)
        for(int i=0;i<TABLE_SIZE;i++)
        {
            if(funcTable[i]!=NULL && funcTable[i]->state==DECLARED)
                ErrMsg(18,funcTable[i]->linenum,funcTable[i]->name);
        }
}
void ExtDefList(Infonode* now)
{
    if(now->nodechild)
    {
        ExtDef(now->nodechild);
        ExtDefList(now->nodechild->nodebrothers);
    }
}
void ExtDef(Infonode* now)
{
    Type gvntp=Specifier(now->nodechild);
    if(!gvntp) return;
    else
    {
        Infonode* p=now->nodechild->nodebrothers;
        if(strcmp(p->name,"ExtDecList")==0) ExtDecList(p,gvntp);
        else if (strcmp(p->name,"FunDec")==0)
        {
            if(strcmp(p->nodebrothers->name,"SEMI")==0)
            {
                FuncReturnType.valid=1;
                memcpy(FuncReturnType.curFuncRet,gvntp,sizeof(struct Type_));
                FunDec(p,0);//declare a func
                memset(FuncReturnType.curFuncRet,0,sizeof(struct Type_));
                //uncReturnType.valid=0;
                //printf("RR");
            }
            else
            {
                FuncReturnType.valid=1;
                memcpy(FuncReturnType.curFuncRet,gvntp,sizeof(struct Type_));
                FunDec(p,1);
                CompSt(p->nodebrothers);
                memset(FuncReturnType.curFuncRet,0,sizeof(struct Type_));
                //FuncReturnType.valid=0;
            }
        }
    }
}
void ExtDecList(Infonode* now,Type givenType)
{
    FieldList v=VarDec(now->nodechild,givenType);
    if(checknameVar(v->name)==NULL && checknameFunc(v->name)==NULL)
        addVar(v);
    else 
    {
        ErrMsg(3,now->nodechild->line,v->name);
        return;
    }
    if(now->nodechild->nodebrothers)  
        ExtDecList(now->nodechild->nodebrothers->nodebrothers,givenType);
}