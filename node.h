#ifndef NODE_H
#define NODE_H

typedef struct {
    char nome;         // Nome do nó (A, B, C, etc.)
    double x;          // Posição x (coordenada)
    double y;          // Posição y (coordenada)
    double force_x;    // Força aplicada no eixo x
    double force_y;    // Força aplicada no eixo y
    char vinculo;      // Tipo de vínculo: 
                       // N - Nó livre (sem vínculo)
                       // P - Pino (reações x e y)
                       // X - Rolete (reação vertical)
                       // Y - Apoio lateral (reação horizontal)
    double reacao_x;   // Força de reação em x (para cálculo futuro)
    double reacao_y;   // Força de reação em y (para cálculo futuro)

} Node;

// Funções para manipulação de nós
Node* create_node(char nome, double x, double y, double force_x, double force_y, char vinculo);
void print_node(const Node* node);
void free_node(Node* node);

#endif