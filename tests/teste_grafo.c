#include <stdio.h>
#include "grafo.h"

static int falhas = 0;

static void confere(const char *nome, int condicao) {
    printf("[%s] %s\n", condicao ? "OK" : "FALHOU", nome);
    if (!condicao) {
        falhas++;
    }
}

/* Confere se a lista tem exatamente os IDs esperados, na ordem. */
static int lista_igual(const Lista *l, const int *esperado, int n) {
    const NoLista *p = l->inicio;
    int i;
    if (l->tamanho != n) {
        return 0;
    }
    for (i = 0; i < n; i++) {
        if (p == NULL || p->id != esperado[i]) {
            return 0;
        }
        p = p->prox;
    }
    return 1;
}

int main(void) {
    static Grafo g;
    Lista l;
    int i;

    grafo_inicializar(&g);
    confere("criar vertice 10", grafo_criar_vertice(&g, 10) == GRAFO_OK);
    grafo_criar_vertice(&g, 25);
    grafo_criar_vertice(&g, 40);
    grafo_criar_vertice(&g, 60);
    grafo_criar_vertice(&g, 90);
    grafo_criar_vertice(&g, 95);
    confere("6 vertices", g.qtd == 6);
    confere("vertice duplicado rejeitado",
            grafo_criar_vertice(&g, 25) == GRAFO_VERTICE_DUPLICADO);

    /* Adiciona 10-40 ANTES de 10-25 para testar a ordem crescente */
    confere("aresta 10-40", grafo_adicionar_aresta(&g, 10, 40) == GRAFO_OK);
    confere("aresta 10-25", grafo_adicionar_aresta(&g, 10, 25) == GRAFO_OK);
    confere("aresta 25-60", grafo_adicionar_aresta(&g, 25, 60) == GRAFO_OK);
    confere("aresta 90-95", grafo_adicionar_aresta(&g, 90, 95) == GRAFO_OK);

    confere("laco (10,10) rejeitado",
            grafo_adicionar_aresta(&g, 10, 10) == GRAFO_LACO);
    confere("duplicada (10,25) rejeitada",
            grafo_adicionar_aresta(&g, 10, 25) == GRAFO_ARESTA_DUPLICADA);
    confere("duplicada invertida (25,10) rejeitada",
            grafo_adicionar_aresta(&g, 25, 10) == GRAFO_ARESTA_DUPLICADA);
    confere("ID inexistente rejeitado",
            grafo_adicionar_aresta(&g, 10, 999) == GRAFO_VERTICE_INEXISTENTE);

    confere("existe aresta 10-25", grafo_existe_aresta(&g, 10, 25));
    confere("existe aresta 25-10 (nao direcionado)",
            grafo_existe_aresta(&g, 25, 10));
    confere("nao existe aresta 10-60", !grafo_existe_aresta(&g, 10, 60));

    /* BFS a partir do 10: 10, depois 25 e 40 (crescente), depois 60 */
    {
        int esperado[] = {10, 25, 40, 60};
        lista_inicializar(&l);
        confere("bfs(10) retorna OK", grafo_bfs(&g, 10, &l) == GRAFO_OK);
        printf("  bfs(10): ");
        lista_imprimir(&l);
        confere("bfs(10) = 10,25,40,60", lista_igual(&l, esperado, 4));
        lista_liberar(&l);
    }
    {
        int esperado[] = {90, 95};
        lista_inicializar(&l);
        grafo_bfs(&g, 95, &l);
        confere("bfs(95) = 95,90 (outra componente)",
                l.tamanho == 2 && l.inicio->id == 95 && l.fim->id == 90);
        (void)esperado;
        lista_liberar(&l);
    }
    lista_inicializar(&l);
    confere("bfs de origem inexistente",
            grafo_bfs(&g, 999, &l) == GRAFO_VERTICE_INEXISTENTE
            && l.tamanho == 0);
    lista_liberar(&l);

    confere("caminho 10 -> 60: sim", grafo_existe_caminho(&g, 10, 60) == 1);
    confere("caminho 60 -> 10: sim", grafo_existe_caminho(&g, 60, 10) == 1);
    confere("caminho 10 -> 90: nao", grafo_existe_caminho(&g, 10, 90) == 0);
    confere("caminho 10 -> 10: sim", grafo_existe_caminho(&g, 10, 10) == 1);
    confere("caminho com ID inexistente",
            grafo_existe_caminho(&g, 10, 999) == GRAFO_VERTICE_INEXISTENTE);

    /* Ciclo: 10-25-40-10 nao pode causar laco infinito na BFS */
    grafo_adicionar_aresta(&g, 25, 40);
    lista_inicializar(&l);
    grafo_bfs(&g, 10, &l);
    confere("ciclo: bfs termina e nao repete", l.tamanho == 4);
    lista_liberar(&l);

    /* Remover aresta */
    confere("remove aresta 10-25", grafo_remover_aresta(&g, 10, 25) == GRAFO_OK);
    confere("aresta sumiu nos dois sentidos",
            !grafo_existe_aresta(&g, 10, 25) && !grafo_existe_aresta(&g, 25, 10));
    confere("remover aresta inexistente",
            grafo_remover_aresta(&g, 10, 25) == GRAFO_ARESTA_INEXISTENTE);

    /* Remover vertice apaga as arestas dele */
    confere("remove vertice 25", grafo_remover_vertice(&g, 25) == GRAFO_OK);
    confere("25 nao existe mais", !grafo_existe_vertice(&g, 25));
    confere("aresta 25-60 sumiu", !grafo_existe_aresta(&g, 25, 60));
    confere("remover vertice inexistente",
            grafo_remover_vertice(&g, 25) == GRAFO_VERTICE_INEXISTENTE);

    /* Reaproveita a posicao livre sem herdar arestas antigas */
    grafo_criar_vertice(&g, 77);
    confere("vertice novo nao herda arestas",
            !grafo_existe_aresta(&g, 77, 60) && !grafo_existe_aresta(&g, 77, 40));
    grafo_criar_vertice(&g, 25);
    confere("recriar o 25 vem sem arestas antigas",
            !grafo_existe_aresta(&g, 25, 60) && !grafo_existe_aresta(&g, 25, 40));

    /* Limite de 100 vertices */
    grafo_inicializar(&g);
    for (i = 1; i <= MAX_VERTICES; i++) {
        grafo_criar_vertice(&g, i * 1000);   /* IDs grandes, como 5000 */
    }
    confere("100 vertices cabem", g.qtd == MAX_VERTICES);
    confere("101o vertice e recusado",
            grafo_criar_vertice(&g, 1) == GRAFO_CHEIO);
    confere("aresta entre IDs grandes",
            grafo_adicionar_aresta(&g, 5000, 100000) == GRAFO_OK);
    grafo_remover_vertice(&g, 5000);
    confere("apos remover, volta a caber um novo",
            grafo_criar_vertice(&g, 1) == GRAFO_OK);

    if (falhas == 0) {
        printf("\nTodos os testes passaram.\n");
        return 0;
    }
    printf("\n%d teste(s) falharam.\n", falhas);
    return 1;
}
