#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "trelica.h"
#include "equacoes.h"


Trelica* create_trelica() {
    Trelica* trelica = (Trelica*)malloc(sizeof(Trelica));
    if (trelica == NULL) {
        fprintf(stderr, "Erro ao alocar memória para a treliça\n");
        exit(EXIT_FAILURE);
    }
    
    trelica->nodes = NULL;
    trelica->componentes = NULL;
    trelica->node_count = 0;
    trelica->componente_count = 0;
    
    return trelica;
}

void add_node_to_trelica(Trelica* trelica, Node* node) {
    trelica->nodes = (Node**)realloc(trelica->nodes, (trelica->node_count + 1) * sizeof(Node*));
    if (trelica->nodes == NULL) {
        fprintf(stderr, "Erro ao realocar memória para nós\n");
        exit(EXIT_FAILURE);
    }
    
    trelica->nodes[trelica->node_count] = node;
    trelica->node_count++;
}

void add_componente_to_trelica(Trelica* trelica, Componente* componente) {
    trelica->componentes = (Componente**)realloc(trelica->componentes, 
                                               (trelica->componente_count + 1) * sizeof(Componente*));
    if (trelica->componentes == NULL) {
        fprintf(stderr, "Erro ao realocar memória para componentes\n");
        exit(EXIT_FAILURE);
    }
    
    trelica->componentes[trelica->componente_count] = componente;
    trelica->componente_count++;
}

void print_trelica_info(const Trelica* trelica) {
    printf("=== INFORMAÇÕES DA TRELIÇA ===\n");
    printf("Número de nós: %d\n", trelica->node_count);
    printf("Número de componentes: %d\n", trelica->componente_count);
    
    printf("\n=== DETALHES DOS NÓS ===\n");
    for (int i = 0; i < trelica->node_count; i++) {
        print_node(trelica->nodes[i]);
        printf("\n");
    }
    
    printf("\n=== COMPONENTES ===\n");
    for (int i = 0; i < trelica->componente_count; i++) {
        print_componente(trelica->componentes[i]);
        printf("\n");
    }
}

Trelica* read_trelica_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir arquivo %s\n", filename);
        return NULL;
    }
    
    Trelica* trelica = create_trelica();
    int n, m;
    
    // Lê número de nós e componentes
    if (fscanf(file, "%d ; %d", &n, &m) != 2) {
        fprintf(stderr, "Erro ao ler número de nós e componentes\n");
        fclose(file);
        free_trelica(trelica);
        return NULL;
    }
    
    // Lê nós
    for (int i = 0; i < n; i++) {
        char nome;
        double x, y;
        if (fscanf(file, " %c ; %lf ; %lf", &nome, &x, &y) != 3) {
            fprintf(stderr, "Erro ao ler nó %d\n", i);
            fclose(file);
            free_trelica(trelica);
            return NULL;
        }
        Node* node = create_node(nome, x, y, 0.0, 0.0, 'N');
        add_node_to_trelica(trelica, node);
    }
    
    // Lê matriz de adjacência (formato: 0 ; 1 ; 0)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int conexao;
            if (j < n - 1) {
                if (fscanf(file, "%d ;", &conexao) != 1) {
                    fprintf(stderr, "Erro ao ler conexão [%d][%d] da matriz\n", i, j);
                    fclose(file);
                    free_trelica(trelica);
                    return NULL;
                }
            } else {
                if (fscanf(file, "%d", &conexao) != 1) {
                    fprintf(stderr, "Erro ao ler conexão [%d][%d] da matriz\n", i, j);
                    fclose(file);
                    free_trelica(trelica);
                    return NULL;
                }
            }
            
            // Cria componente se há conexão e i < j (evita duplicatas)
            if (conexao == 1 && i < j) {
                Componente* comp = create_componente(trelica->nodes[i]->nome, trelica->nodes[j]->nome);
                add_componente_to_trelica(trelica, comp);
            }
        }
    }
    
    // Lê forças de carregamento
    for (int i = 0; i < n; i++) {
        double fx, fy;
        if (fscanf(file, "%lf ; %lf", &fx, &fy) != 2) {
            fprintf(stderr, "Erro ao ler forças do nó %d\n", i);
            fclose(file);
            free_trelica(trelica);
            return NULL;
        }
        trelica->nodes[i]->force_x = fx;
        trelica->nodes[i]->force_y = fy;
    }
    
    // Lê vínculos
    for (int i = 0; i < n; i++) {
        char vinculo;
        int c;
        do {
            c = fgetc(file);
        } while (isspace(c) && c != EOF);
        
        if (c == EOF) {
            fprintf(stderr, "Erro: fim de arquivo inesperado ao ler vínculos\n");
            fclose(file);
            free_trelica(trelica);
            return NULL;
        }
        
        vinculo = (char)c;
        trelica->nodes[i]->vinculo = vinculo;
    }
    
    fclose(file);
    return trelica;
}

void free_trelica(Trelica* trelica) {
    for (int i = 0; i < trelica->node_count; i++) {
        free_node(trelica->nodes[i]);
    }
    free(trelica->nodes);
    
    for (int i = 0; i < trelica->componente_count; i++) {
        free_componente(trelica->componentes[i]);
    }
    free(trelica->componentes);
    
    free(trelica);
}

int montar_equacoes_equilibrio(const Trelica* trelica) {
    montar_matriz_equilibrio(trelica);
    return 0;
}