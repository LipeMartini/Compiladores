//

#include "symbols.hpp"
#include <string>
#include <map>

map<string, SYMBOL*> SymbolTable;

SYMBOL *symbolInstert (int type, char *text)
{
    if (SymbolTable.find(string(text)) != SymbolTable.end())
        return SymbolTable.find(string(text))->second;
    SYMBOL *newsymbol = new SYMBOL(type, string(text));
    SymbolTable[string(text)] = newsymbol;
    return newsymbol;
}

void symbolPrintTable(void)
{
    for (auto s: SymbolTable)
    {
        printf("Symbol[%d,%s]\n", s.second->type, s.second->text.c_str());
    }
}
