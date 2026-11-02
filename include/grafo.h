#ifndef GRAFO_H
#define GRAFO_H

#include "lista.h"

/* Grafo NAO direcionado e SEM peso, em matriz de adjacencia.
   Os vertices sao identificados pelo ID do registro (o mesmo da BST).
   Como o ID pode ser qualquer numero (ex.: 5000), nao da para usar o ID
   como indice da matriz. Por isso guardamos um MAPEAMENTO:
       ids[i]   = ID do vertice que ocupa a posicao i
       usado[i] = 1 se a posicao i esta ocupada, 0 se esta livre
   Ao remover um vertice, a posicao fica livre e pode ser reaproveitada. */

#define MAX_VERTICES 100

typedef struct {
    int  ids[MAX_VERTICES];
    char usado[MAX_VERTICES];
    char matriz[MAX_VERTICES][MAX_VERTICES];   /* 1 = existe aresta */
    int  qtd;                                  /* vertices ativos */
} Grafo;

/* Codigos de retorno */
#define GRAFO_OK                 1
#define GRAFO_VERTICE_INEXISTENTE 0
#define GRAFO_VERTICE_DUPLICADO  -1
#define GRAFO_CHEIO              -2
#define GRAFO_LACO               -3   /* aresta (A, A) */
#define GRAFO_ARESTA_DUPLICADA   -4
#define GRAFO_ARESTA_INEXISTENTE -5
#define GRAFO_SEM_MEMORIA        -6

void grafo_inicializar(Grafo *g);
int  grafo_existe_vertice(const Grafo *g, int id);
int  grafo_criar_vertice(Grafo *g, int id);
int  grafo_remover_vertice(Grafo *g, int id);   /* remove tambem as arestas */

int  grafo_adicionar_aresta(Grafo *g, int a, int b);
int  grafo_remover_aresta(Grafo *g, int a, int b);
int  grafo_existe_aresta(const Grafo *g, int a, int b);   /* 1 ou 0 */

/* BFS a partir de "origem". Escreve na lista os IDs alcancaveis, na ordem
   em que foram descobertos (vizinhos em ordem crescente de ID).
   A lista deve vir inicializada e e liberada por quem chamou. */
int  grafo_bfs(const Grafo *g, int origem, Lista *descobertos);

/* 1 se existe caminho entre a e b, 0 se nao, ou codigo de erro (< 0 ou
   GRAFO_VERTICE_INEXISTENTE quando a ou b nao existem). */
int  grafo_existe_caminho(const Grafo *g, int a, int b);

#endif
