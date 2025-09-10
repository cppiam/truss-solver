#ifndef COMPONENTE_H
#define COMPONENTE_H

typedef struct {
    char node1_nome;   // Nome do primeiro nó do componente
    char node2_nome;   // Nome do segundo nó do componente
    double forca_reacao; // Força de reação no componente (para cálculo futuro)
} Componente;

// Funções para manipulação de componentes
Componente* create_componente(char node1_nome, char node2_nome);
void print_componente(const Componente* componente);
void free_componente(Componente* componente);

#endif