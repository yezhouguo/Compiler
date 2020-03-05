#include "main.h"
//#define NULL 0
#ifndef nodeh
#define nodeh
typedef struct InfoNode
{
    struct InfoNode* nodeparent;
    struct InfoNode* nodechild;
    struct InfoNode* nodebrothers;
    char name[32];
    char value[32];
    int line;
}Infonode;
typedef struct ERRFLAG
{
    unsigned char errorA;
    unsigned char errorB;
    unsigned char errorC;
}ErrorFlags;

Infonode* createInfoNode(char* name,char* value,int line);
void addNode(Infonode *parent, Infonode *child);
void printTreePr(Infonode* root, int spaceNum);
#endif