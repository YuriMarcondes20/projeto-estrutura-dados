#include <stdio.h>
#include <stdlib.h>
#include "bst.h"

void bst_inicializar(BST *arvore) {
    arvore->raiz = NULL;
    arvore->tamanho = 0;
}

/* ---------- INSERIR ----------
   Desce pela arvore comparando o ID: menor vai para a esquerda,
   maior vai para a direita. Quando chega num lugar vazio (NULL),
   cria o no ali. Recebe "NoBST **" para poder alterar o ponteiro
   do pai (ou a raiz) diretamente. */
static int inserir_no(NoBST **no, const Registro *r, int *resultado) {
    if (*no == NULL) {
        NoBST *novo = (NoBST *)malloc(sizeof(NoBST));
        if (novo == NULL) {
            *resultado = BST_SEM_MEMORIA;
            return 0;
        }
        novo->dado = *r;
        novo->esq = NULL;
        novo->dir = NULL;
        *no = novo;
        *resultado = BST_OK;
        return 1;
    }
    if (r->id < (*no)->dado.id) {
        return inserir_no(&(*no)->esq, r, resultado);
    }
    if (r->id > (*no)->dado.id) {
        return inserir_no(&(*no)->dir, r, resultado);
    }
    *resultado = BST_DUPLICADO;   /* ID igual: nao permite repetir */
    return 0;
}

int bst_inserir(BST *arvore, const Registro *r) {
    int resultado = BST_SEM_MEMORIA;
    if (inserir_no(&arvore->raiz, r, &resultado)) {
        arvore->tamanho++;
    }
    return resultado;
}

/* ---------- BUSCAR ----------
   Mesmo caminho do inserir, mas so procura. Em cada passo descarta
   metade (em media) dos nos: custo O(h), h = altura da arvore. */
const Registro *bst_buscar(const BST *arvore, int id) {
    const NoBST *atual = arvore->raiz;
    while (atual != NULL) {
        if (id == atual->dado.id) {
            return &atual->dado;
        }
        atual = (id < atual->dado.id) ? atual->esq : atual->dir;
    }
    return NULL;
}

/* ---------- MENOR / MAIOR ----------
   Menor ID: sempre para a esquerda ate acabar.
   Maior ID: sempre para a direita ate acabar. */
static NoBST *no_menor(NoBST *no) {
    while (no != NULL && no->esq != NULL) {
        no = no->esq;
    }
    return no;
}

const Registro *bst_menor(const BST *arvore) {
    NoBST *m = no_menor(arvore->raiz);
    return (m == NULL) ? NULL : &m->dado;
}

const Registro *bst_maior(const BST *arvore) {
    NoBST *no = arvore->raiz;
    while (no != NULL && no->dir != NULL) {
        no = no->dir;
    }
    return (no == NULL) ? NULL : &no->dado;
}

/* ---------- REMOVER ----------
   Primeiro acha o no. Depois ha 3 casos:
   1) Sem filhos (folha): libera o no e o pai passa a apontar para NULL.
   2) Um filho so: o pai passa a apontar direto para esse filho.
   3) Dois filhos: nao da para simplesmente apagar. Troca o dado do no
      pelo do SUCESSOR (o menor ID da subarvore da direita, que e o
      proximo na ordem crescente) e depois remove o sucessor, que cai
      no caso 1 ou 2 (ele nunca tem filho esquerdo). */
static int remover_no(NoBST **no, int id) {
    NoBST *alvo;

    if (*no == NULL) {
        return 0;                          /* nao existe */
    }
    if (id < (*no)->dado.id) {
        return remover_no(&(*no)->esq, id);
    }
    if (id > (*no)->dado.id) {
        return remover_no(&(*no)->dir, id);
    }

    alvo = *no;                            /* achou o no a remover */

    if (alvo->esq == NULL && alvo->dir == NULL) {          /* caso 1 */
        *no = NULL;
        free(alvo);
    } else if (alvo->esq == NULL) {                        /* caso 2 */
        *no = alvo->dir;
        free(alvo);
    } else if (alvo->dir == NULL) {                        /* caso 2 */
        *no = alvo->esq;
        free(alvo);
    } else {                                               /* caso 3 */
        NoBST *sucessor = no_menor(alvo->dir);
        alvo->dado = sucessor->dado;                       /* copia o dado */
        remover_no(&alvo->dir, sucessor->dado.id);         /* remove o sucessor */
    }
    return 1;
}

int bst_remover(BST *arvore, int id) {
    if (remover_no(&arvore->raiz, id)) {
        arvore->tamanho--;
        return 1;
    }
    return 0;
}

/* ---------- PERCURSO EM-ORDEM ----------
   esquerda -> no -> direita. Numa BST isso sai em ordem crescente de ID. */
static void listar_no(const NoBST *no) {
    if (no == NULL) {
        return;
    }
    listar_no(no->esq);
    printf("ID %d | %s | %s | %s\n",
           no->dado.id, no->dado.nome, no->dado.tipo, no->dado.localizacao);
    listar_no(no->dir);
}

void bst_listar_em_ordem(const BST *arvore) {
    if (arvore->raiz == NULL) {
        printf("Nenhum equipamento cadastrado.\n");
        return;
    }
    listar_no(arvore->raiz);
}

/* ---------- COPIAR PARA VETOR ----------
   Usa o mesmo percurso em-ordem. O vetor deve ter espaco para
   "arvore->tamanho" registros. Devolve quantos copiou. */
static void copiar_no(const NoBST *no, Registro *vetor, int *pos) {
    if (no == NULL) {
        return;
    }
    copiar_no(no->esq, vetor, pos);
    vetor[*pos] = no->dado;
    (*pos)++;
    copiar_no(no->dir, vetor, pos);
}

int bst_copiar_para_vetor(const BST *arvore, Registro *vetor) {
    int pos = 0;
    copiar_no(arvore->raiz, vetor, &pos);
    return pos;
}

/* ---------- LIBERAR ----------
   Pos-ordem: libera os filhos antes do pai. */
static void liberar_no(NoBST *no) {
    if (no == NULL) {
        return;
    }
    liberar_no(no->esq);
    liberar_no(no->dir);
    free(no);
}

void bst_liberar(BST *arvore) {
    liberar_no(arvore->raiz);
    arvore->raiz = NULL;
    arvore->tamanho = 0;
}
