#ifndef FILA_H
#define FILA_H

/* Fila encadeada de inteiros (FIFO: o primeiro a entrar e o primeiro a sair).
   Usada dentro da BFS. */

typedef struct NoFila {
    int valor;
    struct NoFila *prox;
} NoFila;

typedef struct {
    NoFila *inicio;   /* de onde sai */
    NoFila *fim;      /* onde entra */
    int tamanho;
} Fila;

void fila_inicializar(Fila *f);
int  fila_vazia(const Fila *f);
int  fila_enfileirar(Fila *f, int valor);     /* 1 = ok, 0 = sem memoria */
int  fila_desenfileirar(Fila *f, int *valor); /* 1 = ok, 0 = fila vazia */
void fila_liberar(Fila *f);

#endif
