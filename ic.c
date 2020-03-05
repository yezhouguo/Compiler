#include "ic.h"
#include "semantic.h"


InterCode codes[MAX_LINE];
int curCode = 0;

int tmpNum = 0;
int var_no = 0;
int labelNum = 0;
unsigned char leftaddr = 0, inStruct = 0;
int stctOff = 0;
char *stctName;

int structSize(FieldList str);
int arraySize(FieldList arr)
{
    if (arr->type->u.array.elem->kind == BASIC)
        return 4;
    else
        return structSize(checknameVar(arr->type->u.array.elem->u.structure->name));
}
int structSize(FieldList str)
{
    int strSize = 0;
    FieldList p = str->type->u.structure->tail;
    while (p)
    {
        switch (p->type->kind)
        {
        case BASIC:
            strSize += 4;
            break;
        case ARRAY:
            strSize += (p->type->u.array.size * arraySize(checknameVar(p->name)));
            break;
        case STRUCTURE:
            strSize += structSize(checknameVar(p->type->u.structure->name));
            break;
        default:
            break;
        }
        p = p->tail;
    }
    return strSize;
}
int stctMbmOff(char *stctName, char *stctMbmName)
{
    FieldList stct = checknameVar(stctName);
    FieldList p = stct->type->u.structure->tail;
    int MbmOff = 0;
    while (p)
    {
        if (strcmp(p->name, stctMbmName) == 0)
            return MbmOff;
        switch (p->type->kind)
        {
        case BASIC:
            MbmOff += 4;
            break;
        case ARRAY:
            MbmOff += (p->type->u.array.size * arraySize(checknameVar(p->name)));
            break;
        case STRUCTURE:
            MbmOff += structSize(checknameVar(p->type->u.structure->name));
            break;
        default:
            break;
        }
        p = p->tail;
    }
    return -1;
}
InterCode makeAssign(int lk, int lv, int rk, int rv)
{
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    Operand r = (Operand)malloc(sizeof(struct Operand_));
    Operand l = (Operand)malloc(sizeof(struct Operand_));
    l->kind = lk;
    l->u.value = lv;
    r->kind = rk;
    r->u.var_no = rv;
    ic->kind = ASSIGNII;
    ic->u.assign.right = r;
    ic->u.assign.left = l;
    return ic;
}
InterCode makeCall(int op1k, int op1v, int op2k, char *op2v)
{
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    Operand op1 = (Operand)malloc(sizeof(struct Operand_));
    Operand op2 = (Operand)malloc(sizeof(struct Operand_));
    op1->kind = op1k;
    op1->u.value = op1v;
    op2->kind = op2k;
    op2->u.name = op2v;
    ic->u.call.op1 = op1;
    ic->u.call.op2 = op2;
    ic->kind = CALL;
    return ic;
}
InterCode makeDec(int kind, int newvar, int size)
{
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = kind;
    op->u.value = newvar;
    ic->u.dec.op1 = op;
    ic->u.dec.size = size;
    ic->kind = DEC;
    return ic;
}
InterCode make1op(int kind, int val, int ic_kind)
{
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = kind;
    op->u.value = val;
    ic->u.oneop.op = op;
    ic->kind = ic_kind;
    return ic;
}
InterCode make2op(int op1k, int op1v, int op2k, int op2v, int rk, int rv, int ic_kind)
{
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    Operand op1 = (Operand)malloc(sizeof(struct Operand_));
    Operand op2 = (Operand)malloc(sizeof(struct Operand_));
    Operand result = (Operand)malloc(sizeof(struct Operand_));
    op1->kind = op1k;
    op1->u.value = op1v;
    op2->kind = op2k;
    op2->u.value = op2v;
    result->u.value = rv;
    result->kind = rk;
    ic->u.twoop.result = result;
    ic->u.twoop.op1 = op1;
    ic->u.twoop.op2 = op2;
    ic->kind = ic_kind;
    return ic;
}
InterCode make3op(int op1k, int op1v, int op2k, int op2v, int op3k, int op3v, char *relop, int ic_kind)
{
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    Operand op1 = (Operand)malloc(sizeof(struct Operand_));
    Operand op2 = (Operand)malloc(sizeof(struct Operand_));
    Operand op3 = (Operand)malloc(sizeof(struct Operand_));
    op1->kind = op1k;
    op1->u.value = op1v;
    op2->kind = op2k;
    op2->u.value = op2v;
    op3->kind = op3k;
    op3->u.value = op3v;
    ic->u.threeop.op1 = op1;
    ic->u.threeop.op2 = op2;
    ic->u.threeop.op3 = op3;
    ic->u.threeop.relop = relop;
    ic->kind = ic_kind;
    return ic;
}
char* printO(Operand op)
{
    if (op == NULL) return NULL;
    char* ret = (char*)malloc(sizeof(char)*20);
    memset(ret, 0, sizeof(ret));
    switch (op->kind)
    {
        case TMP:
            sprintf(ret, "t%d", op->u.var_no);
            break;
        case VARIABLE:
            sprintf(ret, "v%d", op->u.var_no);
            break;
        case CONSTANT:
            sprintf(ret, "#%d", op->u.value);
            break;
        case ADDRESS:  break;    //ADDR IS DEALT IN printIC
        case FUNC:
            sprintf(ret, "FUNCTION %s",op->u.name);
            break;        
        case LABEL:
            sprintf(ret, "label%d", op->u.var_no);
            break; 
        case O_ARRAY:
            sprintf(ret, "t%d", op->u.var_no);
            break;
        default: break;
    }
    return ret;
}

char* printIC(InterCode ic)
{
    char* ic_out = (char*)malloc(sizeof(char)*50);
    memset(ic_out, 0, sizeof(ic_out));
    switch (ic->kind)
    {
        case I_LABEL:
            sprintf(ic_out, "LABEL %s :\n", printO(ic->u.oneop.op));
            break;
        case I_FUNC:
            sprintf(ic_out, "%s :\n", printO(ic->u.oneop.op));
            break;
        case ASSIGNII:
            sprintf(ic_out, "%s := %s\n", printO(ic->u.assign.left), printO(ic->u.assign.right));
            break;
        case ADD:
            sprintf(ic_out, "%s := %s + %s\n", printO(ic->u.twoop.result), printO(ic->u.twoop.op1), printO(ic->u.twoop.op2));
            break;
        case SUB:
            sprintf(ic_out, "%s := %s - %s\n", printO(ic->u.twoop.result), printO(ic->u.twoop.op1), printO(ic->u.twoop.op2));
            break;
        case MUL:
            sprintf(ic_out, "%s := %s * %s\n", printO(ic->u.twoop.result), printO(ic->u.twoop.op1), printO(ic->u.twoop.op2));
            break;
        case DIV:
            sprintf(ic_out, "%s := %s / %s\n", printO(ic->u.twoop.result), printO(ic->u.twoop.op1), printO(ic->u.twoop.op2));
            break;
        case I_ADDRESS:
            sprintf(ic_out, "%s := &%s + %s\n", printO(ic->u.twoop.result), printO(ic->u.twoop.op1), printO(ic->u.twoop.op2));
            break;
        case ASSIGNIP:
            sprintf(ic_out, "%s := *%s\n", printO(ic->u.assign.left), printO(ic->u.assign.right));
            break;
        case ASSIGNPI:
            sprintf(ic_out, "*%s := %s\n", printO(ic->u.assign.left), printO(ic->u.assign.right));
            break;
        case GOTO:
            sprintf(ic_out, "GOTO label%d\n", ic->u.oneop.op->u.var_no);
            break;
        case IFGOTO:
            sprintf(ic_out, "IF %s %s %s GOTO label%d\n", printO(ic->u.threeop.op1), ic->u.threeop.relop, printO(ic->u.threeop.op2), ic->u.threeop.op3->u.var_no);
            break;
        case RETURN:
            sprintf(ic_out, "RETURN %s\n", printO(ic->u.oneop.op));
            break;
        case DEC:
            sprintf(ic_out, "DEC %s %d\n", printO(ic->u.dec.op1), ic->u.dec.size);
            break;
        case ARGV:
            sprintf(ic_out, "ARG %s\n", printO(ic->u.oneop.op));
            break;
        case ARGA:
            sprintf(ic_out, "ARG &%s\n", printO(ic->u.oneop.op));
            break;
        case CALL:
            sprintf(ic_out, "%s := CALL %s\n", printO(ic->u.call.op1), ic->u.call.op2->u.name);
            break;
        case PARAM:
            sprintf(ic_out, "PARAM %s\n", printO(ic->u.oneop.op));
            break;
        case READ:
            sprintf(ic_out, "READ %s\n", printO(ic->u.oneop.op));
            break;
        case WRITE:
            sprintf(ic_out, "WRITE %s\n", printO(ic->u.oneop.op));
            break;
        default:
            break;
    }
    return ic_out;
}
int translate_Exp(Infonode *now, int *place)
{
    //if (curCode == 2)
    //    printf("");
    if (strcmp(now->nodechild->name, "INT") == 0) //int
    {
        if (!place)
            return -1;
        int val = atoi(now->nodechild->value);
        InterCode ic = makeAssign(TMP, *place, CONSTANT, val);
        codes[curCode++] = ic;
        return 0;
    }
    else if (strcmp(now->nodechild->name, "ID") == 0) //id
    {
        if (now->nodechild->nodebrothers == NULL)
        {
            if (!place)
                return -1;
            tmpNum--;
            //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
            var_no++;
            *place = var_no;
            //STRUCT FIX!!
            FieldList tmpf = checknameVar(now->nodechild->value);
            if (tmpf->type->kind == STRUCTURE)
            {
                stctName = tmpf->type->u.structure->name;
                //stctMbm = tmpf->name;
                stctOff = 0;
            }
            if (tmpf->assigned == 0) //left
            {
                tmpf->assigned = 1;
                tmpf->val = *place;
            }
            else
            {
                var_no--;
                *place = tmpf->val;
            }
            return 1;
        }
        else if (strcmp(now->nodechild->nodebrothers->nodebrothers->name, "Args") == 0) //id lb args
        {
            if (!place)
            {
                int tmp = 0;
                tmpNum++;
                //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
                tmp = tmpNum;
                place = &tmp;
            }
            if (strcmp(now->nodechild->value, "write") == 0)
            {
                tmpNum++;
                //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
                int tmpt = tmpNum;
                int kind = translate_Args(now->nodechild->nodebrothers->nodebrothers, &tmpt);
                InterCode ic = make1op(kind, *place, WRITE);
                codes[curCode++] = ic;
                return 0;
            }
            else
            {
                translate_Args(now->nodechild->nodebrothers->nodebrothers, NULL);
                InterCode ic = makeCall(TMP, *place, FUNC, now->nodechild->value);
                codes[curCode++] = ic;
                return 0;
            }
        }
        else //id lb rb
        {
            if (!place)
            {
                int tmp = 0;
                tmpNum++;
                //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
                tmp = tmpNum;
                place = &tmp;
            }
            if (strcmp(now->nodechild->value, "read") == 0)
            {
                InterCode ic = make1op(TMP, *place, READ);
                codes[curCode++] = ic;
                return 0;
            }
            else
            {
                InterCode ic = makeCall(TMP, *place, FUNC, now->nodechild->value);
                codes[curCode++] = ic;
                return 0;
            }
        }
    }
    else if (strcmp(now->nodechild->name, "MINUS") == 0) //exp=-3
    {
        if (!place)
            return -1;
        tmpNum++;
        //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
        int tmpn = tmpNum;
        int k = translate_Exp(now->nodechild->nodebrothers, &tmpn);
        InterCode ic = make2op(CONSTANT, 0, k, tmpn, TMP, *place, SUB);
        codes[curCode++] = ic;
        return 0;
    }
    else if (strcmp(now->nodechild->name, "Exp") == 0) //exp=exp1 ??? exp2
    {
        char *eop = now->nodechild->nodebrothers->name;
        if (strcmp(eop, "ASSIGNOP") == 0)
        {
            tmpNum++;
            //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
            //printf("EXP Assignop tempnum:%d\n", tmpNum);
            int ln = tmpNum;
            //addr FIX
            leftaddr = 1;
            int lk = translate_Exp(now->nodechild, &ln);
            leftaddr = 0;
            tmpNum++;
            //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
            int rn = tmpNum;
            int rk = translate_Exp(now->nodechild->nodebrothers->nodebrothers, &rn);
            InterCode ic1 = makeAssign(lk, ln, rk, rn);
            //TODO
            if(now->nodechild->nodechild)
            if(now->nodechild->nodechild->nodebrothers)
            if(strcmp(now->nodechild->nodechild->nodebrothers->name,"DOT")==0
            ||
            strcmp(now->nodechild->nodechild->nodebrothers->name,"LB")==0
            )//TEMPORARY APPROACH!!!! TODO
                ic1->kind=ASSIGNPI;
            //array structure FIX ic kind = assignpi
            codes[curCode++] = ic1;
            if (place)
            {
                InterCode ic2 = makeAssign(TMP, *place, lk, ln);
                codes[curCode++] = ic2;
                return 0;
            }
            return -1;
        }
        else if (strcmp(eop, "PLUS") == 0 || strcmp(eop, "MINUS") == 0 || strcmp(eop, "STAR") == 0 || strcmp(eop, "DIV") == 0)
        {
            if (!place)
                return -1;
            tmpNum++;
            //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
            int op1v = tmpNum;
            int op1k = translate_Exp(now->nodechild, &op1v);
            tmpNum++;
            //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
            int op2v = tmpNum;
            int op2k = translate_Exp(now->nodechild->nodebrothers->nodebrothers, &op2v);
            int ic_kind = 0;
            if (strcmp(eop, "PLUS") == 0)
                ic_kind = ADD;
            else if (strcmp(eop, "MINUS") == 0)
                ic_kind = SUB;
            else if (strcmp(eop, "STAR") == 0)
                ic_kind = MUL;
            else if (strcmp(eop, "DIV") == 0)
                ic_kind = DIV;
            InterCode ic = make2op(op1k, op1v, op2k, op2v, TMP, *place, ic_kind);
            codes[curCode++] = ic;
            return 0;
        }
        else if (strcmp(eop, "RELOP") == 0 || strcmp(eop, "AND") == 0 || strcmp(eop, "OR") == 0)
        {
            if (!place)
                return -1;
            else
            {
                labelNum++;
                int lb1 = labelNum;
                labelNum++;
                int lb2 = labelNum;
                InterCode ic1 = makeAssign(TMP, *place, CONSTANT, 0);
                codes[curCode++] = ic1;
                translate_Cond(now->nodechild, lb1, lb2);
                InterCode ic2 = make1op(LABEL, lb1, I_LABEL);
                codes[curCode++] = ic2;
                InterCode ic3 = makeAssign(TMP, *place, CONSTANT, 1);
                codes[curCode++] = ic3;
                InterCode ic4 = make1op(LABEL, lb2, I_LABEL);
                codes[curCode++] = ic4;
                return 0;
            }
        }
        else if (strcmp(eop, "LB") == 0) //array
        {
            if (!place)
                return -1;
            if (strcmp(now->nodechild->nodechild->name, "Exp") == 0 && strcmp(now->nodechild->nodechild->nodebrothers->name, "LB") == 0)
            {
                printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
                exit(-1);
                return -1;
            }
            tmpNum++;
            //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
            int tmpn = tmpNum;
            int tmpk = translate_Exp(now->nodechild, &tmpn);
            int index = -1;
            Infonode *indexp = now->nodechild->nodebrothers->nodebrothers->nodechild;
            if (strcmp(indexp->name, "INT") == 0) //a[1]
                index = atoi(indexp->value);
            //else return -1;//err
            int width = 4; //structure array FIX
            width = arraySize(checknameVar(now->nodechild->nodechild->value));
            tmpNum++;
            //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
            int tmpa = tmpNum;
            InterCode ic1;
            if (index != -1) //int
            {
                if (leftaddr)
                {
                    tmpNum--;
                    //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
                    ic1 = make2op(tmpk, tmpn, CONSTANT, index * width, TMP, *place, I_ADDRESS);
                }
                else
                    ic1 = make2op(tmpk, tmpn, CONSTANT, index * width, TMP, tmpa, I_ADDRESS);
            }
            else
            {
                tmpNum++;
                //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
                int x1 = tmpNum;
                int k1 = translate_Exp(now->nodechild->nodebrothers->nodebrothers, &x1);
                tmpNum++;
                //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
                int o1 = tmpNum;
                InterCode ic1_5 = make2op(k1, x1, CONSTANT, width, TMP, o1, MUL);
                codes[curCode++] = ic1_5;
                if (leftaddr)
                    ic1 = make2op(tmpk, tmpn, CONSTANT, o1, TMP, *place, I_ADDRESS);
                else
                    ic1 = make2op(tmpk, tmpn, CONSTANT, o1, TMP, tmpa, I_ADDRESS);
            }
            codes[curCode++] = ic1;
            if (leftaddr == 0)
            {
                InterCode ic2 = makeAssign(TMP, *place, TMP, tmpa);
                ic2->kind = ASSIGNIP;
                codes[curCode++] = ic2;
            }
            return 6;
        }
        else if (strcmp(eop, "DOT") == 0) //struct
        {
            if (!place)
                return -1;
            if (inStruct == 1)
            {
                translate_Exp(now->nodechild, place);
                stctOff += stctMbmOff(stctName, now->nodechild->nodebrothers->nodebrothers->value);
                return 0;
            }
            tmpNum++;
            //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
            int t1 = tmpNum;
            int k;
            if (strcmp(now->nodechild->nodechild->name, "ID") != 0)
            {
                k = VARIABLE;
                inStruct = 1;
                translate_Exp(now->nodechild, &t1);
                inStruct = 0;
                stctOff += stctMbmOff(stctName, now->nodebrothers->nodebrothers->value);
            }
            else
            {
                k = VARIABLE;
                int a1 = translate_Exp(now->nodechild, &t1);
                FieldList stct = checknameVar(now->nodechild->nodechild->value);
                stctOff = stctMbmOff(stct->type->u.structure->name, now->nodechild->nodebrothers->nodebrothers->value);
            }
            tmpNum++;
            //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
            int a2 = tmpNum;
            //int v1=(++var_no);
            InterCode ic1;
            if (!leftaddr)
                ic1 = make2op(k, t1, CONSTANT, stctOff, TMP, a2, ADD);
            else
                ic1 = make2op(k, t1, CONSTANT, stctOff, TMP, *place, I_ADDRESS);
            codes[curCode++] = ic1;
            if (!leftaddr)
            {
                InterCode ic2 = makeAssign(TMP, *place, TMP, a2);
                ic2->kind=ASSIGNIP;
                codes[curCode++] = ic2;
            }
            return 6;
        }
    }
    else if (strcmp(now->nodechild->name, "NOT") == 0)
    {
        if (!place)
            return -1;
        labelNum++;
        int lb1 = labelNum;
        labelNum++;
        int lb2 = labelNum;
        InterCode ic1 = makeAssign(TMP, *place, CONSTANT, 0);
        codes[curCode++] = ic1;
        translate_Cond(now->nodechild, lb1, lb2);
        InterCode ic2 = make1op(LABEL, lb1, I_LABEL);
        codes[curCode++] = ic2;
        InterCode ic3 = makeAssign(TMP, *place, CONSTANT, 1);
        codes[curCode++] = ic3;
        InterCode ic4 = make1op(LABEL, lb2, I_LABEL);
        codes[curCode++] = ic4;
        return 0;
    }
    else if (strcmp(now->nodechild->name, "LP") == 0)
        return translate_Exp(now->nodechild->nodebrothers, place);
    else
        return -1;
}
void translate_Cond(Infonode *now, int label_true, int label_false)
{
    Infonode *p = now->nodechild;
    if (strcmp(p->name, "Exp") == 0) //ecp ??? exp2
    {
        if (strcmp(p->nodebrothers->name, "RELOP") == 0) //exp1 >< exp2
        {
            tmpNum++;
            //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
            int v1 = tmpNum;
            //tmpNum++;printf("[tmpn %d icnum %d]\n",tmpNum,curCode);  //should be deleted
            int v2 = tmpNum;
            int k1 = translate_Exp(p, &v1);
            int k2 = translate_Exp(p->nodebrothers->nodebrothers, &v2);
            char *relop = p->nodebrothers->value;
            InterCode ic1 = make3op(k1, v1, k2, v2, LABEL, label_true, relop, IFGOTO);
            codes[curCode++] = ic1;
            InterCode ic2 = make1op(LABEL, label_false, GOTO);
            codes[curCode++] = ic2;
        }
        else if (strcmp(p->nodebrothers->name, "AND") == 0 || strcmp(p->nodebrothers->name, "OR") == 0)
        {
            labelNum++;
            int l1 = labelNum;
            if (strcmp(p->nodebrothers->name, "AND") == 0)
                translate_Cond(p, l1, label_false);
            else
                translate_Cond(p, label_true, l1);
            InterCode ic1 = make1op(LABEL, l1, I_LABEL);
            codes[curCode++] = ic1;
            translate_Cond(p->nodebrothers->nodebrothers, label_true, label_false);
        }
    }
    else if (strcmp(p->name, "NOT") == 0)
        translate_Cond(p->nodebrothers, label_false, label_true);
    else
    {
        tmpNum++;
        //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
        int t1 = tmpNum;
        int k1 = translate_Exp(now, &t1);
        char relop[5];
        strcpy(relop, "!=");
        InterCode ic1 = make3op(k1, t1, CONSTANT, 0, LABEL, label_true, relop, IFGOTO);
        codes[curCode++] = ic1;
        InterCode ic2 = make1op(LABEL, label_false, GOTO);
        codes[curCode++] = ic2;
    }
}
int translate_Args(Infonode *now, int *place)
{
    if (place) //write
        return translate_Exp(now->nodechild, place);
    else //other
    {
        tmpNum++;
        //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
        int t1 = tmpNum;
        int k = translate_Exp(now->nodechild, &t1);
        if (strcmp(now->nodechild->nodechild->name, "ID") == 0)
        {
            FieldList param = checknameVar(now->nodechild->nodechild->value);
            if (param->type->kind == STRUCTURE)
            {
                
                if (now->nodechild->nodebrothers)
                    translate_Args(now->nodechild->nodebrothers->nodebrothers, NULL);
                InterCode sp = make1op(k, t1, ARGA);
                codes[curCode++] = sp;
                return -1;
            }
        }
        if (now->nodechild->nodebrothers)
            translate_Args(now->nodechild->nodebrothers->nodebrothers, NULL);
        InterCode ic = make1op(k, t1, ARGV);
        codes[curCode++] = ic;
    }
    return -1;
}
void translate_Dec(Infonode *now)
{
    Infonode *p = now->nodechild;
    if (p->nodebrothers) //vardec assign exp
    {
        if (strcmp(p->nodechild->name, "ID") == 0)
        {
            FieldList newvar = checknameVar(p->nodechild->value);
            if (newvar->assigned == 0)
            {
                newvar->assigned = 1;
                newvar->val = (++var_no);
            }
            int lv = newvar->val;
            int rv = (++var_no);
            int rk = translate_Exp(p->nodebrothers->nodebrothers, &rv);
            InterCode ic = makeAssign(VARIABLE, lv, rk, rv);
            codes[curCode++] = ic;
        }
    }
    else
    {
        if (strcmp(p->nodechild->name, "ID") == 0)
        {
            FieldList newvar = checknameVar(p->nodechild->value);
            if (newvar->type->kind == STRUCTURE) //dec xxx
            {
                if (newvar->assigned == 0)
                {
                    newvar->assigned = 1;
                    newvar->val = (++var_no);
                }
                int size = structSize(checknameVar(newvar->type->u.structure->name));
                InterCode ic = makeDec(VARIABLE, newvar->val, size);
                codes[curCode++] = ic;
            }
        }
        else //vardec lb int rb
        {
            if (strcmp(p->nodechild->nodechild->name, "ID") != 0)
            {
                printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
                exit(-1);
                return;
            }
            else
            {
                FieldList newvar = checknameVar(p->nodechild->nodechild->value);
                if (newvar->assigned == 0)
                {
                    newvar->assigned = 1;
                    newvar->val = (++var_no);
                }
                int size = 0;
                if(p->nodechild->nodebrothers != NULL)
                    if(p->nodechild->nodebrothers->nodebrothers != NULL)
                        //printf("rrr%s\n",p->nodechild->nodebrothers->nodebrothers->value);
                        size = atoi(p->nodechild->nodebrothers->nodebrothers->value) * 4;
                        InterCode ic = makeDec(VARIABLE, newvar->val, size);
                        codes[curCode++] = ic;
            }
        }
    }
}
void translate_DefList(Infonode *now)
{
    Infonode *p = now->nodechild;
    if (p)
    {
        translate_Def(p);
        translate_DefList(p->nodebrothers);
    }
}
void translate_Def(Infonode *now)
{
    translate_DecList(now->nodechild->nodebrothers);
}
void translate_DecList(Infonode *now)
{
    Infonode *p = now->nodechild;
    translate_Dec(p);
    if (p->nodebrothers)
        translate_DecList(p->nodebrothers->nodebrothers);
}
void translate_Stmt(Infonode *now)
{
    Infonode *p = now->nodechild;
    char *ptype = p->name;
    if (strcmp(ptype, "Exp") == 0) //exp semi
        translate_Exp(p, NULL);
    else if (strcmp(ptype, "CompSt") == 0) //compst
        translate_CompSt(p);
    else if (strcmp(ptype, "RETURN") == 0) //return exp semi
    {
        tmpNum++;
        //printf("[tmpn %d icnum %d]\n", tmpNum, curCode);
        int t = tmpNum;
        int k = translate_Exp(p->nodebrothers, &t);
        InterCode ic = make1op(k, t, RETURN);
        codes[curCode++] = ic;
    }
    else if (strcmp(ptype, "IF") == 0)
    {
        if (p->nodebrothers->nodebrothers->nodebrothers->nodebrothers->nodebrothers) //if else
        {
            int l1, l2, l3;
            labelNum++;
            l1 = labelNum;
            labelNum++;
            l2 = labelNum;
            labelNum++;
            l3 = labelNum;
            translate_Cond(p->nodebrothers->nodebrothers, l1, l2);
            InterCode ic1 = make1op(LABEL, l1, I_LABEL);
            codes[curCode++] = ic1;
            translate_Stmt(p->nodebrothers->nodebrothers->nodebrothers->nodebrothers);
            InterCode ic2 = make1op(LABEL, l3, GOTO);
            codes[curCode++] = ic2;
            InterCode ic3 = make1op(LABEL, l2, I_LABEL);
            codes[curCode++] = ic3;
            translate_Stmt(p->nodebrothers->nodebrothers->nodebrothers->nodebrothers->nodebrothers->nodebrothers);
            InterCode ic4 = make1op(LABEL, l3, I_LABEL);
            codes[curCode++] = ic4;
        }
        else //if
        {
            int l1, l2;
            labelNum++;
            l1 = labelNum;
            labelNum++;
            l2 = labelNum;
            translate_Cond(p->nodebrothers->nodebrothers, l1, l2);
            InterCode ic1 = make1op(LABEL, l1, I_LABEL);
            codes[curCode++] = ic1;
            translate_Stmt(p->nodebrothers->nodebrothers->nodebrothers->nodebrothers);
            InterCode ic2 = make1op(LABEL, l2, I_LABEL);
            codes[curCode++] = ic2;
        }
    }
    else if (strcmp(ptype, "WHILE") == 0)
    {
        int l1, l2, l3;
        labelNum++;
        l1 = labelNum;
        labelNum++;
        l2 = labelNum;
        labelNum++;
        l3 = labelNum;
        InterCode ic1 = make1op(LABEL, l1, I_LABEL);
        codes[curCode++] = ic1;
        translate_Cond(p->nodebrothers->nodebrothers, l2, l3);
        InterCode ic2 = make1op(LABEL, l2, I_LABEL);
        codes[curCode++] = ic2;
        translate_Stmt(p->nodebrothers->nodebrothers->nodebrothers->nodebrothers);
        InterCode ic3 = make1op(LABEL, l1, GOTO);
        codes[curCode++] = ic3;
        InterCode ic4 = make1op(LABEL, l3, I_LABEL);
        codes[curCode++] = ic4;
    }
}
void translate_CompSt(Infonode *now)
{
    translate_DefList(now->nodechild->nodebrothers);
    translate_StmtList(now->nodechild->nodebrothers->nodebrothers);
}
void translate_StmtList(Infonode *now)
{
    if (now->nodechild)
    {
        translate_Stmt(now->nodechild);
        translate_StmtList(now->nodechild->nodebrothers);
    }
}
void translate_Program(Infonode *now,char* irFile)
{
    for (int k = 0; k < MAX_LINE; k++)
        codes[k] = NULL;
    translate_ExtDefList(now->nodechild);
    FILE* irf=fopen(irFile,"w+");
    if(irf==NULL)
    {
        printf("IR file open error!");
        return;
    }
    int i = 0;
    while (codes[i])
        fprintf(irf,"%s",printIC(codes[i++]));
    fclose(irf);
}
void translate_ExtDefList(Infonode *now)
{
    if (!now)
        return;
    if (now->nodechild)
    {
        translate_ExtDef(now->nodechild);
        translate_ExtDefList(now->nodechild->nodebrothers);
    }
}
void translate_ExtDef(Infonode *now)
{
    if (strcmp(now->nodechild->nodebrothers->name, "SEMI") != 0)
    {
        translate_FunDec(now->nodechild->nodebrothers);
        translate_CompSt(now->nodechild->nodebrothers->nodebrothers);
    }
}
void translate_FunDec(Infonode *now)
{
    InterCode ic = make1op(FUNC, 0, I_FUNC);
    ic->u.oneop.op->u.name = now->nodechild->value;
    codes[curCode++] = ic;
    //varlist?
    if (strcmp(now->nodechild->nodebrothers->nodebrothers->name, "VarList") == 0)
    {
        FunctionList func = checknameFunc(now->nodechild->value);
        FieldList parameters = func->tail;
        while (parameters)
        {
            if (parameters->type->kind == BASIC || parameters->type->kind == STRUCTURE)
            {
                int v = (++var_no);
                parameters->assigned = 1; //FIX??
                parameters->val = v;
                InterCode pdec = make1op(VARIABLE, v, PARAM);
                codes[curCode++] = pdec;
                parameters = parameters->tail;
            }
            else
            {
                printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
                exit(-1);
				return;
            }
        }
    }
}