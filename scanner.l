/*
 *  UFRGS - Compiladores - 2025-1
 *  Autoria: Luís Filipe Martini Gastmann
 */

%{

#include "parser.tab.hpp"
#include "symbols.hpp"
#include <stdio.h>

int lineNumber = 1;
int running = 1;
%}

%x COMMENT

%%

byte                    return KW_BYTE;
real                    return KW_REAL;
int                     return KW_INT;
if                      return KW_IF;
else                    return KW_ELSE;
do                      return KW_DO;
while                   return KW_WHILE;
read                    return KW_READ;
print                   return KW_PRINT;
return                  return KW_RETURN;
string                  return KW_STRING;
char                    return KW_CHAR;

[a-zA-Z_][a-zA-Z0-9_]*  { symbolInsert(TK_IDENTIFIER, yytext); return TK_IDENTIFIER; }

[0-9]+\.[0-9]+         { symbolInsert(LIT_REAL, yytext); return LIT_REAL; }
[0-9]+                 { symbolInsert(LIT_INT, yytext); return LIT_INT; }
'.'                     { symbolInsert(LIT_CHAR, yytext); return LIT_CHAR; }
\"[^"\n]*\"            { symbolInsert(LIT_STRING, yytext); return LIT_STRING; }

"<="                    return OPERATOR_LE;
">="                    return OPERATOR_GE;
"=="                    return OPERATOR_EQ;
"!="                    return OPERATOR_DIF;

[-,;()\[\]{}=+*/<>!&|~%]  return yytext[0];

[ \t]+                  ; /* ignore whitespace */
\n                      { lineNumber++; }
\r                      ; /* ignore carriage return */

"//".*                  ; /* ignore single-line comments */

"/--"                   BEGIN(COMMENT);
<COMMENT>[^-\n]*       ; /* eat anything that's not a '-' */
<COMMENT>-+[^-/\n]*    ; /* eat up '-'s not followed by '/' */
<COMMENT>\n            { lineNumber++; }
<COMMENT>"--/"         BEGIN(INITIAL);
<COMMENT>.             ; /* ignore any other character in comment */

.                       return TOKEN_ERROR;

%%

int getLineNumber(void) {
    return lineNumber;
}

int isRunning(void) {
    return running;
}

int yywrap() {
    running = 0;
    return 1;
}

void initMe(void) {

}
