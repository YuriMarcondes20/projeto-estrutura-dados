#include <string.h>
#include "grafo.h"
#include "fila.h"

void grafo_inicializar(Grafo *g) {
    memset(g, 0, sizeof(Grafo));   /* tudo zerado: nenhuma posicao usada */
}

/* Devolve o indice (posicao na matriz) do vertice com esse ID,
   ou -1 se nao existir. Busca linear: no maximo 100 posicoes. */
static int indice_de(const Grafo *g, int id) {
    int i;
    for (i = 0; i < MAX_VERTICES; i++) {
        if (g->usado[i] && g->ids[i] == id) {
            return i;
        }
    }
    return -1;
}

int grafo_existe_vertice(const Grafo *g, int id) {
    return indice_de(g, id) != -1;
}

int grafo_criar_vertice(Grafo *g, int id) {
    int i;

    if (indice_de(g, id) != -1) {
        return GRAFO_VERTICE_DUPLICADO;
    }
    for (i = 0; i < MAX_VERTICES; i++) {     /* primeira posicao livre */
        if (!g->usado[i]) {
            g->usado[i] = 1;
            g->ids[i] = id;
            g->qtd++;
            return GRAFO_OK;
        }
    }
    return GRAFO_CHEIO;
}

/* Libera a posicao e apaga a linha e a coluna da matriz, para que
   um vertice novo que reutilize a posicao nao herde arestas antigas. */
int grafo_remover_vertice(Grafo *g, int id) {
    int i, k;

    i = indice_de(g, id);
    if (i == -1) {
        return GRAFO_VERTICE_INEXISTENTE;
    }
    for (k = 0; k < MAX_VERTICES; k++) {
        g->matriz[i][k] = 0;
        g->matriz[k][i] = 0;
    }
    g->usado[i] = 0;
    g->qtd--;
    return GRAFO_OK;
}

/* Grafo nao direcionado: a aresta A-B e a mesma que B-A,
   entao marcamos nas duas posicoes da matriz. */
int grafo_adicionar_aresta(Grafo *g, int a, int b) {
    int ia, ib;

    ia = indice_de(g, a);
    ib = indice_de(g, b);
    if (ia == -1 || ib == -1) {
        return GRAFO_VERTICE_INEXISTENTE;
    }
    if (a == b) {
        return GRAFO_LACO;
    }
    if (g->matriz[ia][ib]) {
        return GRAFO_ARESTA_DUPLICADA;       /* vale para (A,B) e (B,A) */
    }
    g->matriz[ia][ib] = 1;
    g->matriz[ib][ia] = 1;
    return GRAFO_OK;
}

int grafo_remover_aresta(Grafo *g, int a, int b) {
    int ia, ib;

    ia = indice_de(g, a);
    ib = indice_de(g, b);
    if (ia == -1 || ib == -1) {
        return GRAFO_VERTICE_INEXISTENTE;
    }
    if (!g->matriz[ia][ib]) {
        return GRAFO_ARESTA_INEXISTENTE;
    }
    g->matriz[ia][ib] = 0;
    g->matriz[ib][ia] = 0;
    return GRAFO_OK;
}

int grafo_existe_aresta(const Grafo *g, int a, int b) {
    int ia = indice_de(g, a);
    int ib = indice_de(g, b);
    if (ia == -1 || ib == -1) {
        return 0;
    }
    return g->matriz[ia][ib];
}

/* Coloca em "viz" os indices dos vizinhos de "v", ordenados pelo ID
   (Insertion Sort). Devolve quantos sao. Necessario porque a ordem das
   posicoes na matriz nao segue a ordem dos IDs. */
static int vizinhos_ordenados(const Grafo *g, int v, int *viz) {
    int n = 0, k, i, j, chave;

    for (k = 0; k < MAX_VERTICES; k++) {
        if (g->usado[k] && g->matriz[v][k]) {
            viz[n++] = k;
        }
    }
    for (i = 1; i < n; i++) {
        chave = viz[i];
        j = i - 1;
        while (j >= 0 && g->ids[viz[j]] > g->ids[chave]) {
            viz[j + 1] = viz[j];
            j--;
        }
        viz[j + 1] = chave;
    }
    return n;
}

/* ---------- BFS (busca em largura) ----------
   Visita o vertice de origem, depois todos os vizinhos dele, depois os
   vizinhos dos vizinhos, e assim por diante (por camadas).
   - A FILA guarda os vertices descobertos que ainda faltam visitar
     (primeiro a entrar, primeiro a sair: garante a ordem por camadas).
   - "visitado" evita visitar o mesmo vertice duas vezes (ciclos).
   - A LISTA guarda a ordem em que os vertices foram descobertos.
   Custo: O(V^2) com matriz de adjacencia (para cada vertice, varre a linha). */
int grafo_bfs(const Grafo *g, int origem, Lista *descobertos) {
    char visitado[MAX_VERTICES];
    int  viz[MAX_VERTICES];
    Fila fila;
    int  io, atual, n, i, resultado = GRAFO_OK;

    io = indice_de(g, origem);
    if (io == -1) {
        return GRAFO_VERTICE_INEXISTENTE;
    }

    memset(visitado, 0, sizeof(visitado));
    fila_inicializar(&fila);

    visitado[io] = 1;                                    /* marca ao enfileirar */
    if (!fila_enfileirar(&fila, io) ||
        !lista_inserir_fim(descobertos, g->ids[io])) {
        fila_liberar(&fila);
        return GRAFO_SEM_MEMORIA;
    }

    while (fila_desenfileirar(&fila, &atual)) {
        n = vizinhos_ordenados(g, atual, viz);
        for (i = 0; i < n; i++) {
            if (!visitado[viz[i]]) {
                visitado[viz[i]] = 1;
                if (!fila_enfileirar(&fila, viz[i]) ||
                    !lista_inserir_fim(descobertos, g->ids[viz[i]])) {
                    resultado = GRAFO_SEM_MEMORIA;
                    break;
                }
            }
        }
        if (resultado != GRAFO_OK) {
            break;
        }
    }
    fila_liberar(&fila);
    return resultado;
}

/* Existe caminho entre a e b se b aparece entre os alcancaveis de a. */
int grafo_existe_caminho(const Grafo *g, int a, int b) {
    Lista alcancaveis;
    const NoLista *p;
    int r, achou = 0;

    if (!grafo_existe_vertice(g, a) || !grafo_existe_vertice(g, b)) {
        return GRAFO_VERTICE_INEXISTENTE;
    }
    lista_inicializar(&alcancaveis);
    r = grafo_bfs(g, a, &alcancaveis);
    if (r != GRAFO_OK) {
        lista_liberar(&alcancaveis);
        return r;
    }
    for (p = alcancaveis.inicio; p != NULL; p = p->prox) {
        if (p->id == b) {
            achou = 1;
            break;
        }
    }
    lista_liberar(&alcancaveis);
    return achou;
}
