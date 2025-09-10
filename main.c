#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "trelica.h"
#include "equacoes.h"
#include "visualizacao.h"

int main() {
    setlocale(LC_ALL, ""); // Usa o locale do sistema
    
    wprintf(L"=== PROGRAMA DE ANÁLISE DE TRELIÇAS ===\n\n");
    
    // Lê treliça do arquivo
    wprintf(L"Lendo treliça do arquivo 'trelica.txt'...\n");
    setlocale(LC_ALL, "C");
    Trelica* my_trelica = read_trelica_from_file("trelica.txt");  
    setlocale(LC_ALL, ""); // Usa o locale do sistema
    if (my_trelica == NULL) {
        fwprintf(stderr, L"Erro ao carregar treliça do arquivo\n");
        return 1;
    }
    
    // Mostra informações básicas da treliça
    wprintf(L"Treliça carregada com sucesso!\n");
    wprintf(L"Número de nós: %d\n", my_trelica->node_count);
    wprintf(L"Número de componentes: %d\n\n", my_trelica->componente_count);
    
    // Mostra detalhes dos nós
    wprintf(L"=== DETALHES DOS NÓS ===\n");
    for (int i = 0; i < my_trelica->node_count; i++) {
        wprintf(L"Nó %c: (%.2f, %.2f) - ", my_trelica->nodes[i]->nome, 
               my_trelica->nodes[i]->x, my_trelica->nodes[i]->y);
        wprintf(L"Força: (%.2f, %.2f) - ", my_trelica->nodes[i]->force_x, 
               my_trelica->nodes[i]->force_y);
        wprintf(L"Vínculo: ");
        switch(my_trelica->nodes[i]->vinculo) {
            case 'P': wprintf(L"Pino\n"); break;
            case 'X': wprintf(L"Rolete\n"); break;
            case 'Y': wprintf(L"Apoio lateral\n"); break;
            case 'N': wprintf(L"Livre\n"); break;
            default: wprintf(L"Desconhecido\n");
        }
    }
    wprintf(L"\n");
    
    // Mostra componentes
    wprintf(L"=== COMPONENTES ===\n");
    for (int i = 0; i < my_trelica->componente_count; i++) {
        wprintf(L"Componente %d: %c-%c\n", i+1, 
               my_trelica->componentes[i]->node1_nome, 
               my_trelica->componentes[i]->node2_nome);
    }
    wprintf(L"\n");
    
    // Monta e mostra as equações de equilíbrio
    wprintf(L"Montando equações de equilíbrio...\n");
    montar_equacoes_equilibrio(my_trelica);
    wprintf(L"Equações de equilíbrio montadas.\n\n");

    // Visualizar a treliça com OpenGL
    wprintf(L"Iniciando visualização da treliça...\n");
    visualizar_trelica(my_trelica);
    
    
    // Libera memória
    wprintf(L"\n=== LIBERANDO MEMÓRIA ===\n");
    free_trelica(my_trelica);
    
    wprintf(L"Programa finalizado com sucesso!\n");
    return 0;
}