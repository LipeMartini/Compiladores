// ufrgs - Compiladores 2025-1
// Autoria: Luís Filipe Martini Gastmann
// symbols.hpp - tabela de simbolos

#ifndef SYMBOLS_HPP
#define SYMBOLS_HPP

#include <string>

class Symbol {
    public:
        int token; // tipo do token associado ao símbolo (ex: KW_BYTE, KW_INT, ...)
        int type;
        std::string text;
        
        Symbol(int t, std::string s) : token(0), type(t), text(s) {}
};

Symbol* symbolInsert(int type, const char* text);
void symbolPrintTable(void);

// END OF FILE

#endif // SYMBOLS_HPP
