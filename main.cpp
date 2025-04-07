#include <stdio.h>
#include <stdlib.h>
#include "symbols.hpp"

extern FILE *yyin;
extern int yyparse(void);

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Call: ./etapa2 input_file\n");
        exit(1);
    }
    
    if (!(yyin = fopen(argv[1], "r"))) {
        fprintf(stderr, "Error: Cannot open file %s\n", argv[1]);
        exit(2);
    }

    int ret = yyparse();
    
    printf("\nSymbol Table:\n");
    printf("-------------\n");
    symbolPrintTable();
    
    fclose(yyin);
    return ret;
}
