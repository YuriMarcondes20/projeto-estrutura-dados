#include <stdlib.h>
#include "fila.h"

void fila_inicializar(Fila *f) {
    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
}

int fila_vazia(const Fila *f) {
    return f->inicio == NULL;
}

int fila_enfileirar(Fila *f, int valor) {
    NoFila *novo = (NoFila *)malloc(sizeof(NoFila));
    if (novo == NULL) {
        return 0;
    }
    novo->valor = valor;
    novo->prox = NULL;

    if (f->fim == NULL) {          /* fila vazia */
        f->inicio = novo;
    } else {
        f->fim->prox = novo;
    }
    f->fim = novo;
    f->tamanho++;
    return 1;
}

int fila_desenfileirar(Fila *f, int *valor) {
    NoFila *removido;

    if (f->inicio == NULL) {
        return 0;
    }
    removido = f->inicio;
    *valor = removido->valor;
    f->inicio = removido->prox;
    if (f->inicio == NULL) {       /* ficou vazia: fim tambem volta a NULL */
        f->fim = NULL;
    }
    free(removido);
    f->tamanho--;
    return 1;
}

void fila_liberar(Fila *f) {
    int descartado;
    while (fila_desenfileirar(f, &descartado)) {
        /* so esvazia */
    }
}
