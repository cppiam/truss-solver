#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "node.h"

Node* create_node(char nome, double x, double y, double force_x, double force_y, char vinculo) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o nó\n");
        exit(EXIT_FAILURE);
    }
    
    node->nome = nome;
    node->x = x;
    node->y = y;
    node->force_x = force_x;
    node->force_y = force_y;
    node->vinculo = vinculo;
    node->reacao_x = 0.0;
    node->reacao_y = 0.0;
    
    return node;
}

void print_node(const Node* node) {
    printf("Nó %c:\n", node->nome);
    printf("  Posição: (%.2f, %.2f)\n", node->x, node->y);
    printf("  Forças de carregamento: Fx=%.2f, Fy=%.2f\n", node->force_x, node->force_y);
    printf("  Vínculo: %c\n", node->vinculo);
    printf("  Reações (futuro): Rx=%.2f, Ry=%.2f\n", node->reacao_x, node->reacao_y);
}

void free_node(Node* node) {
    free(node);
}