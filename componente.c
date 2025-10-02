    #include <stdio.h>
    #include <stdlib.h>
    #include <locale.h>
    #include "componente.h"

    Componente* create_componente(char node1_nome, char node2_nome) {
        Componente* comp = (Componente*)malloc(sizeof(Componente));
        if (comp == NULL) {
            fprintf(stderr, "Erro ao alocar memória para o componente\n");
            exit(EXIT_FAILURE);
        }
        comp->node1_nome = node1_nome;
        comp->node2_nome = node2_nome;
        comp->forca_reacao = 0.0;
        // Valores arbitrários para madeira
        comp->area = 0.0001;        // 100 mm²
        comp->E = 1.0e10;           // 10 GPa
        comp->comprimento = 1.0;    // 1 metro (valor padrão, pode ser ajustado depois)
        comp->deformacao = 0.0;
        return comp;
    }

    void print_componente(const Componente* componente) {
        printf("Componente %c-%c:\n", componente->node1_nome, componente->node2_nome);
        printf("  Força de reação: %.2f\n", componente->forca_reacao);
        printf("  Área: %.6f m²\n", componente->area);
        printf("  Módulo de elasticidade: %.2e Pa\n", componente->E);
        printf("  Comprimento: %.2f m\n", componente->comprimento);
        printf("  Deformação: %.6e\n", componente->deformacao);
    }

    void free_componente(Componente* componente) {
        free(componente);
    }
