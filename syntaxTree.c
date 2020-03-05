#include "node.h"

Infonode* createInfoNode(char* name,char* value,int line)
{
    Infonode* infonode = (Infonode*)malloc(sizeof(Infonode));
    //if(infonode==NULL) return 1;
    infonode->nodeparent=NULL;
    infonode->nodechild=NULL;
    infonode->nodebrothers=NULL;
    strcpy(infonode->name,name);
    if(strcmp(value ,"  ")!=0)  strcpy(infonode->value,value);
    else strcpy(infonode->value,"shouldn'tusethis");
    infonode->line=line;
    //printf("%s\n",infonode->name);
    return infonode;
}
void addNode(Infonode *parent, Infonode *child)
{
    //printf("addNODE");
    if(parent->nodechild==NULL) parent->nodechild=child;
    else
    {
        Infonode *p = parent->nodechild;
		while (p->nodebrothers != NULL)	p = p->nodebrothers;
		p->nodebrothers = child;
    }
    child->nodeparent = parent;
}
int checkType(char* nodename)
{
    if(strcmp("Program",nodename)==0) return 1;
    if(strcmp("ExtDefList",nodename)==0) return 1;
    if(strcmp("ExtDef",nodename)==0) return 1;
    if(strcmp("ExtDecList",nodename)==0) return 1;
    if(strcmp("Specifier",nodename)==0) return 1;
    if(strcmp("StructSpecifier",nodename)==0) return 1;
    if(strcmp("OptTag",nodename)==0) return 1;
    if(strcmp("Tag",nodename)==0) return 1;
    if(strcmp("VarDec",nodename)==0) return 1;
    if(strcmp("FunDec",nodename)==0) return 1;
    if(strcmp("VarList",nodename)==0) return 1;
    if(strcmp("ParamDec",nodename)==0) return 1;
    if(strcmp("CompSt",nodename)==0) return 1;
    if(strcmp("StmtList",nodename)==0) return 1;
    if(strcmp("Stmt",nodename)==0) return 1;
    if(strcmp("DefList",nodename)==0) return 1;
    if(strcmp("Def",nodename)==0) return 1;
    if(strcmp("DecList",nodename)==0) return 1;
    if(strcmp("Dec",nodename)==0) return 1;
    if(strcmp("Exp",nodename)==0) return 1;
    if(strcmp("Args",nodename)==0) return 1;
    return 0;


}
void printTreePr(Infonode* root, int spaceNum)
{
    if (root==NULL ||
            (root->nodechild==NULL && strcmp(root->value,"`")==0))
    		return;
	for (int i = 0; i < spaceNum; i++)
		printf("  ");
	printf("%s", root->name);
	if (strcmp(root->value, "shouldn'tusethis") != 0) {
		printf(": %s\n", root->value);
	} else {
		if(checkType(root->name)) printf(" (%d)",root->line);
        printf("\n");
	}
	Infonode *node = root->nodechild;
    //if(node!=NULL) height++;
	while (node != NULL) {
     //   height++;
		printTreePr(node,spaceNum+1);
    //    height--;
		node = node->nodebrothers;
	}
    //height--;
}