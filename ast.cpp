#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.tab.hpp"
#include "symbols.hpp"
#include <cstring>
#include <set>

AST* astCreate(AstNodeType type, const void* symbol, AST* son0, AST* son1, AST* son2, AST* son3) {
    AST* node = (AST*)malloc(sizeof(AST));
    if (!node) {
        fprintf(stderr, "Erro de memória ao criar nó da AST\n");
        exit(1);
    }
    node->type = type;
    node->symbol = (void*)symbol;
    node->son[0] = son0;
    node->son[1] = son1;
    node->son[2] = son2;
    node->son[3] = son3;
    node->next = NULL;
    node->line_number = -1;
    return node;
}

const char* astTypeName(AstNodeType type) {
    switch(type) {
        case AST_PROGRAMA: return "PROGRAMA";
        case AST_DECL_LIST: return "DECL_LIST";
        case AST_VAR_DECL: return "VAR_DECL";
        case AST_FUNC_DECL: return "FUNC_DECL";
        case AST_PARAM_LIST: return "PARAM_LIST";
        case AST_CMD_LIST: return "CMD_LIST";
        case AST_CMD: return "CMD";
        case AST_BLOCK: return "BLOCK";
        case AST_ASSIGN: return "ASSIGN";
        case AST_READ: return "READ";
        case AST_PRINT: return "PRINT";
        case AST_RETURN: return "RETURN";
        case AST_IF: return "IF";
        case AST_IF_ELSE: return "IF_ELSE";
        case AST_WHILE: return "WHILE";
        case AST_DO_WHILE: return "DO_WHILE";
        case AST_FUNC_CALL: return "FUNC_CALL";
        case AST_EXPR_LIST: return "EXPR_LIST";
        case AST_OP: return "OP";
        case AST_SYMBOL: return "SYMBOL";
        default: return "UNKNOWN";
    }
}

// Função auxiliar para imprimir símbolo (identificador/literal)
void astPrintSymbol(void* symbol, AstNodeType type) {
    if (!symbol) return;
    if (type == AST_SYMBOL) {
        printf("%s", ((Symbol*)symbol)->text.c_str());
    } else if (type == AST_OP) {
        printf("%s", (const char*)symbol);
    } else {
        printf("?");
    }
}

void astPrint(AST* node, int level) {
    if (!node) return;
    for (int i = 0; i < level; ++i) printf("  ");
    printf("[%s", astTypeName(node->type));
    if (node->symbol) {
        printf(", symbol=");
        astPrintSymbol(node->symbol, node->type);
    }
    printf("]\n");
    for (int i = 0; i < 4; ++i) {
        if (node->son[i]) {
            astPrint(node->son[i], level + 1);
        }
    }
    if (node->next) {
        astPrint(node->next, level);
    }
}

// Função auxiliar para imprimir indentação
void printIndent(FILE* out, int level) {
    for (int i = 0; i < level; ++i) {
        fprintf(out, "    ");
    }
}

// Variável global para controlar o nível de indentação
int indent_level = 0;

// Declaração antecipada para resolver dependência circular
void astDecompile(AST* node, FILE* out);

// Nova função simplificada para descompilação da AST
void astDecompileSimple(AST* node, FILE* out);

// Função auxiliar para decompilação de expressões
void astDecompileExpr(AST* node, FILE* out) {
    if (!node) return;
    
    switch (node->type) {
        case AST_OP:
        {
            // Acesso a vetor: v[i]
            if (node->symbol && strcmp((const char*)node->symbol, "INDEX") == 0) {
                if (node->son[0]) astDecompileExpr(node->son[0], out);
                fprintf(out, "[");
                if (node->son[1]) astDecompileExpr(node->son[1], out);
                fprintf(out, "]");
            } else {
                // Operação binária normal
                fprintf(out, "(");
                if (node->son[0]) astDecompileExpr(node->son[0], out);
                if (node->symbol) fprintf(out, " %s ", (const char*)node->symbol);
                if (node->son[1]) astDecompileExpr(node->son[1], out);
                fprintf(out, ")");
            }
            break;
        }
            
        case AST_FUNC_CALL:
        {
            if (node->symbol) fprintf(out, "%s", ((Symbol*)node->symbol)->text.c_str());
            fprintf(out, "(");
            // Processar lista de argumentos
            AST* arg = node->son[0];
            bool first = true;
            while (arg) {
                if (!first) fprintf(out, ", ");
                astDecompileExpr(arg, out);
                first = false;
                arg = arg->next;
            }
            fprintf(out, ")");
            break;
        }
            
        case AST_SYMBOL:
        {
            if (node->symbol) fprintf(out, "%s", ((Symbol*)node->symbol)->text.c_str());
            break;
        }
            
        default:
            break;
    }
}

// Função principal de decompilação
void astDecompile(AST* node, FILE* out) {
    if (!node) return;
    
    // Imprimir indentação para comandos (exceto blocos)
    if (node->type != AST_BLOCK && node->type != AST_PROGRAMA && 
        node->type != AST_DECL_LIST && node->type != AST_VAR_DECL && 
        node->type != AST_FUNC_DECL && node->type != AST_PARAM_LIST) {
        printIndent(out, indent_level);
    }
    
    // Não usamos mais o mecanismo de rastreamento de nós
    // pois ele impede o processamento adequado das declarações encadeadas
    
    switch (node->type) {
        case AST_PROGRAMA:
            // Imprimir cabeçalho do arquivo decompilado
            fprintf(out, "=== DECOMPILADO ===\n\n");
            
            // Processar todas as declarações globais
            if (node->son[0]) {
                // Percorrer a lista de declarações
                for (AST* decl = node->son[0]; decl != NULL; decl = decl->next) {
                    astDecompile(decl, out);
                }
            }
            
            // Imprimir rodapé do arquivo decompilado
            fprintf(out, "===================\n");
            break;
            
        case AST_DECL_LIST:
            // Processar a declaração atual
            if (node->son[0]) {
                astDecompile(node->son[0], out);
            }
            
            // Não processamos o next aqui, pois já é feito no AST_PROGRAMA
            break;
            
        // Lista de expressões
        case AST_EXPR_LIST:
            // Processar todas as expressões na lista
            for (AST* expr = node; expr != NULL; expr = expr->next) {
                if (expr != node) fprintf(out, ", ");
                astDecompileExpr(expr, out);
            }
            break;
        case AST_VAR_DECL:
        {
            // Indentação só para variáveis locais
            bool is_local = (indent_level > 0);
            if (is_local) {
                printIndent(out, indent_level);
            }
            
            // Imprime o tipo
            if (node->type_symbol) {
                Symbol* sym = (Symbol*)node->type_symbol;
                switch (sym->token) {
                    case KW_BYTE:   fprintf(out, "byte "); break;
                    case KW_INT:    fprintf(out, "int "); break;
                    case KW_REAL:   fprintf(out, "real "); break;
                    case KW_STRING: fprintf(out, "string "); break;
                    case KW_CHAR:   fprintf(out, "char "); break;
                    default:        fprintf(out, "? "); break;
                }
            }
            
            // Imprime o nome da variável
            if (node->symbol) {
                fprintf(out, "%s", ((Symbol*)node->symbol)->text.c_str());
            }
            
            // Determinar o tipo de declaração baseado na estrutura da AST
            if (node->son[0]) {
                // Verificar se é um vetor ou uma variável simples com inicialização
                if (node->son[1]) {
                    // Vetor com inicialização: int v[10] = 0, 1, 2, ...
                    fprintf(out, "[");
                    astDecompileExpr(node->son[0], out);
                    fprintf(out, "] = ");
                    
                    // Imprimir lista de valores iniciais
                    astDecompileExpr(node->son[1], out);
                    
                    // Os demais literais serão processados pela chamada recursiva
                    // que trata os irmãos (next) em AST_EXPR_LIST
                } else {
                    // Variável simples com inicialização: int a = 0;
                    fprintf(out, " = ");
                    astDecompileExpr(node->son[0], out);
                }
            }
            
            fprintf(out, ";\n");
            break;
        }
        case AST_FUNC_DECL: {
            // Tipo de retorno
            if (node->type_symbol) {
                Symbol* sym = (Symbol*)node->type_symbol;
                switch (sym->token) {
                    case KW_BYTE:   fprintf(out, "byte "); break;
                    case KW_INT:    fprintf(out, "int "); break;
                    case KW_REAL:   fprintf(out, "real "); break;
                    case KW_STRING: fprintf(out, "string "); break;
                    case KW_CHAR:   fprintf(out, "char "); break;
                    default:        fprintf(out, "? "); break;
                }
            }
            
            // Nome da função
            if (node->symbol) {
                fprintf(out, "%s", ((Symbol*)node->symbol)->text.c_str());
            }
            
            // Parâmetros
            fprintf(out, "(");
            AST* param = node->son[0];
            bool first = true;
            while (param) {
                if (!first) fprintf(out, ", ");
                
                // Tipo do parâmetro
                if (param->type_symbol) {
                    Symbol* sym = (Symbol*)param->type_symbol;
                    switch (sym->token) {
                        case KW_BYTE:   fprintf(out, "byte "); break;
                        case KW_INT:    fprintf(out, "int "); break;
                        case KW_REAL:   fprintf(out, "real "); break;
                        case KW_STRING: fprintf(out, "string "); break;
                        case KW_CHAR:   fprintf(out, "char "); break;
                        default:        fprintf(out, "? "); break;
                    }
                }
                
                // Nome do parâmetro
                if (param->symbol) {
                    fprintf(out, "%s", ((Symbol*)param->symbol)->text.c_str());
                }
                
                first = false;
                param = param->next;
            }
            fprintf(out, ") ");
            
            // Bloco de código
            if (node->son[1]) {
                fprintf(out, "{\n");
                indent_level++;
                
                // Processar comandos dentro do bloco
                if (node->son[1]->son[0]) {
                    if (node->son[1]->son[0]) {
                        printIndent(out, indent_level);
                        astDecompile(node->son[1]->son[0], out);
                    }
                }
                
                indent_level--;
                printIndent(out, indent_level);
                fprintf(out, "}\n");
            } else {
                fprintf(out, ";\n");
            }
            
            break;
        }
        case AST_BLOCK: {
            fprintf(out, "{\n");
            indent_level++;
            
            // Processar todos os comandos dentro do bloco
            if (node->son[0]) {
                // Percorrer a lista de comandos
                for (AST* cmd = node->son[0]; cmd != NULL; cmd = cmd->next) {
                    printIndent(out, indent_level);
                    astDecompile(cmd, out);
                }
            }
            
            indent_level--;
            printIndent(out, indent_level);
            fprintf(out, "}\n");
            break;
        }
        case AST_ASSIGN:
            if (node->symbol) {
                // Nome da variável
                fprintf(out, "%s", ((Symbol*)node->symbol)->text.c_str());
                
                if (node->son[0] && node->son[1]) {
                    // Atribuição a vetor: a[i] = x
                    fprintf(out, "[");
                    astDecompileExpr(node->son[0], out);
                    fprintf(out, "] = ");
                    astDecompileExpr(node->son[1], out);
                } else {
                    // Atribuição simples: a = x
                    fprintf(out, " = ");
                    astDecompileExpr(node->son[0], out);
                }
                fprintf(out, ";\n");
            }
            
            // Processar o próximo comando, se houver
            if (node->next) {
                printIndent(out, indent_level);
                astDecompile(node->next, out);
            }
            break;
        case AST_PRINT:
            fprintf(out, "print ");
            if (node->son[0]) astDecompileExpr(node->son[0], out);
            fprintf(out, ";\n");
            
            // Processar o próximo comando, se houver
            if (node->next) {
                printIndent(out, indent_level);
                astDecompile(node->next, out);
            }
            break;
        case AST_READ:
            fprintf(out, "read ");
            if (node->symbol) fprintf(out, "%s", ((Symbol*)node->symbol)->text.c_str());
            fprintf(out, ";\n");
            
            // Processar o próximo comando, se houver
            if (node->next) {
                printIndent(out, indent_level);
                astDecompile(node->next, out);
            }
            break;
        case AST_RETURN:
            fprintf(out, "return ");
            if (node->son[0]) astDecompileExpr(node->son[0], out);
            fprintf(out, ";\n");
            
            // Processar o próximo comando, se houver
            if (node->next) {
                printIndent(out, indent_level);
                astDecompile(node->next, out);
            }
            break;
        case AST_IF:
            fprintf(out, "if (");
            if (node->son[0]) astDecompileExpr(node->son[0], out);
            fprintf(out, ") ");
            if (node->son[1]) astDecompile(node->son[1], out);
            
            // Processar o próximo comando, se houver
            if (node->next) {
                printIndent(out, indent_level);
                astDecompile(node->next, out);
            }
            break;
        case AST_IF_ELSE:
            fprintf(out, "if (");
            if (node->son[0]) astDecompileExpr(node->son[0], out);
            fprintf(out, ") ");
            if (node->son[1]) astDecompile(node->son[1], out);
            
            printIndent(out, indent_level);
            fprintf(out, "else ");
            if (node->son[2]) astDecompile(node->son[2], out);
            
            // Processar o próximo comando, se houver
            if (node->next) {
                printIndent(out, indent_level);
                astDecompile(node->next, out);
            }
            break;
        case AST_WHILE:
            fprintf(out, "while (");
            if (node->son[0]) astDecompileExpr(node->son[0], out);
            fprintf(out, ") do ");
            if (node->son[1]) astDecompile(node->son[1], out);
            
            // Processar o próximo comando, se houver
            if (node->next) {
                printIndent(out, indent_level);
                astDecompile(node->next, out);
            }
            break;
        case AST_DO_WHILE:
            fprintf(out, "do ");
            if (node->son[0]) astDecompile(node->son[0], out);
            printIndent(out, indent_level);
            fprintf(out, "while (");
            if (node->son[1]) astDecompileExpr(node->son[1], out);
            fprintf(out, ");\n");
            
            // Processar o próximo comando, se houver
            if (node->next) {
                printIndent(out, indent_level);
                astDecompile(node->next, out);
            }
            break;
        case AST_FUNC_CALL: {
            if (node->symbol) fprintf(out, "%s", ((Symbol*)node->symbol)->text.c_str());
            fprintf(out, "(");
            // Processar lista de argumentos
            AST* arg = node->son[0];
            bool first = true;
            while (arg) {
                if (!first) fprintf(out, ", ");
                astDecompileExpr(arg, out);
                first = false;
                arg = arg->next;
            }
            fprintf(out, ")");
            
            // Processar o próximo comando, se houver
            if (node->next) {
                printIndent(out, indent_level);
                astDecompile(node->next, out);
            }
            break;
        }

        case AST_OP:
            if (node->symbol && strcmp((const char*)node->symbol, "INDEX") == 0) {
                // Acesso a vetor: v[i]
                if (node->son[0]) astDecompileExpr(node->son[0], out);
                fprintf(out, "[");
                if (node->son[1]) astDecompileExpr(node->son[1], out);
                fprintf(out, "]");
            } else {
                // Operação binária normal
                fprintf(out, "(");
                if (node->son[0]) astDecompileExpr(node->son[0], out);
                if (node->symbol) fprintf(out, " %s ", (const char*)node->symbol);
                if (node->son[1]) astDecompileExpr(node->son[1], out);
                fprintf(out, ")");
            }
            break;
        case AST_SYMBOL:
            if (node->symbol) fprintf(out, "%s", ((Symbol*)node->symbol)->text.c_str());
            break;
        default:
            break;
    }
    
            // IMPORTANTE: Não processamos automaticamente nós irmãos (next) aqui
    // para evitar duplicações na decompilação
}

// Função simplificada para descompilação da AST
void astDecompileSimple(AST* node, FILE* out) {
    if (!node) return;
    
    fprintf(out, "=== DECOMPILADO ===\n\n");
    
    // Se o nó raiz é do tipo AST_PROGRAMA, processar seus filhos
    if (node->type == AST_PROGRAMA && node->son[0]) {
        node = node->son[0]; // Primeira declaração
    }
    
    // Percorrer todas as declarações globais
    for (AST* decl = node; decl != NULL; decl = decl->next) {
        // Processar declaração de variável global
        if (decl->type == AST_VAR_DECL) {
            // Tipo da variável
            if (decl->type_symbol) {
                Symbol* sym = (Symbol*)decl->type_symbol;
                switch (sym->token) {
                    case KW_BYTE:   fprintf(out, "byte "); break;
                    case KW_INT:    fprintf(out, "int "); break;
                    case KW_REAL:   fprintf(out, "real "); break;
                    case KW_STRING: fprintf(out, "string "); break;
                    case KW_CHAR:   fprintf(out, "char "); break;
                    default:        fprintf(out, "? "); break;
                }
            }
            
            // Nome da variável
            if (decl->symbol) {
                fprintf(out, "%s", ((Symbol*)decl->symbol)->text.c_str());
            }
            
            // Determinar o tipo de declaração baseado na estrutura da AST
            if (decl->son[0]) {
                // Verificar se é um vetor ou uma variável simples com inicialização
                if (decl->son[1]) {
                    // Vetor com inicialização: int v[10] = 0, 1, 2, ...
                    fprintf(out, "[");
                    astDecompileExpr(decl->son[0], out);
                    fprintf(out, "] = ");
                    
                    // Imprimir lista de valores iniciais
                    AST* lit = decl->son[1];
                    bool first = true;
                    while (lit) {
                        if (!first) fprintf(out, ", ");
                        astDecompileExpr(lit, out);
                        first = false;
                        lit = lit->next;
                    }
                } else {
                    // Verificar se é um vetor sem inicialização
                    Symbol* sym = (Symbol*)decl->symbol;
                    if (sym && sym->nature == SYMBOL_VECTOR) {
                        fprintf(out, "[");
                        astDecompileExpr(decl->son[0], out);
                        fprintf(out, "]");
                    } else {
                        // Variável simples com inicialização: int a = 0;
                        fprintf(out, " = ");
                        astDecompileExpr(decl->son[0], out);
                    }
                }
            }
            
            fprintf(out, ";\n");
        }
        // Processar declaração de função
        else if (decl->type == AST_FUNC_DECL) {
            // Tipo de retorno
            if (decl->type_symbol) {
                Symbol* sym = (Symbol*)decl->type_symbol;
                switch (sym->token) {
                    case KW_BYTE:   fprintf(out, "byte "); break;
                    case KW_INT:    fprintf(out, "int "); break;
                    case KW_REAL:   fprintf(out, "real "); break;
                    case KW_STRING: fprintf(out, "string "); break;
                    case KW_CHAR:   fprintf(out, "char "); break;
                    default:        fprintf(out, "? "); break;
                }
            }
            
            // Nome da função
            if (decl->symbol) {
                fprintf(out, "%s", ((Symbol*)decl->symbol)->text.c_str());
            }
            
            // Parâmetros
            fprintf(out, "(");
            if (decl->son[0]) {
                // Percorrer lista de parâmetros
                AST* param = decl->son[0];
                bool first = true;
                while (param) {
                    if (!first) fprintf(out, ", ");
                    
                    // Tipo do parâmetro
                    if (param->type_symbol) {
                        Symbol* sym = (Symbol*)param->type_symbol;
                        switch (sym->token) {
                            case KW_BYTE:   fprintf(out, "byte "); break;
                            case KW_INT:    fprintf(out, "int "); break;
                            case KW_REAL:   fprintf(out, "real "); break;
                            case KW_STRING: fprintf(out, "string "); break;
                            case KW_CHAR:   fprintf(out, "char "); break;
                            default:        fprintf(out, "? "); break;
                        }
                    }
                    
                    // Nome do parâmetro
                    if (param->symbol) {
                        fprintf(out, "%s", ((Symbol*)param->symbol)->text.c_str());
                    }
                    
                    first = false;
                    param = param->next;
                }
            }
            fprintf(out, ") ");
            
            // Corpo da função
            if (decl->son[1]) {
                fprintf(out, "{\n");
                
                // Percorrer comandos dentro do bloco
                if (decl->son[1]->son[0]) {
                    decompileCommands(decl->son[1]->son[0], out, 2); // 2 níveis de indentação
                }
                
                fprintf(out, "}\n");
            }
        }
    }
    
    fprintf(out, "===================\n");
}

// Função auxiliar para descompilação de comandos com indentação
void decompileCommands(AST* cmd, FILE* out, int indent) {
    if (!cmd) return;
    
    // Imprimir indentação
    for (int i = 0; i < indent; i++) {
        fprintf(out, "  "); // 2 espaços por nível de indentação
    }
    
    // Processar comando
    switch (cmd->type) {
        case AST_RETURN:
            fprintf(out, "return ");
            if (cmd->son[0]) {
                astDecompileExpr(cmd->son[0], out);
            }
            fprintf(out, ";\n");
            break;
            
        case AST_ASSIGN:
            if (cmd->symbol) {
                fprintf(out, "%s", ((Symbol*)cmd->symbol)->text.c_str());
                
                // Verificar se é atribuição a vetor
                if (cmd->son[0] && cmd->son[1]) {
                    fprintf(out, "[");
                    astDecompileExpr(cmd->son[0], out);
                    fprintf(out, "] = ");
                    astDecompileExpr(cmd->son[1], out);
                } else {
                    fprintf(out, " = ");
                    astDecompileExpr(cmd->son[0], out);
                }
                fprintf(out, ";\n");
            }
            break;
            
        case AST_PRINT:
            fprintf(out, "print ");
            if (cmd->son[0]) {
                // Processar lista de expressões
                AST* expr = cmd->son[0];
                bool first = true;
                while (expr) {
                    if (!first) fprintf(out, " ");
                    astDecompileExpr(expr, out);
                    first = false;
                    expr = expr->next;
                }
            }
            fprintf(out, ";\n");
            break;
            
        case AST_READ:
            fprintf(out, "read ");
            if (cmd->symbol) {
                fprintf(out, "%s", ((Symbol*)cmd->symbol)->text.c_str());
            }
            fprintf(out, ";\n");
            break;
            
        case AST_IF:
            fprintf(out, "if (");
            if (cmd->son[0]) astDecompileExpr(cmd->son[0], out);
            fprintf(out, ") \n");
            
            // Processar bloco do if
            if (cmd->son[1]) {
                if (cmd->son[1]->type == AST_BLOCK) {
                    // Imprimir indentação
                    for (int i = 0; i < indent; i++) {
                        fprintf(out, "  ");
                    }
                    fprintf(out, "{\n");
                    
                    // Processar comandos dentro do bloco
                    if (cmd->son[1]->son[0]) {
                        decompileCommands(cmd->son[1]->son[0], out, indent + 1);
                    }
                    
                    // Imprimir indentação
                    for (int i = 0; i < indent; i++) {
                        fprintf(out, "  ");
                    }
                    fprintf(out, "}\n");
                } else {
                    decompileCommands(cmd->son[1], out, indent + 1);
                }
            }
            break;
            
        case AST_IF_ELSE:
            fprintf(out, "if (");
            if (cmd->son[0]) astDecompileExpr(cmd->son[0], out);
            fprintf(out, ") \n");
            
            // Processar bloco do if
            if (cmd->son[1]) {
                if (cmd->son[1]->type == AST_BLOCK) {
                    // Imprimir indentação
                    for (int i = 0; i < indent; i++) {
                        fprintf(out, "  ");
                    }
                    fprintf(out, "{\n");
                    
                    // Processar comandos dentro do bloco
                    if (cmd->son[1]->son[0]) {
                        decompileCommands(cmd->son[1]->son[0], out, indent + 1);
                    }
                    
                    // Imprimir indentação
                    for (int i = 0; i < indent; i++) {
                        fprintf(out, "  ");
                    }
                    fprintf(out, "}\n");
                } else {
                    decompileCommands(cmd->son[1], out, indent + 1);
                }
            }
            
            // Imprimir indentação
            for (int i = 0; i < indent; i++) {
                fprintf(out, "  ");
            }
            fprintf(out, "else \n");
            
            // Processar bloco do else
            if (cmd->son[2]) {
                if (cmd->son[2]->type == AST_BLOCK) {
                    // Imprimir indentação
                    for (int i = 0; i < indent; i++) {
                        fprintf(out, "  ");
                    }
                    fprintf(out, "{\n");
                    
                    // Processar comandos dentro do bloco
                    if (cmd->son[2]->son[0]) {
                        decompileCommands(cmd->son[2]->son[0], out, indent + 1);
                    }
                    
                    // Imprimir indentação
                    for (int i = 0; i < indent; i++) {
                        fprintf(out, "  ");
                    }
                    fprintf(out, "}\n");
                } else {
                    decompileCommands(cmd->son[2], out, indent + 1);
                }
            }
            break;
            
        case AST_WHILE:
            fprintf(out, "while (");
            if (cmd->son[0]) astDecompileExpr(cmd->son[0], out);
            fprintf(out, ") do\n");
            
            // Processar bloco do while
            if (cmd->son[1]) {
                if (cmd->son[1]->type == AST_BLOCK) {
                    // Imprimir indentação
                    for (int i = 0; i < indent; i++) {
                        fprintf(out, "  ");
                    }
                    fprintf(out, "{\n");
                    
                    // Processar comandos dentro do bloco
                    if (cmd->son[1]->son[0]) {
                        decompileCommands(cmd->son[1]->son[0], out, indent + 1);
                    }
                    
                    // Imprimir indentação
                    for (int i = 0; i < indent; i++) {
                        fprintf(out, "  ");
                    }
                    fprintf(out, "}\n");
                } else {
                    decompileCommands(cmd->son[1], out, indent + 1);
                }
            }
            break;
            
        case AST_DO_WHILE:
            fprintf(out, "do\n");
            
            // Processar bloco do do-while
            if (cmd->son[0]) {
                if (cmd->son[0]->type == AST_BLOCK) {
                    // Imprimir indentação
                    for (int i = 0; i < indent; i++) {
                        fprintf(out, "  ");
                    }
                    fprintf(out, "{\n");
                    
                    // Processar comandos dentro do bloco
                    if (cmd->son[0]->son[0]) {
                        decompileCommands(cmd->son[0]->son[0], out, indent + 1);
                    }
                    
                    // Imprimir indentação
                    for (int i = 0; i < indent; i++) {
                        fprintf(out, "  ");
                    }
                    fprintf(out, "}\n");
                } else {
                    decompileCommands(cmd->son[0], out, indent + 1);
                }
            }
            
            // Imprimir indentação
            for (int i = 0; i < indent; i++) {
                fprintf(out, "  ");
            }
            fprintf(out, "while (");
            if (cmd->son[1]) astDecompileExpr(cmd->son[1], out);
            fprintf(out, ");\n");
            break;
            
        case AST_BLOCK:
            fprintf(out, "{\n");
            
            // Processar comandos dentro do bloco
            if (cmd->son[0]) {
                decompileCommands(cmd->son[0], out, indent + 1);
            }
            
            // Imprimir indentação
            for (int i = 0; i < indent; i++) {
                fprintf(out, "  ");
            }
            fprintf(out, "}\n");
            break;
            
        case AST_FUNC_CALL:
            if (cmd->symbol) {
                fprintf(out, "%s", ((Symbol*)cmd->symbol)->text.c_str());
            }
            fprintf(out, "(");
            
            // Processar argumentos
            if (cmd->son[0]) {
                AST* arg = cmd->son[0];
                bool first = true;
                while (arg) {
                    if (!first) fprintf(out, ", ");
                    astDecompileExpr(arg, out);
                    first = false;
                    arg = arg->next;
                }
            }
            fprintf(out, ");\n");
            break;
    }
    
    // Processar próximo comando
    if (cmd->next) {
        decompileCommands(cmd->next, out, indent);
    }
}


void astFree(AST* node) {
    if (!node) return;
    for (int i = 0; i < 4; ++i) {
        astFree(node->son[i]);
    }
    astFree(node->next);
    free(node);
}
