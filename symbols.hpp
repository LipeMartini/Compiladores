// ufrgs - Compiladores 2025-1
// Autoria: Luís Filipe Martini Gastmann
// symbols.hpp - tabela de simbolos

#ifndef SYMBOLS_HPP
#define SYMBOLS_HPP

#include <string>

class Symbol {
    public:
        int type;
        std::string text;
        
        Symbol(int t, std::string s) : type(t), text(s) {}
};

Symbol* symbolInsert(int type, char* text);
void symbolPrintTable(void);

// END OF FILE

#endif // SYMBOLS_HPP
