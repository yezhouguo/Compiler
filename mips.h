#ifndef MIPS
#define MIPS
#include "ic.h"
typedef struct memObject* memObj;
FILE* mipsF;

struct memObject {
    char* name;
    int stackP;
};

int joinObj(char* name, int size);
void startMIPS(char* filename);
char* v2MIPS(Operand op);
memObj checkObj(char* obname);
void lw(char* oname,int size,int reg);
void sw(char* oname,int size,int reg);
void MIPSLabel(InterCode code);
void MIPSAssignII(InterCode code);
void MIPSAssignIP(InterCode code);
void MIPSAssignPI(InterCode code);
void MIPSAdd(InterCode code);
void MIPSSub(InterCode code);
void MIPSMul(InterCode code);
void MIPSDiv(InterCode code);
void MIPSGoto(InterCode code);
void MIPSIfGoto(InterCode code);
void MIPSFunc(InterCode code);
void MIPSParam(InterCode code,int paramNum);
void MIPSReturn(InterCode code);
void MIPSDec(InterCode code);
void MIPSArgV(InterCode code);
void MIPSCall(InterCode code);
void MIPSRead(InterCode code);
void MIPSWrite(InterCode code);
void MIPSAddr(InterCode code);
#endif