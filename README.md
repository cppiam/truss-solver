# truss-solver
Um programa em C para análise estática de treliças 2D que calcula reações de apoio e forças internas nas barras.


## Funcionamento

O programa lê a definição da treliça de um arquivo, monta e resolve as equações de equilíbrio usando eliminação de Gauss, e calcula as forças nas barras (tração/compressão). Inclui visualização OpenGL para representar graficamente a treliça com cores indicando o tipo de força.

## Formato do Arquivo de Treliça

```
3; 2
A; 0.0; 0.0
B; 3.0; 0.0
C; 1.0; 2.0
0; 0; 1
0; 0; 1
1; 1; 0
0.0; 0.0
0.0; 0.0
-600.0; 300.0
P
P
N
```

- Linha 1: Número de nós; Número de componentes
- Próximas N linhas: Nós (Nome; Coordenada X; Coordenada Y)
- Matriz de adjacência N×N (0/1 indicando conexões)
- N linhas de forças aplicadas (Força X; Força Y)
- N linhas de vínculos (P=Pino, X=Rolete, Y=Apoio lateral, N=Livre)

## Compilação e Execução

```
# Compilar (Windows com MinGW)
gcc -o trelica.exe main.c node.c componente.c trelica.c equacoes.c visualizacao.c -lopengl32 -lglu32 -lgdi32 -lm

# Executar
.\trelica.exe
```

O programa lerá automaticamente o arquivo `trelica.txt` e abrirá uma janela de visualização OpenGL.
