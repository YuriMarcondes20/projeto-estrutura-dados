#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

void lista_inicializar(Lista *l) {
    l->inicio = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}

/* Insere no fim em O(1), porque guardamos o ponteiro "fim". */
int lista_inserir_fim(Lista *l, int id) {
    NoLista *novo = (NoLista *)malloc(sizeof(NoLista));
    if (novo == NULL) {
        return 0;
    }
    novo->id = id;
    novo->prox = NULL;

    if (l->fim == NULL) {          /* lista vazia */
        l->inicio = novo;
    } else {
        l->fim->prox = novo;
    }
    l->fim = novo;
    l->tamanho++;
    return 1;
}

void lista_imprimir(const Lista *l) {
    const NoLista *p = l->inicio;
    while (p != NULL) {
        printf("%d", p->id);
        if (p->prox != NULL) {
            printf(" -> ");
        }
        p = p->prox;
    }
    printf("\n");
}

void lista_liberar(Lista *l) {
    NoLista *p = l->inicio;
    while (p != NULL) {
        NoLista *prox = p->prox;
        free(p);
        p = prox;
    }
    l->inicio = NULL;
    l->fim = NULL;
    l->tamanho = 0;
}
