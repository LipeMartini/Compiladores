#ifndef AST_H
#define AST_H

#include <stdio.h>

// Enum para tipos de nó da AST
typedef enum {
    AST_PROGRAMA,
    AST_DECL_LIST,
    AST_VAR_DECL,
    AST_FUNC_DECL,
    AST_PARAM_LIST,
    AST_CMD_LIST,
    AST_CMD,
    AST_BLOCK,
    AST_ASSIGN,
    AST_READ,
    AST_PRINT,
    AST_RETURN,
    AST_IF,
    AST_IF_ELSE,
    AST_WHILE,
    AST_DO_WHILE,
    AST_FUNC_CALL,
    AST_EXPR_LIST,
    AST_OP,        // Operação aritmética/relacional/lógica
    AST_SYMBOL     // Identificador ou literal
    // ... adicione outros tipos conforme necessário
} AstNodeType;

// Estrutura do nó da AST
typedef struct ast_node {
    AstNodeType type;
    void* symbol;
    void* type_symbol; // símbolo do tipo (ex: KW_INT, KW_BYTE, etc) // ponteiro para tabela de símbolos, se aplicável
    struct ast_node* son[4]; // até 4 filhos
    struct ast_node* next;   // para listas encadeadas à direita
    int line_number;         // linha do código fonte
} AST;

// Funções utilitárias
AST* astCreate(AstNodeType type, const void* symbol, AST* son0, AST* son1, AST* son2, AST* son3);
void astPrint(AST* node, int level);
void astDecompile(AST* node, FILE* out);
void astDecompileSimple(AST* node, FILE* out);
void decompileCommands(AST* cmd, FILE* out, int indent);
void astFree(AST* node);

#endif // AST_H
