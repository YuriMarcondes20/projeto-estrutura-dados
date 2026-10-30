#ifndef LISTA_H
#define LISTA_H

/* Lista encadeada simples de IDs.
   Usada para guardar a ordem em que a BFS descobre os vertices. */

typedef struct NoLista {
    int id;
    struct NoLista *prox;
} NoLista;

typedef struct {
    NoLista *inicio;
    NoLista *fim;
    int tamanho;
} Lista;

void lista_inicializar(Lista *l);
int  lista_inserir_fim(Lista *l, int id);   /* 1 = ok, 0 = sem memoria */
void lista_imprimir(const Lista *l);
void lista_liberar(Lista *l);

#endif
