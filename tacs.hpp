//
// tacs.hpp - Definição das estruturas de TACs (Three-Address Codes)
//

#ifndef TACS_HPP
#define TACS_HPP

#include <string>
#include <vector>

// Tipos de operações para TACs
typedef enum {
    TAC_SYMBOL,     // Símbolo (não gera código)
    TAC_MOVE,       // Atribuição: a = b
    TAC_ADD,        // Adição: a = b + c
    TAC_SUB,        // Subtração: a = b - c
    TAC_MUL,        // Multiplicação: a = b * c
    TAC_DIV,        // Divisão: a = b / c
    TAC_LT,         // Menor que: a = b < c
    TAC_GT,         // Maior que: a = b > c
    TAC_LE,         // Menor ou igual: a = b <= c
    TAC_GE,         // Maior ou igual: a = b >= c
    TAC_EQ,         // Igual: a = b == c
    TAC_NE,         // Diferente: a = b != c
    TAC_LABEL,      // Rótulo para saltos
    TAC_BEGINFUN,   // Início de função
    TAC_ENDFUN,     // Fim de função
    TAC_IFZ,        // If zero: if a == 0 goto label
    TAC_JUMP,       // Salto incondicional: goto label
    TAC_CALL,       // Chamada de função: a = call b
    TAC_ARG,        // Argumento para função: arg a
    TAC_RET,        // Retorno de função: return a
    TAC_PRINT,      // Impressão: print a
    TAC_READ,       // Leitura: read a
    TAC_VECTOR_INDEX, // Acesso a vetor: a = b[c]
    TAC_VECTOR_ASSIGN // Atribuição a vetor: a[b] = c
} TacType;

// Estrutura para representar uma TAC
typedef struct tac_node {
    TacType type;
    void* res;
    void* op1;
    void* op2;
    struct tac_node* prev;
    struct tac_node* next;
} TAC;

// Funções para criar e manipular TACs
TAC* tacCreate(TacType type, void* res, void* op1, void* op2);
TAC* tacJoin(TAC* l1, TAC* l2);
void tacPrint(TAC* tac);
void tacPrintBackwards(TAC* tac);
void tacPrintForward(TAC* tac);

// Funções para criar símbolos temporários e labels
char* makeTemp();
char* makeLabel();

// Função principal para gerar código a partir da AST
TAC* generateCode(void* node);

#endif // TACS_HPP
