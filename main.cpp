#include <stdio.h>
#include <stdlib.h>
#include "symbols.hpp"
FILE* ast_output_file = nullptr;

extern FILE *yyin;
extern int yyparse(void);
extern void yyrestart(FILE*);
//extern FILE* ast_output_file;


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Call: ./etapa3 input_file [output_file]\n");
        exit(1);
    }

    FILE *yyin = fopen(argv[1], "r");
    if (!yyin) {
        fprintf(stderr, "Cannot open input file %s\n", argv[1]);
        exit(1);
    }
    FILE *out = stdout;
    if (argc >= 3) {
        out = fopen(argv[2], "w");
        if (!out) {
            fprintf(stderr, "Cannot open output file %s\n", argv[2]);
            fclose(yyin);
            exit(1);
        }
    }
    yyrestart(yyin);
    ast_output_file = out;
    int ret = yyparse();
    fclose(yyin);
    if (out != stdout) fclose(out);
    printf("\nSymbol Table:\n");
    printf("-------------\n");
    symbolPrintTable();
    return ret;
}
