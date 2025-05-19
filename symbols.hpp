// ufrgs - Compiladores 2025-1
// Autoria: Luís Filipe Martini Gastmann
// symbols.hpp - tabela de simbolos

#ifndef SYMBOLS_HPP
#define SYMBOLS_HPP

#include <string>
#include <vector>

// Definição dos tipos de natureza de símbolos
typedef enum {
    SYMBOL_SCALAR,    // Variável escalar
    SYMBOL_VECTOR,    // Vetor
    SYMBOL_FUNCTION   // Função
} SymbolNature;

// Definição dos tipos de dados
typedef enum {
    DATATYPE_UNDEFINED,
    DATATYPE_BYTE,
    DATATYPE_INT,
    DATATYPE_REAL,
    DATATYPE_BOOLEAN,
    DATATYPE_STRING,
    DATATYPE_CHAR
} DataType;

// Classe para parâmetros de função
class Parameter {
public:
    DataType dataType;
    std::string name;
    
    Parameter(DataType dt, std::string n) : dataType(dt), name(n) {}
};

class Symbol {
public:
    int token;                  // Tipo do token associado ao símbolo
    int type;                   // Tipo do símbolo
    std::string text;           // Texto do símbolo
    SymbolNature nature;        // Natureza do símbolo (escalar, vetor, função)
    DataType dataType;          // Tipo de dado do símbolo
    int vectorSize;             // Tamanho do vetor (se for vetor)
    std::vector<Parameter> parameters;  // Lista de parâmetros (se for função)
    DataType returnType;        // Tipo de retorno (se for função)
    bool isDeclared;            // Flag para indicar se o símbolo foi declarado
    
    Symbol(int t, std::string s) : 
        token(0), 
        type(t), 
        text(s), 
        nature(SYMBOL_SCALAR), 
        dataType(DATATYPE_UNDEFINED), 
        vectorSize(0), 
        returnType(DATATYPE_UNDEFINED), 
        isDeclared(false) {}
};

// Funções para manipulação de símbolos
Symbol* symbolInsert(int type, const char* text);
Symbol* symbolFind(const char* text);
Symbol* findFunction(const char* text);
void symbolPrintTable(void);

// Funções para verificação semântica
DataType getDataTypeFromToken(int token);
bool isTypeCompatible(DataType type1, DataType type2);
DataType getExpressionType(void* node);
bool checkVectorIndex(void* node);
bool checkFunctionCall(void* node);
bool checkDeclaration(void* node);
bool checkAssignment(void* node);
int getSemanticErrorCount(void);

// END OF FILE

#endif // SYMBOLS_HPP
