/*
 *  UFRGS - Compiladores - 2025-1
 *  Autoria: Luís Filipe Martini Gastmann
 */

%{

#include "ast.h"
#include "parser.tab.hpp"
#include "symbols.hpp"
#include <stdio.h>

int lineNumber = 1;
int running = 1;
%}

%x COMMENT

%%

byte      { yylval.symbol = (void*)symbolInsert(KW_BYTE, yytext); ((Symbol*)yylval.symbol)->token = KW_BYTE; return KW_BYTE; }
real      { yylval.symbol = (void*)symbolInsert(KW_REAL, yytext); ((Symbol*)yylval.symbol)->token = KW_REAL; return KW_REAL; }
int       { yylval.symbol = (void*)symbolInsert(KW_INT, yytext); ((Symbol*)yylval.symbol)->token = KW_INT; return KW_INT; }
if                      return KW_IF;
else                    return KW_ELSE;
do                      return KW_DO;
while                   return KW_WHILE;
read                    return KW_READ;
print                   return KW_PRINT;
return                  return KW_RETURN;
string    { yylval.symbol = (void*)symbolInsert(KW_STRING, yytext); ((Symbol*)yylval.symbol)->token = KW_STRING; return KW_STRING; }
char      { yylval.symbol = (void*)symbolInsert(KW_CHAR, yytext); ((Symbol*)yylval.symbol)->token = KW_CHAR; return KW_CHAR; }

[a-zA-Z_][a-zA-Z0-9_]*  { yylval.symbol = (void*)symbolInsert(TK_IDENTIFIER, yytext); return TK_IDENTIFIER; }

[0-9]+\.[0-9]+         { yylval.symbol = (void*)symbolInsert(LIT_REAL, yytext); return LIT_REAL; }
[0-9]+                 { yylval.symbol = (void*)symbolInsert(LIT_INT, yytext); return LIT_INT; }
'.'                     { yylval.symbol = (void*)symbolInsert(LIT_CHAR, yytext); return LIT_CHAR; }
\"[^"\n]*\"            { yylval.symbol = (void*)symbolInsert(LIT_STRING, yytext); return LIT_STRING; }

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
