#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
#include "symbols.hpp"

extern int lineNumber;
int yylex(void);
extern char *yytext;
extern FILE *yyin;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Call: ./etapa1 input_file\n");
        exit(1);
    }
    
    if (!(yyin = fopen(argv[1], "r"))) {
        fprintf(stderr, "Error: Cannot open file %s\n", argv[1]);
        exit(2);
    }
    
    int token = 0;
    while ((token = yylex()) != 0) {
        switch(token) {
            case KW_BYTE: printf("Keyword: BYTE\n"); break;
            case KW_INT: printf("Keyword: INT\n"); break;
            case KW_REAL: printf("Keyword: REAL\n"); break;
            case KW_STRING: printf("Keyword: STRING\n"); break;
            case KW_CHAR: printf("Keyword: CHAR\n"); break;
            case KW_IF: printf("Keyword: IF\n"); break;
            case KW_ELSE: printf("Keyword: ELSE\n"); break;
            case KW_DO: printf("Keyword: DO\n"); break;
            case KW_WHILE: printf("Keyword: WHILE\n"); break;
            case KW_READ: printf("Keyword: READ\n"); break;
            case KW_PRINT: printf("Keyword: PRINT\n"); break;
            case KW_RETURN: printf("Keyword: RETURN\n"); break;
            case TK_IDENTIFIER: printf("Identifier: %s\n", yytext); break;
            case LIT_INT: printf("Int literal: %s\n", yytext); break;
            case LIT_REAL: printf("Real literal: %s\n", yytext); break;
            case LIT_CHAR: printf("Char literal: %s\n", yytext); break;
            case LIT_STRING: printf("String literal: %s\n", yytext); break;
            case TOKEN_ERROR: printf("Error at line %d: %s\n", lineNumber, yytext); break;
            case OPERATOR_LE: printf("Operator: <=\n"); break;
            case OPERATOR_GE: printf("Operator: >=\n"); break;
            case OPERATOR_EQ: printf("Operator: ==\n"); break;
            case OPERATOR_DIF: printf("Operator: !=\n"); break;
            default: printf("Special char: %c\n", token); break;
        }
    }
    
    printf("\nSymbol Table:\n");
    printf("-------------\n");
    symbolPrintTable();
    printf("\nTotal lines: %d\n", lineNumber);

    int tok;
    
    yyparse();
    
    fclose(yyin);


    return 0;
}
