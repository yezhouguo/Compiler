#include "mips.h"
extern InterCode codes[MAX_LINE];
memObj objs[MAX_LINE];

int curObjNum=0;
int stackP=0;

void startMIPS(char* filename) {
    int paramNum=0;
    mipsF = fopen(filename, "w");
    if(!mipsF) {
        printf("MIPS OUTPUT FILE: CAN NOT OPEN\n");
        exit(-1);
    }
    for(int i=0;i<MAX_LINE;i++) objs[i]=NULL;
    fprintf(mipsF, ".data\n");
    fprintf(mipsF, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(mipsF, "_ret: .asciiz \"\\n\"\n");
    fprintf(mipsF, ".globl main\n");
    fprintf(mipsF, ".text\n");
    fprintf(mipsF, "read:\n");
    fprintf(mipsF, "\tli $v0, 4\n");
    fprintf(mipsF, "\tla $a0, _prompt\n");
    fprintf(mipsF, "\tsyscall\n");
    fprintf(mipsF, "\tli $v0, 5\n");
    fprintf(mipsF, "\tsyscall\n");
    fprintf(mipsF, "\tjr $ra\n");
    fprintf(mipsF, "\nwrite:\n");
    fprintf(mipsF, "\tli $v0, 1\n");
    fprintf(mipsF, "\tsyscall\n");
    fprintf(mipsF, "\tli $v0, 4\n");
    fprintf(mipsF, "\tla $a0, _ret\n");
    fprintf(mipsF, "\tsyscall\n");
    fprintf(mipsF, "\tmove $v0, $0\n");
    fprintf(mipsF, "\tjr $ra\n");
    int i=0;
    while(codes[i]!=NULL)
    {
        switch (codes[i]->kind)
        {
            case I_LABEL:
                MIPSLabel(codes[i]);
                break;
            case I_FUNC:
                MIPSFunc(codes[i]);
                break;
            case ASSIGNII:
                MIPSAssignII(codes[i]);
                break;
            case ASSIGNIP:
                MIPSAssignIP(codes[i]);
                break;
            case ASSIGNPI:
                MIPSAssignPI(codes[i]);
                break;
            case ADD:
                MIPSAdd(codes[i]);
                break;
            case SUB:
                MIPSSub(codes[i]);
                break;
            case MUL:
                MIPSMul(codes[i]);
                break;
            case DIV:
                MIPSDiv(codes[i]);
                break;
            case GOTO:
                MIPSGoto(codes[i]);
                break;
            case IFGOTO:
                MIPSIfGoto(codes[i]);
                break;
            case RETURN:
                MIPSReturn(codes[i]);
                break;
            case DEC:
                MIPSDec(codes[i]);
                break;
            case ARGV:
                MIPSArgV(codes[i]);
                break;
            case CALL:
                MIPSCall(codes[i]);
                break;
            case READ:
                MIPSRead(codes[i]);
                break;
            case WRITE:
                MIPSWrite(codes[i]);
                break;
            case I_ADDRESS:
                MIPSAddr(codes[i]);
                break;
            case PARAM:
                {
                    MIPSParam(codes[i],paramNum);
                    if(codes[i+1]!=NULL && codes[i+1]->kind==PARAM) paramNum++;
                    else paramNum=0;
                }
                break;
            default:
                break;
        }
        i++;
    }
    fclose(mipsF);
}   
int joinObj(char* name, int size)
{
    stackP-=size;
    memObj m=malloc(sizeof(struct memObject));
    m->name=name;
    m->stackP=stackP;
    objs[curObjNum++]=m;
    return stackP;
    //return 0;//EXPR NOT OKAY
}
char* v2MIPS(Operand op)
{
    char* MIPSname = (char*)malloc(10);
    switch(op->kind)
    {
        case TMP:
        case O_ARRAY:
            sprintf(MIPSname,"t%d",op->u.var_no);
            break;
        case VARIABLE:
            sprintf(MIPSname,"v%d",op->u.var_no);
            break;
        case LABEL:
            sprintf(MIPSname,"label%d",op->u.var_no);
            break;
        default:break;
    }
    return MIPSname;
}
memObj checkObj(char* obname)
{
    int k=0;
    while(k<curObjNum)
    {
        if(strcmp(objs[k]->name,obname)==0) return objs[k];
        k++;
    }
    return NULL;
}
void lw(char* oname,int size,int reg)
{
    memObj m=checkObj(oname);
    int sp=0;
    if(m) 
        sp=m->stackP;
    else
    {
        sp=joinObj(oname,size);
        fprintf(mipsF,"\taddi $sp, $sp, -%d\n",size);
    }
    fprintf(mipsF,"\tlw $t%d, %d($fp)\n",reg,sp);
}
void sw(char* oname,int size,int reg)
{
    memObj m=checkObj(oname);
    int sp=0;
    if(m) sp=m->stackP;
    else
    {
        sp=joinObj(oname,size);
        fprintf(mipsF,"\taddi $sp, $sp, -%d\n",size);
    }
    fprintf(mipsF,"\tsw $t%d, %d($fp)\n",reg,sp);
}
void MIPSLabel(InterCode code)
{
    fprintf(mipsF,"label%d:\n",code->u.oneop.op->u.var_no);
}
void MIPSAssignII(InterCode code)
{
    Operand l,r;
    l=code->u.assign.left;
    r=code->u.assign.right;
    char* ln=v2MIPS(l);
    if(r->kind==CONSTANT)
        fprintf(mipsF,"\tli $t1, %d\n",r->u.value);
    else
    {
        char* rn=v2MIPS(r);
        lw(rn,4,0);
        fprintf(mipsF,"\tmove $t1, $t0\n");
    }
    sw(ln,4,1);    
}
void MIPSAssignIP(InterCode code)
{
    char* nl=v2MIPS(code->u.assign.left);
    char* nr=v2MIPS(code->u.assign.right);
    lw(nr,4,0);
    fprintf(mipsF,"\tlw $t1, 0($t0)\n");
    sw(nl,4,1);
}
void MIPSAssignPI(InterCode code)
{
    char* nl=v2MIPS(code->u.assign.left);
    char* nr=v2MIPS(code->u.assign.right);
    lw(nr,4,0);
    lw(nl,4,1);
    fprintf(mipsF,"\tsw $t0, 0($t1)\n");
    sw(nr,4,0);
    sw(nl,4,1);
}
void MIPSAdd(InterCode code)
{
    Operand r,op1,op2;
    r=code->u.twoop.result;
    op1=code->u.twoop.op1;
    op2=code->u.twoop.op2;
    char* rn=v2MIPS(r);
    if(op1->kind==CONSTANT) fprintf(mipsF,"\tli $t0, %d\n",op1->u.value);
    else lw(v2MIPS(op1),4,0);
    if(op2->kind==CONSTANT)
    {
        fprintf(mipsF,"\taddi $t1, $t0, %d\n",op2->u.value);
        sw(rn,4,1);
    }
    else
    {
        lw(v2MIPS(op2),4,1);
        fprintf(mipsF,"\tadd $t2, $t0, $t1\n");        //fprintf(mipsF,"\tadd $t2, $t1, $t0\n");
        sw(rn,4,2);
    }   
}
void MIPSSub(InterCode code)
{
    Operand r,op1,op2;
    r=code->u.twoop.result;
    op1=code->u.twoop.op1;
    op2=code->u.twoop.op2;
    char* rn=v2MIPS(r);
    if(op1->kind==CONSTANT) fprintf(mipsF,"\tli $t0, %d\n",op1->u.value);
    else lw(v2MIPS(op1),4,0);
    if(op2->kind==CONSTANT)
    {
        fprintf(mipsF,"\taddi $t1, $t0, %d\n",-(op2->u.value));
        sw(rn,4,1);
    }
    else
    {
        lw(v2MIPS(op2),4,1);
        fprintf(mipsF,"\tsub $t2, $t0, $t1\n");     //fprintf(mipsF,"\tsub $t2, $t1, $t0\n");
        sw(rn,4,2);
    }   
}
void MIPSMul(InterCode code)
{
    Operand r,op1,op2;
    r=code->u.twoop.result;
    op1=code->u.twoop.op1;
    op2=code->u.twoop.op2;
    char* rn=v2MIPS(r);
    if(op1->kind==CONSTANT) fprintf(mipsF,"\tli $t0, %d\n",op1->u.value);
    else lw(v2MIPS(op1),4,0);
    if(op2->kind==CONSTANT) fprintf(mipsF,"\tli $t1, %d\n",op2->u.value);
    else lw(v2MIPS(op2),4,1);                       //else lw(v2MIPS(op2),4,0);
    fprintf(mipsF,"\tmul $t2, $t0, $t1\n");         //fprintf(mipsF,"\tsub $t2, $t1, $t0\n");
    sw(rn,4,2);
}
void MIPSDiv(InterCode code)
{
    Operand r,op1,op2;
    r=code->u.twoop.result;
    op1=code->u.twoop.op1;
    op2=code->u.twoop.op2;
    char* rn=v2MIPS(r);
    if(op1->kind==CONSTANT) fprintf(mipsF,"\tli $t0, %d\n",op1->u.value);
    else lw(v2MIPS(op1),4,0);
    if(op2->kind==CONSTANT) fprintf(mipsF,"\tli $t1, %d\n",op2->u.value);
    else lw(v2MIPS(op2),4,0);
    fprintf(mipsF,"\tdiv $t0, $t1\n");         //fprintf(mipsF,"\tmul $t2, $t1, $t0\n"); 
    fprintf(mipsF,"\tmflo $t2\n");
    sw(rn,4,2);
}
void MIPSGoto(InterCode code)
{
    char* l=v2MIPS(code->u.oneop.op);
    fprintf(mipsF,"\tj %s\n", l);
}
void MIPSIfGoto(InterCode code)
{
    char* n1=v2MIPS(code->u.threeop.op1);
    char* n2=v2MIPS(code->u.threeop.op2);
    char* l=v2MIPS(code->u.threeop.op3);
    char* relop=code->u.threeop.relop;
    lw(n1,4,0);
    lw(n2,4,1);
	if (strcmp(relop, "==") == 0) 
		fprintf(mipsF, "\tbeq $t0, $t1, %s\n", l);
	else if (strcmp(relop, "!=") == 0) 
		fprintf(mipsF, "\tbne $t0, $t1, %s\n", l);
	else if (strcmp(relop, ">") == 0) 
		fprintf(mipsF, "\tbgt $t0, $t1, %s\n", l);
	else if (strcmp(relop, "<") == 0) 
		fprintf(mipsF, "\tblt $t0, $t1, %s\n", l);
	else if (strcmp(relop, ">=") == 0) 
		fprintf(mipsF, "\tbge $t0, $t1, %s\n", l);
	else if (strcmp(relop, "<=") == 0) 
		fprintf(mipsF, "\tble $t0, $t1, %s\n", l);
    sw(n1,4,0);
    sw(n2,4,1);
}
void MIPSFunc(InterCode code)
{
    stackP=0;
    fprintf(mipsF,"\n%s:\n", code->u.oneop.op->u.name);
    fprintf(mipsF,"\taddi $sp, $sp, -4\n");
    fprintf(mipsF,"\tsw $fp, 0($sp)\n");
    fprintf(mipsF,"\tmove $fp, $sp\n");
}
void MIPSParam(InterCode code,int paramNum)
{
    memObj o=(memObj)malloc(sizeof(struct memObject));
    o->name=v2MIPS(code->u.oneop.op);
    o->stackP=8+4*paramNum;
    objs[curObjNum++]=o;
}
void MIPSReturn(InterCode code)
{
    char* rt=v2MIPS(code->u.oneop.op);
    memObj m=checkObj(rt);
    fprintf(mipsF, "\tlw $v0, %d($fp)\n", m->stackP);
    fprintf(mipsF, "\tmove $sp, $fp\n");
    fprintf(mipsF, "\tlw $fp, 0($sp)\n");
    fprintf(mipsF, "\taddi $sp, $sp, 4\n");
    fprintf(mipsF, "\tjr $ra\n");
}
void MIPSDec(InterCode code)
{
    char* vn=v2MIPS(code->u.dec.op1);
    lw(vn,code->u.dec.size,0);
    sw(vn,code->u.dec.size,0);
}
void MIPSArgV(InterCode code)
{
    Operand arg=code->u.oneop.op;
    lw(v2MIPS(arg),4,0);
    stackP-=4;
    fprintf(mipsF,"\taddi $sp, $fp, %d\n", stackP);
    fprintf(mipsF,"\tsw $t0, 0($sp)\n");
}
void MIPSCall(InterCode code)
{
    char* vn=v2MIPS(code->u.call.op1);
    fprintf(mipsF,"\taddi $sp, $sp, -4\n");
    stackP-=4;
    fprintf(mipsF,"\tsw $ra, 0($sp)\n");
    fprintf(mipsF,"\tjal %s\n", code->u.call.op2->u.name);
    fprintf(mipsF,"\tlw $ra, 0($sp)\n");
    fprintf(mipsF,"\taddi $sp, $sp, 4\n");
    stackP+=4;
    lw(vn,4,0);
    fprintf(mipsF,"\tmove $t0, $v0\n");
    sw(vn,4,0);
}
void MIPSRead(InterCode code)
{
    char* vn=v2MIPS(code->u.oneop.op);
    fprintf(mipsF, "\taddi $sp, $sp, -4\n");
	fprintf(mipsF, "\tsw $ra, 0($sp)\n");
    stackP-=4;
	fprintf(mipsF, "\tjal read\n");
	fprintf(mipsF, "\tlw $ra, 0($sp)\n");
	fprintf(mipsF, "\taddi $sp, $sp, 4\n");
    stackP+=4;
	lw(vn,4,0);
	fprintf(mipsF, "\tmove $t0, $v0\n");
	sw(vn,4,0);
}
void MIPSWrite(InterCode code)
{
    lw(v2MIPS(code->u.oneop.op),4,0);
    fprintf(mipsF, "\tmove $a0, $t0\n");        //fprintf(mipsF, "\tmove $a0, $t1\n");   
    fprintf(mipsF, "\taddi $sp, $sp, -4\n");
	fprintf(mipsF, "\tsw $ra, 0($sp)\n");
    stackP-=4;
	fprintf(mipsF, "\tjal write\n");
	fprintf(mipsF, "\tlw $ra, 0($sp)\n");
	fprintf(mipsF, "\taddi $sp, $sp, 4\n");
    stackP+=4;
}
void MIPSAddr(InterCode code)
{
    if(code->u.twoop.op2->kind==CONSTANT) fprintf(mipsF,"\tli $t1, %d\n", code->u.twoop.op2->u.value);
    else lw(v2MIPS(code->u.twoop.op2),4,1);
    memObj m=checkObj(v2MIPS(code->u.twoop.op1));
    fprintf(mipsF,"\taddi $t0, $fp, %d\n", m->stackP);
    fprintf(mipsF,"\tadd $t2, $t1, $t0\n");
    sw(v2MIPS(code->u.twoop.result),4,2);
}