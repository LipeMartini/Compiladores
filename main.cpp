#include <stdio.h>
#include <stdlib.h>

extern int lineNumber;
int yylex();

int main(void) {
    yylex();
    return 0;
}
