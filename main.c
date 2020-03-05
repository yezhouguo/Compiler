#include "ic.h"
#include "semantic.h"
#include "mips.h"
extern FILE* yyin;
extern int yylineno;
extern void yyrestart(FILE*);
extern void yyparse(void);
Infonode* syntaxTree;
//int height;
ErrorFlags ErrorF;
int main(int argc, char **argv) 
{
//    height=0;
	if (argc > 1) 
    {
        if (!(yyin=fopen(argv[1],"r"))) 
        {
            printf("ERROR\n");
			return -1;
		}
        ErrorF.errorA=0;
        ErrorF.errorB=0;
        ErrorF.errorC=0;
        yyrestart(yyin);
        //extern int yydebug;
        //yydebug = 1;
        //yylex();
        yyparse();
        //printf("%s\n",syntaxTree->nodechild->nodechild->name);
        if(ErrorF.errorA==0 && ErrorF.errorB==0)    
            Program(syntaxTree);
        if(ErrorF.errorA==0 && ErrorF.errorB==0 && ErrorF.errorC==0)    
        {
            //printTreePr(syntaxTree,0);
            if(argc > 2)
                translate_Program(syntaxTree,argv[2]);
            else
            {
                printf("IR FILE NOT GIVEN\n");
                return -1;
            }
            if(argc > 3) startMIPS(argv[3]);
            else
            {
                printf("OBJECT CODE FILE NOT GIVEN\n");
                return -2;
            }
            
        }
        //printf("++++\n");
        return 0;
    }
    return -1;
}
