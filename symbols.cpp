//
// symbols.cpp - Implementação da tabela de símbolos e funções de verificação semântica
//

#include "symbols.hpp"
#include "ast.h"
#include "parser.tab.hpp"
#include <string>
#include <map>
#include <cstdio>
#include <cstring>
#include <iostream>

// Tabela de símbolos global
static std::map<std::string, Symbol*> SymbolTable;

// Contador de erros semânticos
static int semanticErrors = 0;

// Função para inserir um símbolo na tabela
Symbol* symbolInsert(int type, const char* text) {
    std::string key(text);
    auto it = SymbolTable.find(key);
    
    if (it != SymbolTable.end())
        return it->second;
        
    Symbol* newSymbol = new Symbol(type, key);
    SymbolTable[key] = newSymbol;
    return newSymbol;
}

// Função para buscar um símbolo na tabela
Symbol* symbolFind(const char* text) {
    std::string key(text);
    auto it = SymbolTable.find(key);
    
    if (it != SymbolTable.end())
        return it->second;
    
    return nullptr;
}

// Função para buscar uma função na tabela de símbolos
Symbol* findFunction(const char* text) {
    std::string key(text);
    auto it = SymbolTable.find(key);
    
    if (it != SymbolTable.end() && it->second->nature == SYMBOL_FUNCTION)
        return it->second;
    
    return nullptr;
}

// Função para imprimir a tabela de símbolos
void symbolPrintTable(void) {
    printf("\n===== SYMBOL TABLE =====\n");
    for (const auto& entry : SymbolTable) {
        Symbol* symbol = entry.second;
        printf("Symbol[%s]: ", symbol->text.c_str());
        
        // Imprimir natureza do símbolo
        switch (symbol->nature) {
            case SYMBOL_SCALAR: printf("SCALAR, "); break;
            case SYMBOL_VECTOR: printf("VECTOR[%d], ", symbol->vectorSize); break;
            case SYMBOL_FUNCTION: printf("FUNCTION, "); break;
        }
        
        // Imprimir tipo de dado
        switch (symbol->dataType) {
            case DATATYPE_BYTE: printf("BYTE"); break;
            case DATATYPE_INT: printf("INT"); break;
            case DATATYPE_REAL: printf("REAL"); break;
            case DATATYPE_BOOLEAN: printf("BOOLEAN"); break;
            case DATATYPE_STRING: printf("STRING"); break;
            case DATATYPE_CHAR: printf("CHAR"); break;
            default: printf("UNDEFINED");
        }
        
        // Se for função, imprimir tipo de retorno e parâmetros
        if (symbol->nature == SYMBOL_FUNCTION) {
            printf(", RETURN: ");
            switch (symbol->returnType) {
                case DATATYPE_BYTE: printf("BYTE"); break;
                case DATATYPE_INT: printf("INT"); break;
                case DATATYPE_REAL: printf("REAL"); break;
                case DATATYPE_BOOLEAN: printf("BOOLEAN"); break;
                case DATATYPE_STRING: printf("STRING"); break;
                case DATATYPE_CHAR: printf("CHAR"); break;
                default: printf("UNDEFINED");
            }
            
            printf(", PARAMS: ");
            if (symbol->parameters.empty()) {
                printf("NONE");
            } else {
                for (size_t i = 0; i < symbol->parameters.size(); i++) {
                    if (i > 0) printf(", ");
                    switch (symbol->parameters[i].dataType) {
                        case DATATYPE_BYTE: printf("BYTE %s", symbol->parameters[i].name.c_str()); break;
                        case DATATYPE_INT: printf("INT %s", symbol->parameters[i].name.c_str()); break;
                        case DATATYPE_REAL: printf("REAL %s", symbol->parameters[i].name.c_str()); break;
                        case DATATYPE_BOOLEAN: printf("BOOLEAN %s", symbol->parameters[i].name.c_str()); break;
                        case DATATYPE_STRING: printf("STRING %s", symbol->parameters[i].name.c_str()); break;
                        case DATATYPE_CHAR: printf("CHAR %s", symbol->parameters[i].name.c_str()); break;
                        default: printf("UNDEFINED %s", symbol->parameters[i].name.c_str());
                    }
                }
            }
        }
        
        printf("\n");
    }
    printf("========================\n");
}

// Função para converter token de tipo para DataType
DataType getDataTypeFromToken(int token) {
    switch (token) {
        case KW_BYTE: return DATATYPE_BYTE;
        case KW_INT: return DATATYPE_INT;
        case KW_REAL: return DATATYPE_REAL;
        case KW_CHAR: return DATATYPE_CHAR;
        case KW_STRING: return DATATYPE_STRING;
        default: return DATATYPE_UNDEFINED;
    }
}

// Função para verificar compatibilidade de tipos
bool isTypeCompatible(DataType type1, DataType type2) {
    // Se algum dos tipos for indefinido, considerar compatível para evitar erros em cascata
    if (type1 == DATATYPE_UNDEFINED || type2 == DATATYPE_UNDEFINED)
        return true;
    
    // Tipos iguais são sempre compatíveis
    if (type1 == type2)
        return true;
    
    // Inteiros e caracteres são compatíveis entre si
    if ((type1 == DATATYPE_INT || type1 == DATATYPE_CHAR || type1 == DATATYPE_BYTE) &&
        (type2 == DATATYPE_INT || type2 == DATATYPE_CHAR || type2 == DATATYPE_BYTE))
        return true;
    
    // Outros tipos não são compatíveis
    return false;
}

// Função para obter o tipo de uma expressão
DataType getExpressionType(void* node) {
    if (!node) return DATATYPE_UNDEFINED;
    
    AST* ast = (AST*)node;
    
    switch (ast->type) {
        case AST_SYMBOL: {
            Symbol* symbol = (Symbol*)ast->symbol;
            if (!symbol) return DATATYPE_UNDEFINED;
            
            // Verificar se é um literal
            if (symbol->type == LIT_INT) return DATATYPE_INT;
            if (symbol->type == LIT_CHAR) return DATATYPE_CHAR;
            if (symbol->type == LIT_REAL) return DATATYPE_REAL;
            if (symbol->type == LIT_STRING) return DATATYPE_STRING;
            
            // Verificar se o símbolo foi declarado
            if (!symbol->isDeclared) {
                std::cerr << "Semantic error: Symbol '" << symbol->text << "' not declared" << std::endl;
                semanticErrors++;
                return DATATYPE_UNDEFINED;
            }
            
            return symbol->dataType;
        }
        
        case AST_OP: {
            const char* op = (const char*)ast->symbol;
            if (!op) return DATATYPE_UNDEFINED;
            
            // Obter tipos dos operandos
            DataType leftType = getExpressionType(ast->son[0]);
            DataType rightType = getExpressionType(ast->son[1]);
            
            // Operadores relacionais (< > <= >= == !=) retornam boolean
            if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
                strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0 ||
                strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
                
                // Verificar compatibilidade dos operandos
                if (!isTypeCompatible(leftType, rightType)) {
                    std::cerr << "Semantic error: Incompatible types in relational expression" << std::endl;
                    semanticErrors++;
                }
                
                return DATATYPE_BOOLEAN;
            }
            
            // Operadores lógicos (& |) retornam boolean e exigem operandos boolean
            if (strcmp(op, "&") == 0 || strcmp(op, "|") == 0) {
                if (leftType != DATATYPE_BOOLEAN || rightType != DATATYPE_BOOLEAN) {
                    std::cerr << "Semantic error: Logical operators require boolean operands" << std::endl;
                    semanticErrors++;
                }
                
                return DATATYPE_BOOLEAN;
            }
            
            // Operador de acesso a vetor (INDEX)
            if (strcmp(op, "INDEX") == 0) {
                // Verificar se o operando esquerdo é um vetor
                Symbol* symbol = nullptr;
                if (ast->son[0] && ast->son[0]->type == AST_SYMBOL)
                    symbol = (Symbol*)ast->son[0]->symbol;
                
                if (!symbol || symbol->nature != SYMBOL_VECTOR) {
                    std::cerr << "Semantic error: Indexed expression is not a vector" << std::endl;
                    semanticErrors++;
                    return DATATYPE_UNDEFINED;
                }
                
                // Verificar se o índice é inteiro
                DataType indexType = getExpressionType(ast->son[1]);
                if (indexType != DATATYPE_INT && indexType != DATATYPE_CHAR && indexType != DATATYPE_BYTE) {
                    std::cerr << "Semantic error: Vector index must be an integer" << std::endl;
                    semanticErrors++;
                }
                
                return symbol->dataType;
            }
            
            // Operadores aritméticos (+ - * / %)
            if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 ||
                strcmp(op, "*") == 0 || strcmp(op, "/") == 0 ||
                strcmp(op, "%") == 0) {
                
                // Verificar compatibilidade dos operandos
                if (!isTypeCompatible(leftType, rightType)) {
                    std::cerr << "Semantic error: Incompatible types in arithmetic expression" << std::endl;
                    semanticErrors++;
                    return DATATYPE_UNDEFINED;
                }
                
                // Se um dos operandos for real, o resultado é real
                if (leftType == DATATYPE_REAL || rightType == DATATYPE_REAL)
                    return DATATYPE_REAL;
                
                // Para o operador de divisão (/), o resultado pode ser real mesmo com operandos inteiros
                if (strcmp(op, "/") == 0)
                    return DATATYPE_REAL;
                
                // Caso contrário, o resultado é int
                return DATATYPE_INT;
            }
            
            return DATATYPE_UNDEFINED;
        }
        
        case AST_FUNC_CALL: {
            // Verificar se a função foi declarada
            Symbol* symbol = (Symbol*)ast->symbol;
            if (!symbol) return DATATYPE_UNDEFINED;
            
            // Verificar se a função existe na tabela de símbolos
            Symbol* funcSymbol = findFunction(symbol->text.c_str());
            
            // Se a função não existe, não reportar erro (pode ser declarada depois)
            // Retornar INT como tipo padrão para funções não declaradas
            if (!funcSymbol) {
                return DATATYPE_INT;
            }
            
            // Verificar argumentos da chamada de função
            checkFunctionCall(ast);
            
            return funcSymbol->returnType;
        }
        
        default:
            return DATATYPE_UNDEFINED;
    }
}

// Função para verificar índice de vetor
bool checkVectorIndex(void* node) {
    if (!node) return false;
    
    AST* ast = (AST*)node;
    
    // Verificar se o nó é uma operação de acesso a vetor
    if (ast->type != AST_OP || !ast->symbol || strcmp((const char*)ast->symbol, "INDEX") != 0)
        return false;
    
    // Verificar se o operando esquerdo é um vetor
    Symbol* symbol = nullptr;
    if (ast->son[0] && ast->son[0]->type == AST_SYMBOL)
        symbol = (Symbol*)ast->son[0]->symbol;
    
    if (!symbol) return false;
    
    // Se o símbolo não foi declarado, não verificar mais (já foi reportado em outro lugar)
    if (!symbol->isDeclared) return false;
    
    // Verificar se é um vetor
    if (symbol->nature != SYMBOL_VECTOR) {
        std::cerr << "Semantic error: Indexed expression is not a vector" << std::endl;
        semanticErrors++;
        return false;
    }
    
    // Verificar se o índice é inteiro
    DataType indexType = getExpressionType(ast->son[1]);
    if (indexType != DATATYPE_INT && indexType != DATATYPE_CHAR && indexType != DATATYPE_BYTE) {
        std::cerr << "Semantic error: Vector index must be an integer" << std::endl;
        semanticErrors++;
        return false;
    }
    
    return true;
}

// Esta função foi movida para o início do arquivo

// Função para verificar chamada de função
bool checkFunctionCall(void* node) {
    if (!node) return false;
    
    AST* ast = (AST*)node;
    
    // Verificar se o nó é uma chamada de função
    if (ast->type != AST_FUNC_CALL)
        return false;
    
    // Verificar se a função foi declarada
    Symbol* symbol = (Symbol*)ast->symbol;
    if (!symbol) return false;
    
    // Verificar se a função existe na tabela de símbolos
    Symbol* funcSymbol = findFunction(symbol->text.c_str());
    
    // Se a função não existe, reportar erro
    if (!funcSymbol) {
        // Verificar se existe algum símbolo com esse nome
        Symbol* anySymbol = symbolFind(symbol->text.c_str());
        if (anySymbol) {
            // Se existe um símbolo com esse nome, mas não é uma função
            std::cerr << "Semantic error: Symbol '" << symbol->text << "' is not a function" << std::endl;
        } else {
            // Se não existe nenhum símbolo com esse nome
            std::cerr << "Semantic error: Function '" << symbol->text << "' not declared" << std::endl;
        }
        semanticErrors++;
        return false;
    }
    
    // Se o símbolo não é uma função, reportar erro
    if (funcSymbol->nature != SYMBOL_FUNCTION) {
        std::cerr << "Semantic error: Symbol '" << symbol->text << "' is not a function" << std::endl;
        semanticErrors++;
        return false;
    }
    
    // Se não houver parâmetros, não verificar argumentos
    if (funcSymbol->parameters.empty() && !ast->son[0]) {
        return true;
    }
    
    // Contar argumentos
    int argCount = 0;
    AST* arg = ast->son[0];
    while (arg) {
        argCount++;
        arg = arg->next;
    }
    
    // Verificar número de argumentos
    if (argCount != (int)funcSymbol->parameters.size()) {
        std::cerr << "Semantic error: Function '" << symbol->text << "' called with wrong number of arguments" << std::endl;
        semanticErrors++;
        return false;
    }
    
    // Verificar tipos dos argumentos
    arg = ast->son[0];
    for (size_t i = 0; i < funcSymbol->parameters.size() && arg; i++) {
        DataType argType = getExpressionType(arg);
        DataType paramType = funcSymbol->parameters[i].dataType;
        
        // Verificar compatibilidade de tipos mesmo se algum tipo for indefinido
        if (argType != DATATYPE_UNDEFINED && paramType != DATATYPE_UNDEFINED && 
            !isTypeCompatible(argType, paramType)) {
            std::cerr << "Semantic error: Incompatible argument type for parameter '" << funcSymbol->parameters[i].name << "'" << std::endl;
            semanticErrors++;
        }
        
        arg = arg->next;
    }
    
    return true;
}

// Função para verificar declaração
bool checkDeclaration(void* node) {
    if (!node) return false;
    
    AST* ast = (AST*)node;
    
    // Verificar se o nó é uma declaração de variável ou função
    if (ast->type != AST_VAR_DECL && ast->type != AST_FUNC_DECL)
        return false;
    
    Symbol* symbol = (Symbol*)ast->symbol;
    if (!symbol) return false;
    
    // Verificar se o símbolo já foi declarado
    if (symbol->isDeclared) {
        std::cerr << "Semantic error: Symbol '" << symbol->text << "' already declared" << std::endl;
        semanticErrors++;
        return false;
    }
    
    // Marcar como declarado
    symbol->isDeclared = true;
    
    // Configurar tipo de dado baseado no tipo_symbol
    if (ast->type_symbol) {
        Symbol* typeSymbol = (Symbol*)ast->type_symbol;
        symbol->dataType = getDataTypeFromToken(typeSymbol->token);
    }
    
    // Configurar natureza e informações adicionais
    if (ast->type == AST_VAR_DECL) {
        // Verificar se é um vetor ou escalar
        if (ast->son[0] && !ast->son[1]) {
            // Verificar se é um vetor ou variável escalar com inicialização
            if (ast->son[0]->type == AST_SYMBOL && 
                ((Symbol*)ast->son[0]->symbol)->type == LIT_INT && 
                ast->son[0]->next) {
                // É uma declaração de vetor sem inicialização: int v[10];
                symbol->nature = SYMBOL_VECTOR;
                
                // Obter tamanho do vetor
                Symbol* sizeSymbol = (Symbol*)ast->son[0]->symbol;
                if (sizeSymbol) {
                    try {
                        symbol->vectorSize = std::stoi(sizeSymbol->text);
                    } catch (...) {
                        symbol->vectorSize = 0;
                    }
                }
            } else {
                // Verificar se é um vetor sem inicialização: int v[10];
                AST* sizeNode = ast->son[0];
                Symbol* sizeSymbol = nullptr;
                
                if (sizeNode->type == AST_SYMBOL && sizeNode->symbol) {
                    sizeSymbol = (Symbol*)sizeNode->symbol;
                    if (sizeSymbol->token == LIT_INT) {
                        // É um vetor com tamanho definido por literal inteiro
                        symbol->nature = SYMBOL_VECTOR;
                        symbol->vectorSize = std::stoi(sizeSymbol->text);
                    } else {
                        // É uma variável escalar com inicialização
                        symbol->nature = SYMBOL_SCALAR;
                        
                        // Verificar inicialização
                        DataType initType = getExpressionType(ast->son[0]);
                        if (initType != DATATYPE_UNDEFINED && symbol->dataType != DATATYPE_UNDEFINED && 
                            !isTypeCompatible(symbol->dataType, initType)) {
                            std::cerr << "Semantic error: Incompatible initialization for '" << symbol->text << "'" << std::endl;
                            semanticErrors++;
                        }
                    }
                } else {
                    // Verificar se o tamanho do vetor é um inteiro
                    DataType sizeType = getExpressionType(ast->son[0]);
                    if (sizeType != DATATYPE_UNDEFINED && sizeType != DATATYPE_INT && 
                        sizeType != DATATYPE_BYTE && sizeType != DATATYPE_CHAR) {
                        std::cerr << "Semantic error: Vector size must be an integer for '" << symbol->text << "'" << std::endl;
                        semanticErrors++;
                    }
                    
                    // É um vetor com tamanho definido por expressão
                    symbol->nature = SYMBOL_VECTOR;
                }
            }
        } else {
            // Variável escalar sem inicialização: int a;
            symbol->nature = SYMBOL_SCALAR;
        }
    } else if (ast->type == AST_FUNC_DECL) {
        // Função
        symbol->nature = SYMBOL_FUNCTION;
        symbol->returnType = symbol->dataType;
        
        // Processar parâmetros
        AST* param = ast->son[0];
        while (param) {
            Symbol* paramSymbol = (Symbol*)param->symbol;
            Symbol* paramTypeSymbol = (Symbol*)param->type_symbol;
            
            if (paramSymbol && paramTypeSymbol) {
                DataType paramType = getDataTypeFromToken(paramTypeSymbol->token);
                symbol->parameters.push_back(Parameter(paramType, paramSymbol->text));
                
                // Adicionar parâmetro na tabela de símbolos
                Symbol* paramEntry = symbolInsert(paramSymbol->type, paramSymbol->text.c_str());
                paramEntry->isDeclared = true;
                paramEntry->dataType = paramType;
                paramEntry->nature = SYMBOL_SCALAR;
            }
            
            param = param->next;
        }
    }
    
    return true;
}

// Função para verificar atribuição
bool checkAssignment(void* node) {
    if (!node) return false;
    
    AST* ast = (AST*)node;
    
    // Verificar se o nó é uma atribuição
    if (ast->type != AST_ASSIGN)
        return false;
    
    Symbol* symbol = (Symbol*)ast->symbol;
    if (!symbol) return false;
    
    // Verificar se o símbolo foi declarado
    if (!symbol->isDeclared) {
        std::cerr << "Semantic error: Symbol '" << symbol->text << "' not declared" << std::endl;
        semanticErrors++;
        return false;
    }
    
    // Verificar natureza do símbolo
    if (symbol->nature == SYMBOL_FUNCTION) {
        std::cerr << "Semantic error: Cannot assign to function '" << symbol->text << "'" << std::endl;
        semanticErrors++;
        return false;
    }
    
    DataType leftType, rightType;
    
    // Verificar se é atribuição a vetor ou escalar
    if (ast->son[0] && ast->son[1]) {
        // Atribuição a vetor: a[i] = x
        if (symbol->nature != SYMBOL_VECTOR) {
            std::cerr << "Semantic error: Indexed assignment to non-vector '" << symbol->text << "'" << std::endl;
            semanticErrors++;
            return false;
        }
        
        // Verificar se o índice é inteiro
        DataType indexType = getExpressionType(ast->son[0]);
        if (indexType != DATATYPE_UNDEFINED && indexType != DATATYPE_INT && 
            indexType != DATATYPE_CHAR && indexType != DATATYPE_BYTE) {
            std::cerr << "Semantic error: Vector index must be an integer" << std::endl;
            semanticErrors++;
        }
        
        leftType = symbol->dataType;
        rightType = getExpressionType(ast->son[1]);
    } else {
        // Atribuição a escalar: a = x
        if (symbol->nature != SYMBOL_SCALAR) {
            std::cerr << "Semantic error: Direct assignment to non-scalar '" << symbol->text << "'" << std::endl;
            semanticErrors++;
            return false;
        }
        
        leftType = symbol->dataType;
        rightType = getExpressionType(ast->son[0]);
    }
    
    // Verificar compatibilidade de tipos (ignorar se algum tipo for indefinido)
    if (leftType != DATATYPE_UNDEFINED && rightType != DATATYPE_UNDEFINED && 
        !isTypeCompatible(leftType, rightType)) {
        std::cerr << "Semantic error: Incompatible types in assignment to '" << symbol->text << "'" << std::endl;
        semanticErrors++;
        return false;
    }
    
    return true;
}

// Função para verificar condições em estruturas de controle
bool checkControlStructure(void* node) {
    if (!node) return false;
    
    AST* ast = (AST*)node;
    
    // Verificar se o nó é uma estrutura de controle
    if (ast->type != AST_IF && ast->type != AST_IF_ELSE && 
        ast->type != AST_WHILE && ast->type != AST_DO_WHILE)
        return false;
    
    // Obter nó da condição
    AST* condition = nullptr;
    switch (ast->type) {
        case AST_IF:
        case AST_IF_ELSE:
        case AST_WHILE:
            condition = ast->son[0];
            break;
        case AST_DO_WHILE:
            condition = ast->son[1];
            break;
        default:
            return false;
    }
    
    if (!condition) return false;
    
    // Verificar se a condição é booleana ou pode ser convertida para booleana
    DataType condType = getExpressionType(condition);
    if (condType != DATATYPE_UNDEFINED && condType != DATATYPE_BOOLEAN && 
        condType != DATATYPE_INT && condType != DATATYPE_BYTE && condType != DATATYPE_CHAR) {
        std::cerr << "Semantic error: Condition must be a boolean or numeric expression" << std::endl;
        semanticErrors++;
        return false;
    }
    
    return true;
}

// Função para verificar tipos de retorno em funções
bool checkReturn(void* node) {
    if (!node) return false;
    
    AST* ast = (AST*)node;
    
    // Verificar se o nó é um comando de retorno
    if (ast->type != AST_RETURN)
        return false;
    
    // Obter a função atual
    // Isso é uma simplificação, idealmente deveríamos ter um escopo para saber em qual função estamos
    AST* parent = ast;
    while (parent && parent->type != AST_FUNC_DECL) {
        // Tentar encontrar o nó pai que é uma declaração de função
        // Isso é uma simplificação e pode não funcionar em todos os casos
        parent = (AST*)parent->symbol; // Usar symbol como ponteiro para o pai (simplificação)
    }
    
    if (!parent || !parent->symbol) return false;
    
    Symbol* funcSymbol = (Symbol*)parent->symbol;
    if (funcSymbol->nature != SYMBOL_FUNCTION) return false;
    
    // Verificar se o tipo de retorno é compatível com o tipo da função
    if (ast->son[0]) {
        DataType returnType = getExpressionType(ast->son[0]);
        if (returnType != DATATYPE_UNDEFINED && funcSymbol->returnType != DATATYPE_UNDEFINED && 
            !isTypeCompatible(funcSymbol->returnType, returnType)) {
            std::cerr << "Semantic error: Incompatible return type in function '" << funcSymbol->text << "'" << std::endl;
            semanticErrors++;
            return false;
        }
    } else if (funcSymbol->returnType != DATATYPE_UNDEFINED) {
        // Retorno vazio em função que deveria retornar um valor
        std::cerr << "Semantic error: Missing return value in function '" << funcSymbol->text << "'" << std::endl;
        semanticErrors++;
        return false;
    }
    
    return true;
}

// Função para obter o número de erros semânticos
int getSemanticErrorCount() {
    return semanticErrors;
}
