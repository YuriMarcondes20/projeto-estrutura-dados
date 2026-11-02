#ifndef BST_H
#define BST_H

#include "registro.h"

/* Arvore binaria de busca (BST) indexada pelo ID do registro.
   Regra: IDs menores vao para a esquerda, IDs maiores para a direita. */

typedef struct NoBST {
    Registro dado;
    struct NoBST *esq;
    struct NoBST *dir;
} NoBST;

typedef struct {
    NoBST *raiz;
    int tamanho;
} BST;

/* Codigos de retorno de bst_inserir */
#define BST_OK            1
#define BST_DUPLICADO     0
#define BST_SEM_MEMORIA  -1

void            bst_inicializar(BST *arvore);
int             bst_inserir(BST *arvore, const Registro *r);
const Registro *bst_buscar(const BST *arvore, int id);   /* NULL se nao achar */
int             bst_remover(BST *arvore, int id);        /* 1 = removeu, 0 = nao existe */
void            bst_listar_em_ordem(const BST *arvore);  /* ordem crescente de ID */
const Registro *bst_menor(const BST *arvore);            /* NULL se vazia */
const Registro *bst_maior(const BST *arvore);            /* NULL se vazia */
int             bst_copiar_para_vetor(const BST *arvore, Registro *vetor);
void            bst_liberar(BST *arvore);

#endif
