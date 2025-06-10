//
// tacs.cpp - Implementação das estruturas de TACs (Three-Address Codes)
//

#include "tacs.hpp"
#include "ast.h"
#include "symbols.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

// Contador para temporários e labels
static int temp_count = 0;
static int label_count = 0;

// Funções para criar símbolos temporários e labels
char* makeTemp() {
    char* temp = (char*)malloc(16);
    sprintf(temp, "_temp%d", temp_count++);
    return temp;
}

char* makeLabel() {
    char* label = (char*)malloc(16);
    sprintf(label, "_label%d", label_count++);
    return label;
}

// Função para criar uma TAC
TAC* tacCreate(TacType type, void* res, void* op1, void* op2) {
    TAC* tac = (TAC*)malloc(sizeof(TAC));
    tac->type = type;
    tac->res = res;
    tac->op1 = op1;
    tac->op2 = op2;
    tac->prev = NULL;
    tac->next = NULL;
    return tac;
}

// Função para juntar duas listas de TACs
TAC* tacJoin(TAC* l1, TAC* l2) {
    if (!l1) return l2;
    if (!l2) return l1;
    
    TAC* tac = l2;
    while (tac->prev) {
        tac = tac->prev;
    }
    tac->prev = l1;
    
    tac = l1;
    while (tac->next) {
        tac = tac->next;
    }
    tac->next = l2;
    
    return l1;
}

// Função auxiliar para imprimir um símbolo de forma segura
void printSymbol(void* symbol) {
    if (!symbol) {
        printf("NULL");
        return;
    }
    
    Symbol* sym = (Symbol*)symbol;
    printf("%s", sym->text.c_str());
}

// Função para imprimir uma TAC
void tacPrint(TAC* tac) {
    if (!tac) return;
    
    printf("TAC(");
    
    switch (tac->type) {
        case TAC_SYMBOL:
            printf("SYMBOL");
            break;
        case TAC_MOVE:
            printf("MOVE");
            break;
        case TAC_ADD:
            printf("ADD");
            break;
        case TAC_SUB:
            printf("SUB");
            break;
        case TAC_MUL:
            printf("MUL");
            break;
        case TAC_DIV:
            printf("DIV");
            break;
        case TAC_LT:
            printf("LT");
            break;
        case TAC_GT:
            printf("GT");
            break;
        case TAC_LE:
            printf("LE");
            break;
        case TAC_GE:
            printf("GE");
            break;
        case TAC_EQ:
            printf("EQ");
            break;
        case TAC_NE:
            printf("NE");
            break;
        case TAC_LABEL:
            printf("LABEL");
            break;
        case TAC_BEGINFUN:
            printf("BEGINFUN");
            break;
        case TAC_ENDFUN:
            printf("ENDFUN");
            break;
        case TAC_IFZ:
            printf("IFZ");
            break;
        case TAC_JUMP:
            printf("JUMP");
            break;
        case TAC_CALL:
            printf("CALL");
            break;
        case TAC_ARG:
            printf("ARG");
            break;
        case TAC_RET:
            printf("RET");
            break;
        case TAC_PRINT:
            printf("PRINT");
            break;
        case TAC_READ:
            printf("READ");
            break;
        case TAC_VECTOR_INDEX:
            printf("VECTOR_INDEX");
            break;
        case TAC_VECTOR_ASSIGN:
            printf("VECTOR_ASSIGN");
            break;
        default:
            printf("UNKNOWN");
    }
    
    printf(", ");
    printSymbol(tac->res);
    
    printf(", ");
    printSymbol(tac->op1);
    
    printf(", ");
    printSymbol(tac->op2);
    
    printf(")\n");
}

// Função para imprimir uma lista de TACs de trás para frente
void tacPrintBackwards(TAC* tac) {
    if (!tac) return;
    
    // Encontrar o último TAC da lista
    TAC* last = tac;
    while (last->next) {
        last = last->next;
    }
    
    // Imprimir a partir do último até o primeiro
    TAC* current = last;
    while (current) {
        tacPrint(current);
        current = current->prev;
    }
}

// Função para imprimir uma lista de TACs para frente
void tacPrintForward(TAC* tac) {
    if (!tac) return;
    tacPrint(tac);
    tacPrintForward(tac->next);
}

// Funções auxiliares para geração de código

// Gera código para expressões
TAC* generateCodeExpr(AST* node) {
    if (!node) return NULL;
    
    // Caso base: símbolo (variável ou literal)
    if (node->type == AST_SYMBOL) {
        return tacCreate(TAC_SYMBOL, node->symbol, NULL, NULL);
    }
    
    // Caso para acesso a vetor: a[i]
    if (node->type == AST_OP && strcmp((char*)node->symbol, "INDEX") == 0) {
        TAC* index = generateCode(node->son[1]);
        TAC* vector = generateCode(node->son[0]);
        
        Symbol* temp = symbolInsert(0, makeTemp());
        temp->dataType = ((Symbol*)node->son[0]->symbol)->dataType;
        
        TAC* vectorIndex = tacCreate(TAC_VECTOR_INDEX, temp, vector->res, index->res);
        return tacJoin(tacJoin(index, vector), vectorIndex);
    }
    
    // Caso para chamada de função
    if (node->type == AST_FUNC_CALL) {
        TAC* args = NULL;
        AST* arg = node->son[0];
        int argCount = 0;
        
        // Processa argumentos
        while (arg) {
            TAC* argCode = generateCode(arg);
            TAC* argTac = tacCreate(TAC_ARG, NULL, argCode->res, NULL);
            args = tacJoin(args, tacJoin(argCode, argTac));
            arg = arg->next;
            argCount++;
        }
        
        // Cria temporário para resultado
        Symbol* temp = symbolInsert(0, makeTemp());
        Symbol* funcSymbol = (Symbol*)node->symbol;
        temp->dataType = funcSymbol->returnType;
        
        // Cria TAC de chamada
        TAC* call = tacCreate(TAC_CALL, temp, node->symbol, NULL);
        return tacJoin(args, call);
    }
    
    // Caso para operações binárias
    if (node->type == AST_OP) {
        TAC* code0 = generateCode(node->son[0]);
        TAC* code1 = generateCode(node->son[1]);
        
        Symbol* temp = symbolInsert(0, makeTemp());
        temp->dataType = getExpressionType(node);
        
        TacType opType;
        char* op = (char*)node->symbol;
        
        if (strcmp(op, "+") == 0) opType = TAC_ADD;
        else if (strcmp(op, "-") == 0) opType = TAC_SUB;
        else if (strcmp(op, "*") == 0) opType = TAC_MUL;
        else if (strcmp(op, "/") == 0) opType = TAC_DIV;
        else if (strcmp(op, "<") == 0 || 
                 strcmp(op, ">") == 0 || 
                 strcmp(op, "<=") == 0 || 
                 strcmp(op, ">=") == 0 || 
                 strcmp(op, "==") == 0 || 
                 strcmp(op, "!=") == 0) {
            // Para operações relacionais, usamos o mesmo TAC_ADD mas com o operador armazenado
            opType = TAC_ADD;
            // Aqui poderíamos criar TACs específicos para cada operação relacional se necessário
        } else {
            opType = TAC_ADD; // Default
        }
        
        TAC* opTac = tacCreate(opType, temp, code0->res, code1->res);
        return tacJoin(tacJoin(code0, code1), opTac);
    }
    
    return NULL;
}

// Gera código para comandos
TAC* generateCodeCmd(AST* node) {
    if (!node) return NULL;
    
    switch (node->type) {
        case AST_IF: {
            TAC* codeExpr = generateCode(node->son[0]);
            TAC* codeCmd = generateCode(node->son[1]);
            
            char* labelEnd = makeLabel();
            Symbol* labelSymbol = symbolInsert(0, labelEnd);
            
            TAC* jumpIfz = tacCreate(TAC_IFZ, labelSymbol, codeExpr->res, NULL);
            TAC* labelTac = tacCreate(TAC_LABEL, labelSymbol, NULL, NULL);
            
            return tacJoin(tacJoin(tacJoin(codeExpr, jumpIfz), codeCmd), labelTac);
        }
        
        case AST_IF_ELSE: {
            TAC* codeExpr = generateCode(node->son[0]);
            TAC* codeThen = generateCode(node->son[1]);
            TAC* codeElse = generateCode(node->son[2]);
            
            char* labelElse = makeLabel();
            char* labelEnd = makeLabel();
            
            Symbol* labelElseSymbol = symbolInsert(0, labelElse);
            Symbol* labelEndSymbol = symbolInsert(0, labelEnd);
            
            TAC* jumpIfz = tacCreate(TAC_IFZ, labelElseSymbol, codeExpr->res, NULL);
            TAC* jumpEnd = tacCreate(TAC_JUMP, labelEndSymbol, NULL, NULL);
            TAC* labelElseTac = tacCreate(TAC_LABEL, labelElseSymbol, NULL, NULL);
            TAC* labelEndTac = tacCreate(TAC_LABEL, labelEndSymbol, NULL, NULL);
            
            return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(
                codeExpr, jumpIfz), codeThen), jumpEnd), labelElseTac), codeElse), labelEndTac);
        }
        
        case AST_WHILE: {
            TAC* codeExpr = generateCode(node->son[0]);
            TAC* codeCmd = generateCode(node->son[1]);
            
            char* labelBegin = makeLabel();
            char* labelEnd = makeLabel();
            
            Symbol* labelBeginSymbol = symbolInsert(0, labelBegin);
            Symbol* labelEndSymbol = symbolInsert(0, labelEnd);
            
            TAC* labelBeginTac = tacCreate(TAC_LABEL, labelBeginSymbol, NULL, NULL);
            TAC* jumpIfz = tacCreate(TAC_IFZ, labelEndSymbol, codeExpr->res, NULL);
            TAC* jumpBegin = tacCreate(TAC_JUMP, labelBeginSymbol, NULL, NULL);
            TAC* labelEndTac = tacCreate(TAC_LABEL, labelEndSymbol, NULL, NULL);
            
            return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(
                labelBeginTac, codeExpr), jumpIfz), codeCmd), jumpBegin), labelEndTac);
        }
        
        case AST_DO_WHILE: {
            TAC* codeCmd = generateCode(node->son[0]);
            TAC* codeExpr = generateCode(node->son[1]);
            
            char* labelBegin = makeLabel();
            
            Symbol* labelBeginSymbol = symbolInsert(0, labelBegin);
            
            TAC* labelBeginTac = tacCreate(TAC_LABEL, labelBeginSymbol, NULL, NULL);
            TAC* jumpIfz = tacCreate(TAC_IFZ, labelBeginSymbol, codeExpr->res, NULL);
            
            return tacJoin(tacJoin(tacJoin(
                labelBeginTac, codeCmd), codeExpr), jumpIfz);
        }
        
        case AST_ASSIGN: {
            // Atribuição simples: a = expr
            if (!node->son[1]) {
                TAC* codeExpr = generateCode(node->son[0]);
                TAC* move = tacCreate(TAC_MOVE, node->symbol, codeExpr->res, NULL);
                return tacJoin(codeExpr, move);
            }
            // Atribuição a vetor: a[expr1] = expr2
            else {
                TAC* codeIndex = generateCode(node->son[0]);
                TAC* codeExpr = generateCode(node->son[1]);
                TAC* vectorAssign = tacCreate(TAC_VECTOR_ASSIGN, node->symbol, codeIndex->res, codeExpr->res);
                return tacJoin(tacJoin(codeIndex, codeExpr), vectorAssign);
            }
        }
        
        case AST_READ: {
            TAC* read = tacCreate(TAC_READ, node->symbol, NULL, NULL);
            return read;
        }
        
        case AST_PRINT: {
            TAC* codePrint = NULL;
            AST* expr = node->son[0];
            
            // Processa cada expressão na lista
            while (expr) {
                TAC* codeExpr = generateCode(expr);
                TAC* print = tacCreate(TAC_PRINT, NULL, codeExpr->res, NULL);
                codePrint = tacJoin(tacJoin(codePrint, codeExpr), print);
                expr = expr->next;
            }
            
            return codePrint;
        }
        
        case AST_RETURN: {
            TAC* codeExpr = generateCode(node->son[0]);
            TAC* ret = tacCreate(TAC_RET, NULL, codeExpr->res, NULL);
            return tacJoin(codeExpr, ret);
        }
        
        case AST_BLOCK: {
            return generateCode(node->son[0]); // Gera código para a lista de comandos
        }
        
        default:
            return generateCode(node->son[0]); // Caso padrão, tenta gerar código para o primeiro filho
    }
}

// Gera código para declarações
TAC* generateCodeDecl(AST* node) {
    if (!node) return NULL;
    
    switch (node->type) {
        case AST_VAR_DECL: {
            // Declaração simples: tipo id;
            if (!node->son[0]) {
                return NULL; // Não gera código para declaração simples
            }
            // Declaração com inicialização: tipo id = expr;
            else if (!node->son[1]) {
                TAC* codeExpr = generateCode(node->son[0]);
                TAC* move = tacCreate(TAC_MOVE, node->symbol, codeExpr->res, NULL);
                return tacJoin(codeExpr, move);
            }
            // Declaração de vetor com inicialização: tipo id[expr] = {expr1, expr2, ...};
            else {
                TAC* codeSize = generateCode(node->son[0]);
                TAC* codeInit = NULL;
                
                // Processa a lista de inicialização
                AST* init = node->son[1];
                int index = 0;
                
                while (init) {
                    TAC* codeExpr = generateCode(init);
                    
                    // Cria um símbolo para o índice
                    char indexStr[16];
                    sprintf(indexStr, "%d", index);
                    Symbol* indexSymbol = symbolInsert(0, strdup(indexStr));
                    
                    TAC* vectorAssign = tacCreate(TAC_VECTOR_ASSIGN, node->symbol, indexSymbol, codeExpr->res);
                    codeInit = tacJoin(tacJoin(codeInit, codeExpr), vectorAssign);
                    
                    init = init->next;
                    index++;
                }
                
                return tacJoin(codeSize, codeInit);
            }
        }
        
        case AST_FUNC_DECL: {
            TAC* codeParams = NULL;
            TAC* codeBlock = generateCode(node->son[1]); // Gera código para o bloco
            
            // Cria TACs para início e fim de função
            TAC* beginFunc = tacCreate(TAC_BEGINFUN, node->symbol, NULL, NULL);
            TAC* endFunc = tacCreate(TAC_ENDFUN, node->symbol, NULL, NULL);
            
            // Junta tudo
            return tacJoin(tacJoin(tacJoin(beginFunc, codeParams), codeBlock), endFunc);
        }
        
        default:
            return NULL;
    }
}

// Função principal para gerar código a partir da AST
TAC* generateCode(void* node) {
    if (!node) return NULL;
    
    AST* ast = (AST*)node;
    
    // Debug: imprimir o tipo do nó que estamos processando (comentado para produção)
    // fprintf(stderr, "Generating code for node type %d\n", ast->type);
    
    // Processar nós irmãos (next) primeiro para garantir que todos os nós sejam processados
    TAC* nextCode = NULL;
    if (ast->next) {
        nextCode = generateCode(ast->next);
    }
    
    TAC* code = NULL;
    
    switch (ast->type) {
        // Expressões
        case AST_SYMBOL:
            // Para símbolos, criamos uma TAC simples
            code = tacCreate(TAC_SYMBOL, ast->symbol, NULL, NULL);
            return tacJoin(code, nextCode);
            
        case AST_OP: {
            // Para operações, geramos código para os operandos e depois para a operação
            TAC* code0 = NULL;
            TAC* code1 = NULL;
            
            if (ast->son[0]) code0 = generateCode(ast->son[0]);
            if (ast->son[1]) code1 = generateCode(ast->son[1]);
            
            // Criar um símbolo temporário para o resultado
            Symbol* temp = symbolInsert(0, makeTemp());
            
            // Determinar o tipo de operação
            TacType opType = TAC_ADD; // Default
            char* op = (char*)ast->symbol;
            
            if (strcmp(op, "+") == 0) opType = TAC_ADD;
            else if (strcmp(op, "-") == 0) opType = TAC_SUB;
            else if (strcmp(op, "*") == 0) opType = TAC_MUL;
            else if (strcmp(op, "/") == 0) opType = TAC_DIV;
            else if (strcmp(op, "<") == 0) opType = TAC_LT;
            else if (strcmp(op, ">") == 0) opType = TAC_GT;
            else if (strcmp(op, "<=") == 0) opType = TAC_LE;
            else if (strcmp(op, ">=") == 0) opType = TAC_GE;
            else if (strcmp(op, "==") == 0) opType = TAC_EQ;
            else if (strcmp(op, "!=") == 0) opType = TAC_NE;
            else if (strcmp(op, "INDEX") == 0) {
                // Caso especial para acesso a vetor
                TAC* vectorIndex = tacCreate(TAC_VECTOR_INDEX, temp, 
                                           code0 ? code0->res : NULL, 
                                           code1 ? code1->res : NULL);
                code = tacJoin(tacJoin(code0, code1), vectorIndex);
                return tacJoin(code, nextCode);
            }
            
            // Criar TAC para a operação
            TAC* opTac = tacCreate(opType, temp, 
                                 code0 ? code0->res : NULL, 
                                 code1 ? code1->res : NULL);
            
            code = tacJoin(tacJoin(code0, code1), opTac);
            return tacJoin(code, nextCode);
        }
            
        case AST_FUNC_CALL: {
            // Para chamadas de função, geramos código para os argumentos
            TAC* args = NULL;
            AST* arg = ast->son[0];
            
            // Processar argumentos
            while (arg) {
                TAC* argCode = generateCode(arg);
                TAC* argTac = tacCreate(TAC_ARG, NULL, argCode ? argCode->res : NULL, NULL);
                args = tacJoin(tacJoin(args, argCode), argTac);
                arg = arg->next;
            }
            
            // Criar temporário para resultado
            char* tempName = makeTemp();
            Symbol* temp = symbolInsert(0, tempName);
            
            // Criar TAC para chamada de função
            TAC* call = tacCreate(TAC_CALL, temp, ast->symbol, NULL);
            code = tacJoin(args, call);
            return tacJoin(code, nextCode);
        }
        
        // Comandos
        case AST_IF: {
            TAC* codeExpr = generateCode(ast->son[0]);
            TAC* codeCmd = generateCode(ast->son[1]);
            
            char* labelEnd = makeLabel();
            Symbol* labelSymbol = symbolInsert(0, labelEnd);
            
            TAC* jumpIfz = tacCreate(TAC_IFZ, labelSymbol, 
                                   codeExpr ? codeExpr->res : NULL, NULL);
            TAC* labelTac = tacCreate(TAC_LABEL, labelSymbol, NULL, NULL);
            
            code = tacJoin(tacJoin(tacJoin(codeExpr, jumpIfz), codeCmd), labelTac);
            return tacJoin(code, nextCode);
        }
        
        case AST_IF_ELSE: {
            TAC* codeExpr = generateCode(ast->son[0]);
            TAC* codeThen = generateCode(ast->son[1]);
            TAC* codeElse = generateCode(ast->son[2]);
            
            char* labelElse = makeLabel();
            char* labelEnd = makeLabel();
            
            Symbol* labelElseSymbol = symbolInsert(0, labelElse);
            Symbol* labelEndSymbol = symbolInsert(0, labelEnd);
            
            TAC* jumpIfz = tacCreate(TAC_IFZ, labelElseSymbol, 
                                   codeExpr ? codeExpr->res : NULL, NULL);
            TAC* jumpEnd = tacCreate(TAC_JUMP, labelEndSymbol, NULL, NULL);
            TAC* labelElseTac = tacCreate(TAC_LABEL, labelElseSymbol, NULL, NULL);
            TAC* labelEndTac = tacCreate(TAC_LABEL, labelEndSymbol, NULL, NULL);
            
            return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(
                codeExpr, jumpIfz), codeThen), jumpEnd), labelElseTac), codeElse), labelEndTac);
        }
        
        case AST_WHILE: {
            TAC* codeExpr = generateCode(ast->son[0]);
            TAC* codeCmd = generateCode(ast->son[1]);
            
            char* labelBegin = makeLabel();
            char* labelEnd = makeLabel();
            
            Symbol* labelBeginSymbol = symbolInsert(0, labelBegin);
            Symbol* labelEndSymbol = symbolInsert(0, labelEnd);
            
            TAC* labelBeginTac = tacCreate(TAC_LABEL, labelBeginSymbol, NULL, NULL);
            TAC* jumpIfz = tacCreate(TAC_IFZ, labelEndSymbol, 
                                   codeExpr ? codeExpr->res : NULL, NULL);
            TAC* jumpBegin = tacCreate(TAC_JUMP, labelBeginSymbol, NULL, NULL);
            TAC* labelEndTac = tacCreate(TAC_LABEL, labelEndSymbol, NULL, NULL);
            
            return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(
                labelBeginTac, codeExpr), jumpIfz), codeCmd), jumpBegin), labelEndTac);
        }
        
        case AST_DO_WHILE: {
            TAC* codeCmd = generateCode(ast->son[0]);
            TAC* codeExpr = generateCode(ast->son[1]);
            
            char* labelBegin = makeLabel();
            char* labelEnd = makeLabel();
            
            Symbol* labelBeginSymbol = symbolInsert(0, labelBegin);
            Symbol* labelEndSymbol = symbolInsert(0, labelEnd);
            
            TAC* labelBeginTac = tacCreate(TAC_LABEL, labelBeginSymbol, NULL, NULL);
            // Se a condição for falsa (zero), sai do loop
            TAC* jumpIfz = tacCreate(TAC_IFZ, labelEndSymbol, 
                                    codeExpr ? codeExpr->res : NULL, NULL);
            // Se a condição for verdadeira, volta ao início
            TAC* jumpBegin = tacCreate(TAC_JUMP, labelBeginSymbol, NULL, NULL);
            TAC* labelEndTac = tacCreate(TAC_LABEL, labelEndSymbol, NULL, NULL);
            
            // Para do-while: primeiro executamos o corpo, depois testamos a condição
            code = tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(
                labelBeginTac, codeCmd), codeExpr), jumpIfz), jumpBegin), labelEndTac);
            return tacJoin(code, nextCode);
        }
        
        case AST_ASSIGN: {
            // Verificar se é atribuição simples ou a vetor
            if (ast->son[1]) {
                // Atribuição a vetor: id[expr1] = expr2
                TAC* codeIndex = generateCode(ast->son[0]);
                TAC* codeExpr = generateCode(ast->son[1]);
                
                TAC* vectorAssign = tacCreate(TAC_VECTOR_ASSIGN, ast->symbol, 
                                           codeIndex ? codeIndex->res : NULL, 
                                           codeExpr ? codeExpr->res : NULL);
                
                code = tacJoin(tacJoin(codeIndex, codeExpr), vectorAssign);
                return tacJoin(code, nextCode);
            } else {
                // Atribuição simples: id = expr
                TAC* codeExpr = generateCode(ast->son[0]);
                
                TAC* move = tacCreate(TAC_MOVE, ast->symbol, 
                                   codeExpr ? codeExpr->res : NULL, NULL);
                
                code = tacJoin(codeExpr, move);
                return tacJoin(code, nextCode);
            }
        }
        
        case AST_READ: {
            code = tacCreate(TAC_READ, ast->symbol, NULL, NULL);
            return tacJoin(code, nextCode);
        }
        
        case AST_PRINT: {
            TAC* codePrint = NULL;
            AST* expr = ast->son[0];
            
            // Processar cada expressão na lista
            while (expr) {
                TAC* codeExpr = generateCode(expr);
                TAC* print = tacCreate(TAC_PRINT, NULL, 
                                     codeExpr ? codeExpr->res : NULL, NULL);
                
                codePrint = tacJoin(tacJoin(codePrint, codeExpr), print);
                expr = expr->next;
            }
            
            return tacJoin(codePrint, nextCode);
        }
        
        case AST_RETURN: {
            TAC* codeExpr = generateCode(ast->son[0]);
            
            TAC* ret = tacCreate(TAC_RET, NULL, 
                                codeExpr ? codeExpr->res : NULL, NULL);
            
            code = tacJoin(codeExpr, ret);
            return tacJoin(code, nextCode);
        }
        
        case AST_BLOCK: {
            // Gerar código para a lista de comandos
            code = generateCode(ast->son[0]);
            return tacJoin(code, nextCode);
        }
        
        // Declarações
        case AST_VAR_DECL: {
            // Declaração com inicialização
            if (ast->son[0]) {
                if (ast->son[1]) {
                    // Declaração de vetor com inicialização: tipo id[expr] = {expr1, expr2, ...};
                    TAC* codeSize = generateCode(ast->son[0]);
                    TAC* codeInit = NULL;
                    
                    // Processar lista de inicialização
                    AST* init = ast->son[1];
                    int index = 0;
                    
                    while (init) {
                        TAC* codeExpr = generateCode(init);
                        
                        // Criar símbolo para o índice
                        char indexStr[16];
                        sprintf(indexStr, "%d", index);
                        Symbol* indexSymbol = symbolInsert(0, strdup(indexStr));
                        
                        TAC* vectorAssign = tacCreate(TAC_VECTOR_ASSIGN, ast->symbol, 
                                                   indexSymbol, 
                                                   codeExpr ? codeExpr->res : NULL);
                        
                        codeInit = tacJoin(tacJoin(codeInit, codeExpr), vectorAssign);
                        
                        init = init->next;
                        index++;
                    }
                    
                    code = tacJoin(codeSize, codeInit);
                    return tacJoin(code, nextCode);
                } else {
                    // Declaração com inicialização: tipo id = expr;
                    TAC* codeExpr = generateCode(ast->son[0]);
                    
                    TAC* move = tacCreate(TAC_MOVE, ast->symbol, 
                                       codeExpr ? codeExpr->res : NULL, NULL);
                    
                    code = tacJoin(codeExpr, move);
                    return tacJoin(code, nextCode);
                }
            }
            // Declaração simples: tipo id;
            return nextCode;
        }
        
        case AST_FUNC_DECL: {
            // Gerar código para o bloco da função
            TAC* codeBlock = generateCode(ast->son[1]);
            
            // Criar TACs para início e fim de função
            TAC* beginFunc = tacCreate(TAC_BEGINFUN, ast->symbol, NULL, NULL);
            TAC* endFunc = tacCreate(TAC_ENDFUN, ast->symbol, NULL, NULL);
            
            code = tacJoin(tacJoin(beginFunc, codeBlock), endFunc);
            return tacJoin(code, nextCode);
        }
        
        // Listas
        case AST_PROGRAMA:
        case AST_DECL_LIST:
        case AST_CMD_LIST:
        case AST_EXPR_LIST:
        case AST_PARAM_LIST: {
            TAC* code = NULL;
            
            // Processar filhos
            for (int i = 0; i < 4; i++) {
                if (ast->son[i]) {
                    TAC* sonCode = generateCode(ast->son[i]);
                    code = tacJoin(code, sonCode);
                }
            }
            
            return tacJoin(code, nextCode);
        }
        
        default:
            fprintf(stderr, "Warning: Unhandled AST node type %d in generateCode\n", ast->type);
            return nextCode;
    }
}
