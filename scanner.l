/*
    UFRGS - 2025-1
    Trabalho 1 - Compiladores
    Autoria: Luís Filipe Martini Gastmann
 */

#define KW_BYTE           256
#define KW_INT            257
#define KW_REAL           258

#define KW_IF             261
#define KW_ELSE           263
#define KW_DO             264
#define KW_WHILE          265
#define KW_READ           266
#define KW_PRINT          267
#define KW_RETURN         268

#define OPERATOR_LE       270
#define OPERATOR_GE       271
#define OPERATOR_EQ       272
#define OPERATOR_DIF      273

#define TK_IDENTIFIER     280

#define LIT_INT           281
#define LIT_CHAR          282
#define LIT_REAL          283
#define LIT_STRING        286

#define TOKEN_ERROR       290

int lineNumber = 1;

%%

[a-z]               { [0-9]+ { yylval.sIndex = *yytext - 'a'; return VARIABLE; } } yylval.iValue = atoi(yytext); return INTEGER; 
[-()>=+*/;{}.]      {return *yytext;} 
">="    return GE; 
"<="    return LE; 
"=="    return EQ; 
"!="    return NE; 
"byte"  return KW_BYTE;
"int"   return KW_IF;
"real"  return KW_REAL;
"if"    return KW_IF;
"else"  return KW_ELSE;
"do"    return KW_DO;
"while" return KW_WHILE;
"read"  return KW_READ;
"print" return KW_PRINT;
"return" return KW_RETURN;
"\n"    lineNumber++;

"//"[a-z0-9 -()!<>=+*/;{}.]*;

"/--" {BEGIN(COMMENT);}

. { return TOKEN_ERROR; }

<COMMENT> \n {++lineNumber};
<COMMENT> "--/" {BEGIN(INITIAL);}
<COMMENT> 

%%

int getLineNumber() { 
    return lineNumber;
}

int main() {
    return 0;
}

int yywrap(void) {
    return 1;
} 
