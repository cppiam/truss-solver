#ifndef EQUACOES_H
#define EQUACOES_H

#include "trelica.h"
#include <wchar.h>

#define MAX_EQUACOES 100
#define MAX_INCOGNITAS 100

void montar_matriz_equilibrio(const Trelica* trelica);
void resolver_sistema_gauss(double **A, double *b, int n, int incognitas, const Trelica* trelica, int* mapa_incognitas);

// Função para buscar equaações de coolinearidade (a fim de testes)
void adicionar_equacoes_colinearidade(double*** A, double** b, int* n, int total_incognitas, 
                                    const Trelica* trelica, int* mapa_incognitas);

// Calcular as forças internas nas barras
void calcular_forcas_barras(const Trelica* trelica);
void calcular_deformacoes_componentes(const Trelica* trelica);

#endif
