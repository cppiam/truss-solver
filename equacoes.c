#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include <locale.h>
#include "equacoes.h"

// Função auxiliar para calcular produto vetorial 2D (componente z)
double produto_vetorial(double rx, double ry, double fx, double fy) {
    return rx * fy - ry * fx;
}

void montar_matriz_equilibrio(const Trelica* trelica) {
    // Para treliça plana: 3 equações de equilíbrio (ΣFx=0, ΣFy=0, ΣM=0)
    int n = 3;
    
    // Contar número total de incógnitas (reações)
    int total_incognitas = 0;
    for (int i = 0; i < trelica->node_count; i++) {
        Node* node = trelica->nodes[i];
        if (node->vinculo == 'P') total_incognitas += 2;
        else if (node->vinculo == 'X' || node->vinculo == 'Y') total_incognitas += 1;
    }
    
    wprintf(L"Sistema: %d equações, %d incógnitas\n", n, total_incognitas);
    
    // Criar mapa de incógnitas: para cada nó, quais colunas correspondem a Rx e Ry
    int* mapa_incognitas = (int*)malloc(trelica->node_count * 2 * sizeof(int));
    int idx_atual = 0;
    
    for (int i = 0; i < trelica->node_count; i++) {
        Node* node = trelica->nodes[i];
        if (node->vinculo == 'P' || node->vinculo == 'Y') {
            mapa_incognitas[i * 2] = idx_atual++; // Rx
        } else {
            mapa_incognitas[i * 2] = -1; // Sem Rx
        }
        
        if (node->vinculo == 'P' || node->vinculo == 'X') {
            mapa_incognitas[i * 2 + 1] = idx_atual++; // Ry
        } else {
            mapa_incognitas[i * 2 + 1] = -1; // Sem Ry
        }
    }
    
    // Alocar matriz A e vetor b
    double** A = (double**)malloc(n * sizeof(double*));
    double* b = (double*)calloc(n, sizeof(double));
    for (int i = 0; i < n; i++) {
        A[i] = (double*)calloc(total_incognitas, sizeof(double));
    }
    
    // EQUAÇÃO 1: ΣFx = 0
    for (int i = 0; i < trelica->node_count; i++) {
        Node* node = trelica->nodes[i];
        
        // Contribuição das forças externas
        b[0] -= node->force_x;
        
        // Contribuição das reações em x
        if (mapa_incognitas[i * 2] != -1) {
            A[0][mapa_incognitas[i * 2]] = 1.0;
        }
    }
    
    // EQUAÇÃO 2: ΣFy = 0
    for (int i = 0; i < trelica->node_count; i++) {
        Node* node = trelica->nodes[i];
        
        // Contribuição das forças externas
        b[1] -= node->force_y;
        
        // Contribuição das reações em y
        if (mapa_incognitas[i * 2 + 1] != -1) {
            A[1][mapa_incognitas[i * 2 + 1]] = 1.0;
        }
    }
    
    // EQUAÇÃO 3: ΣM = 0 (em relação à origem (0,0))
    for (int i = 0; i < trelica->node_count; i++) {
        Node* node = trelica->nodes[i];
        
        // Momento das forças externas
        double momento_forca = produto_vetorial(node->x, node->y, node->force_x, node->force_y);
        b[2] -= momento_forca;
        
        // Momento das reações
        if (mapa_incognitas[i * 2] != -1) { // Rx existe
            // Momento de Rx: -y * Rx
            A[2][mapa_incognitas[i * 2]] += -node->y;
        }
        if (mapa_incognitas[i * 2 + 1] != -1) { // Ry existe
            // Momento de Ry: x * Ry
            A[2][mapa_incognitas[i * 2 + 1]] += node->x;
        }
    }

    // VERIFICAR E ADICIONAR EQUAÇÕES DE COLINARIDADE SE NECESSÁRIO
    adicionar_equacoes_colinearidade(&A, &b, &n, total_incognitas, trelica, mapa_incognitas);
    
    // Mostrar matriz
    wprintf(L"\nMatriz de Coeficientes (A):\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < total_incognitas; j++) {
            wprintf(L"%8.2f ", A[i][j]);
        }
        wprintf(L"| %8.2f\n", b[i]);
    }
    
    // Mostrar equações de forma legível
    wprintf(L"\nSistema de Equações:\n");
    for (int i = 0; i < n; i++) {
        wprintf(L"Eq %d: ", i+1);
        int primeiro_termo = 1;
        
        for (int j = 0; j < total_incognitas; j++) {
            if (fabs(A[i][j]) > 1e-10) { // Evitar zeros numéricos
                if (!primeiro_termo) {
                    wprintf(L" + ");
                }
                
                // Encontrar qual nó e tipo de reação corresponde a esta coluna
                for (int k = 0; k < trelica->node_count; k++) {
                    if (mapa_incognitas[k * 2] == j) {
                        wprintf(L"%.2fRx%c", A[i][j], trelica->nodes[k]->nome);
                        primeiro_termo = 0;
                        break;
                    }
                    if (mapa_incognitas[k * 2 + 1] == j) {
                        wprintf(L"%.2fRy%c", A[i][j], trelica->nodes[k]->nome);
                        primeiro_termo = 0;
                        break;
                    }
                }
            }
        }
        
        if (primeiro_termo) {
            wprintf(L"0");
        }
        
        wprintf(L" = %.2f\n", b[i]);
    }
    
    // Resolver sistema
    resolver_sistema_gauss(A, b, n, total_incognitas, trelica, mapa_incognitas);
    
    // Calcular forças nas barras
    calcular_forcas_barras(trelica);
    
    // Liberar memória
    free(mapa_incognitas);
    for (int i = 0; i < n; i++) free(A[i]);
    free(A);
    free(b);
}

// Algoritimo para resolver sistema linear Ax = b usando Eliminação de Gauss
void resolver_sistema_gauss(double **A, double *b, int n, int incognitas, const Trelica* trelica, int* mapa_incognitas) {
    // Eliminação de Gauss com pivoteamento
    for (int i = 0; i < n; i++) {
        // Pivoteamento parcial
        int max_linha = i;
        for (int k = i + 1; k < n; k++) {
            if (fabs(A[k][i]) > fabs(A[max_linha][i])) {
                max_linha = k;
            }
        }
        
        // Trocar linhas
        if (max_linha != i) {
            for (int j = 0; j < incognitas; j++) {
                double temp = A[i][j];
                A[i][j] = A[max_linha][j];
                A[max_linha][j] = temp;
            }
            double temp_b = b[i];
            b[i] = b[max_linha];
            b[max_linha] = temp_b;
        }
        
        // Verificar se o pivô é zero
        if (fabs(A[i][i]) < 1e-10) {
            wprintf(L"AVISO: Pivô próximo de zero na equação %d\n", i+1);
            continue;
        }
        
        // Eliminação
        for (int k = i + 1; k < n; k++) {
            double factor = A[k][i] / A[i][i];
            for (int j = i; j < incognitas; j++) {
                A[k][j] -= factor * A[i][j];
            }
            b[k] -= factor * b[i];
        }
    }
    
    // Substituição regressiva
    double* x = (double*)calloc(incognitas, sizeof(double));
    for (int i = n - 1; i >= 0; i--) {
        if (fabs(A[i][i]) < 1e-10) {
            if (fabs(b[i]) < 1e-10) {
                x[i] = 0.0; // Sistema indeterminado
            } else {
                x[i] = NAN; // Sistema impossível
            }
            continue;
        }
        
        x[i] = b[i];
        for (int j = i + 1; j < incognitas; j++) {
            x[i] -= A[i][j] * x[j];
        }
        x[i] /= A[i][i];
    }
    
    // Mostrar solução
    wprintf(L"\nSolução do Sistema (Reações nos Nós):\n");
    for (int i = 0; i < trelica->node_count; i++) {
        Node* node = trelica->nodes[i];
        double Rx = 0.0, Ry = 0.0;
        
        if (mapa_incognitas[i * 2] != -1) {
            Rx = x[mapa_incognitas[i * 2]];
        }
        if (mapa_incognitas[i * 2 + 1] != -1) {
            Ry = x[mapa_incognitas[i * 2 + 1]];
        }
        
        // Verificar se a solução é válida
        if (isnan(Rx) || isnan(Ry)) {
            wprintf(L"Nó %c: Rx = %10s, Ry = %10s (Sistema impossível)\n", 
                   node->nome, "NaN", "NaN");
        } else {
            wprintf(L"Nó %c: Rx = %10.2f, Ry = %10.2f\n", node->nome, Rx, Ry);
        }
        
        // Atualizar as reações nos nós (para uso futuro)
        node->reacao_x = Rx;
        node->reacao_y = Ry;
    }
    
    free(x);
}



// Função para tentar adicionar as equações de colinearidade faltantes
void adicionar_equacoes_colinearidade(double*** A, double** b, int* n, int total_incognitas, 
                                    const Trelica* trelica, int* mapa_incognitas) {
    
    // Verificar se precisamos de equações extras
    int equacoes_necessarias = total_incognitas - *n;
    if (equacoes_necessarias <= 0) {
        return; // Não precisa de equações extras
    }
    
    wprintf(L"Adicionando %d equações de colinearidade...\n", equacoes_necessarias);
    
    // Para cada nó com vínculo 'P' (pino) que está conectado a uma barra inclinada
    for (int i = 0; i < trelica->node_count && equacoes_necessarias > 0; i++) {
        Node* node = trelica->nodes[i];
        
        if (node->vinculo == 'P') {
            // Encontrar uma barra conectada a este nó que não seja horizontal/vertical
            for (int j = 0; j < trelica->componente_count; j++) {
                Componente* comp = trelica->componentes[j];
                
                if (comp->node1_nome == node->nome || comp->node2_nome == node->nome) {
                    // Encontrar o nó oposto na barra
                    char outro_no_nome = (comp->node1_nome == node->nome) ? comp->node2_nome : comp->node1_nome;
                    Node* outro_no = NULL;
                    
                    for (int k = 0; k < trelica->node_count; k++) {
                        if (trelica->nodes[k]->nome == outro_no_nome) {
                            outro_no = trelica->nodes[k];
                            break;
                        }
                    }
                    
                    if (outro_no) {
                        // Calcular a direção da barra
                        double dx = outro_no->x - node->x;
                        double dy = outro_no->y - node->y;
                        
                        // Se a barra não é horizontal nem vertical, temos colinearidade
                        if (fabs(dx) > 1e-10 && fabs(dy) > 1e-10) {
                            // A relação é: Ry/Rx = dy/dx (para forças colineares com a barra)
                            // Ou: dx * Ry - dy * Rx = 0
                            
                            // Redimensionar matriz A e vetor b
                            *A = (double**)realloc(*A, (*n + 1) * sizeof(double*));
                            *b = (double*)realloc(*b, (*n + 1) * sizeof(double));
                            
                            (*A)[*n] = (double*)calloc(total_incognitas, sizeof(double));
                            (*b)[*n] = 0.0;
                            
                            // Preencher a nova equação: dx * Ry - dy * Rx = 0
                            if (mapa_incognitas[i * 2] != -1) { // Rx existe
                                (*A)[*n][mapa_incognitas[i * 2]] = -dy;
                            }
                            if (mapa_incognitas[i * 2 + 1] != -1) { // Ry existe
                                (*A)[*n][mapa_incognitas[i * 2 + 1]] = dx;
                            }
                            
                            wprintf(L"Equação de colinearidade para nó %c: %.2fRx%c + %.2fRy%c = 0\n",
                                   node->nome, -dy, node->nome, dx, node->nome);
                            
                            (*n)++;
                            equacoes_necessarias--;
                            break; // Uma equação por nó é suficiente
                        }
                    }
                }
            }
        }
    }
}


// Função para encontrar um nó pelo nome
Node* encontrar_node_por_nome(const Trelica* trelica, char nome) {
    for (int i = 0; i < trelica->node_count; i++) {
        if (trelica->nodes[i]->nome == nome) {
            return trelica->nodes[i];
        }
    }
    return NULL;
}

// Função para calcular o comprimento de uma barra
double calcular_comprimento_barra(Node* node1, Node* node2) {
    double dx = node2->x - node1->x;
    double dy = node2->y - node1->y;
    return sqrt(dx*dx + dy*dy);
}

void calcular_forcas_barras(const Trelica* trelica) {
    wprintf(L"\n=== FORÇAS NAS BARRAS ===\n");

    for (int j = 0; j < trelica->componente_count; j++) {
        Componente* comp = trelica->componentes[j];
        Node* node1 = encontrar_node_por_nome(trelica, comp->node1_nome);
        Node* node2 = encontrar_node_por_nome(trelica, comp->node2_nome);
        if (!node1 || !node2) continue;

        // ============================================================
        // 1) Calcular a direção da barra (de node1 → node2)
        // ============================================================
        double dx = node2->x - node1->x;
        double dy = node2->y - node1->y;
        double len = sqrt(dx*dx + dy*dy);
        if (len < 1e-12) continue; // evita divisão por zero
        double ux = dx / len;  // cosseno diretor em x
        double uy = dy / len;  // cosseno diretor em y

        // DEBUG: mostrar dados básicos
        wprintf(L"\n--- DEBUG Barra %c-%c ---\n", comp->node1_nome, comp->node2_nome);
        wprintf(L"Nó %c: Rx=%.2f, Ry=%.2f\n", node1->nome, node1->reacao_x, node1->reacao_y);
        wprintf(L"Nó %c: Rx=%.2f, Ry=%.2f\n", node2->nome, node2->reacao_x, node2->reacao_y);
        wprintf(L"Direção da barra: ux=%.4f, uy=%.4f\n", ux, uy);

        // ============================================================
        // 2) Projetar as reações dos nós na direção da barra
        // ============================================================
        // Projeção no sentido do vetor (node1 → node2)
        double dot1 = node1->reacao_x * ux + node1->reacao_y * uy;
        // Projeção no sentido oposto (node2 → node1)
        double dot2 = node2->reacao_x * (-ux) + node2->reacao_y * (-uy);

        wprintf(L"  DEBUG Projeção nó %c: (Rx=%.2f * ux=%.4f) + (Ry=%.2f * uy=%.4f) = %.2f\n",
                node1->nome, node1->reacao_x, ux, node1->reacao_y, uy, dot1);

        wprintf(L"  DEBUG Projeção nó %c: (Rx=%.2f * -ux=%.4f) + (Ry=%.2f * -uy=%.4f) = %.2f\n",
                node2->nome, node2->reacao_x, -ux, node2->reacao_y, -uy, dot2);

        // ============================================================
        // 3) Escolher qual projeção usar
        // ============================================================
        double F = (fabs(dot1) > 1e-6) ? dot1 : dot2;

        // ============================================================
        // 4) INVERTER O SINAL para seguir a convenção:
        //    F > 0 → Tração (a barra está sendo esticada)
        //    F < 0 → Compressão (a barra está sendo comprimida)
        // ============================================================
        F = -F; // Esta é a única mudança necessária

        // ============================================================
        // 5) Obter os componentes internos da barra
        // ============================================================
        double Fx = F * ux;
        double Fy = F * uy;

        comp->forca_reacao = F; // guardar para consultas futuras

        // Nova convenção:
        //   F > 0 → Tração (a barra está sendo esticada)
        //   F < 0 → Compressão (a barra está sendo comprimida)
        const wchar_t* tipo = (F > 0) ? L"Tração" : L"Compressão";

        // ============================================================
        // 6) Imprimir resultados
        // ============================================================
        wprintf(L"Barra %c-%c:\n", comp->node1_nome, comp->node2_nome);
        wprintf(L"  Força escalar F = %10.2f N (%ls)\n", F, tipo);
        wprintf(L"  Componentes internos: Fx=%10.2f, Fy=%10.2f\n", Fx, Fy);
        wprintf(L"  Módulo |F| = %10.2f N\n", fabs(F));
    }
}