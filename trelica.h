#ifndef TRELICA_H
#define TRELICA_H

#include "node.h"
#include "componente.h"

typedef struct {
    Node** nodes;           // Array de ponteiros para nós
    Componente** componentes; // Array de ponteiros para componentes
    int node_count;         // Número de nós
    int componente_count;   // Número de componentes
} Trelica;

// Funções para manipulação da treliça
Trelica* create_trelica();
void add_node_to_trelica(Trelica* trelica, Node* node);
void add_componente_to_trelica(Trelica* trelica, Componente* componente);
void print_trelica_info(const Trelica* trelica);
void free_trelica(Trelica* trelica);

// Função para ler arquivo
Trelica* read_trelica_from_file(const char* filename);

// Função para montar equações de equilíbrio
int montar_equacoes_equilibrio(const Trelica* trelica);

#endif