//

#include "symbols.hpp"
#include <string>
#include <map>
#include <cstdio>

static std::map<std::string, Symbol*> SymbolTable;

Symbol* symbolInsert(int type, char* text) {
    std::string key(text);
    auto it = SymbolTable.find(key);
    
    if (it != SymbolTable.end())
        return it->second;
        
    Symbol* newSymbol = new Symbol(type, key);
    SymbolTable[key] = newSymbol;
    return newSymbol;
}

void symbolPrintTable(void) {
    for (const auto& entry : SymbolTable) {
        printf("Symbol[%d,%s]\n", 
            entry.second->type, 
            entry.second->text.c_str());
    }
}
