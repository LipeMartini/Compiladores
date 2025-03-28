// ufrgs - Compiladores 2025-1
// Autoria: Luís Filipe MArtini Gastmann
// symbols.hpp - tabela de simbolos

#include <string>

using namespace std;

string SymbolName[] = {
    LIERAL_INTEIRO, LITERAL_REAL//, SIMBOLOS... ex.: SYMBOL_WHATEVER, SYMBOL_FODASE, ...
};

struct symbols  
    {
        int type;
        string text;
        public:
        SYMBOL(int i, string s): type(t), text(s) {};
    };

SYMBOL *symbolInstert(int type, char *text);
void symbolPrintTable(void);

// END OF FILE
