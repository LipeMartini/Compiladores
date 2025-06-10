#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "symbols.hpp"
#include "ast.h"
#include "tacs.hpp"

FILE* ast_output_file = nullptr;
extern AST* ast_root;

extern FILE *yyin;
extern int yyparse(void);
extern void yyrestart(FILE*);

// Função para realizar a verificação semântica da AST
void semanticAnalysis(AST* node) {
    if (!node) return;
    
    // Verificar o tipo de nó e realizar a verificação semântica apropriada
    switch (node->type) {
        case AST_VAR_DECL:
        case AST_FUNC_DECL:
            checkDeclaration(node);
            break;
            
        case AST_ASSIGN:
            checkAssignment(node);
            break;
            
        case AST_FUNC_CALL:
            checkFunctionCall(node);
            break;
            
        case AST_OP:
            if (node->symbol && strcmp((const char*)node->symbol, "INDEX") == 0) {
                checkVectorIndex(node);
            } else {
                // Para outros operadores, obter o tipo da expressão já faz as verificações
                getExpressionType(node);
            }
            break;
            
        case AST_IF:
        case AST_IF_ELSE:
        case AST_WHILE:
        case AST_DO_WHILE:
            // Verificar condições em estruturas de controle
            checkControlStructure(node);
            break;
            
        case AST_RETURN:
            // Verificar tipos de retorno em funções
            checkReturn(node);
            break;
            
        default:
            // Para outros tipos de nós, não há verificação específica
            break;
    }
    
    // Processar filhos recursivamente
    for (int i = 0; i < 4; i++) {
        if (node->son[i]) {
            semanticAnalysis(node->son[i]);
        }
    }
    
    // Processar irmãos
    if (node->next) {
        semanticAnalysis(node->next);
    }
}

int main(int argc, char **argv) {
    // Códigos de saída conforme especificação:
    // 0: sucesso, sem erros sintáticos ou semânticos
    // 1: arquivo não informado
    // 2: arquivo inexistente
    // 3: erro de sintaxe
    // 4: existência de um ou mais erros semânticos
    
    if (argc < 2) {
        fprintf(stderr, "Call: ./etapa4 input_file [output_file]\n");
        exit(1);  // Código 1: arquivo não informado
    }

    FILE *yyin = fopen(argv[1], "r");
    if (!yyin) {
        fprintf(stderr, "Cannot open input file %s\n", argv[1]);
        exit(2);  // Código 2: arquivo inexistente
    }
    
    FILE *out = stdout;
    if (argc >= 3) {
        // Abrir arquivo de saída em modo de escrita (limpa o conteúdo anterior)
        out = fopen(argv[2], "w");
        if (!out) {
            fprintf(stderr, "Cannot open output file %s\n", argv[2]);
            fclose(yyin);
            exit(2);  // Código 2: arquivo inexistente (saída)
        }
    }
    
    yyrestart(yyin);
    ast_output_file = out;
    
    int parse_result = yyparse();
    if (parse_result != 0) {
        // Erro de sintaxe
        fclose(yyin);
        if (out != stdout) fclose(out);
        exit(3);  // Código 3: erro de sintaxe
    }
    
    printf("\nAnálise semântica...\n\n");
    // Realizar análise semântica
    semanticAnalysis(ast_root);
    
    // Decompilação da AST
    if (ast_root) {
        // Usar a nova função simplificada para descompilação
        astDecompileSimple(ast_root, out);
        
        // Etapa 5: Geração de código intermediário (TACs)
        fprintf(stderr, "\nGenerating intermediate code (TACs)...\n\n");
        TAC* code = generateCode(ast_root);
        
        // Imprimir TACs
        if (code) {
            fprintf(stderr, "\nIntermediate Code:\n\n");
            // Usar tacPrint para imprimir cada TAC na ordem correta
            TAC* current = code;
            while (current) {
                tacPrint(current);
                current = current->next;
            }
        } else {
            fprintf(stderr, "No TACs generated!\n");
        }
    }
    
    fclose(yyin);
    if (out != stdout) fclose(out);
    
    // Imprimir tabela de símbolos
    symbolPrintTable();

    // Verificar se houve erros semânticos
    int semantic_errors = getSemanticErrorCount();
    if (semantic_errors > 0) {
        fprintf(stderr, "\nFound %d semantic errors.\n", semantic_errors);
        exit(4);  // Código 4: existência de erros semânticos
    }
    
    fprintf(stderr, "\nCompilation successful.\n");
    return 0;  // Código 0: sucesso
}
