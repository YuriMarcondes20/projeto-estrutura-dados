#include <stddef.h>
#include "sistema.h"

void sistema_inicializar(Sistema *s) {
    bst_inicializar(&s->bst);
    grafo_inicializar(&s->grafo);
    pilha_inicializar(&s->historico);
    s->simular_falha_vertice = 0;
}

void sistema_liberar(Sistema *s) {
    bst_liberar(&s->bst);
    pilha_liberar(&s->historico);
    grafo_inicializar(&s->grafo);
}

/* ---------- CADASTRO (BST + vertice + historico, com rollback) ---------- */
int sistema_cadastrar(Sistema *s, const Registro *r) {
    int rb, rg;

    /* Etapa 1: BST */
    rb = bst_inserir(&s->bst, r);
    if (rb == BST_DUPLICADO) {
        return SIS_ID_DUPLICADO;           /* nada foi alterado */
    }
    if (rb == BST_SEM_MEMORIA) {
        return SIS_SEM_MEMORIA;            /* nada foi alterado */
    }

    /* Etapa 2: vertice com o mesmo ID */
    if (s->simular_falha_vertice) {
        rg = GRAFO_SEM_MEMORIA;            /* simulacao de falha (testes) */
    } else {
        rg = grafo_criar_vertice(&s->grafo, r->id);
    }
    if (rg != GRAFO_OK) {
        bst_remover(&s->bst, r->id);       /* ROLLBACK: desfaz a insercao na BST */
        if (rg == GRAFO_CHEIO) {
            return SIS_CAPACIDADE_CHEIA;
        }
        if (rg == GRAFO_SEM_MEMORIA) {
            return SIS_SEM_MEMORIA;
        }
        return SIS_INCONSISTENTE;
    }

    /* Etapa 3: historico (so depois de tudo ter dado certo) */
    if (!pilha_empilhar(&s->historico, ACAO_CADASTRO, r->id, 0)) {
        grafo_remover_vertice(&s->grafo, r->id);   /* ROLLBACK completo */
        bst_remover(&s->bst, r->id);
        return SIS_SEM_MEMORIA;
    }
    return SIS_OK;
}

/* ---------- REMOCAO (BST + vertice + arestas) ---------- */
int sistema_remover(Sistema *s, int id) {
    if (bst_buscar(&s->bst, id) == NULL) {
        return SIS_ID_INEXISTENTE;         /* nada foi alterado */
    }
    if (grafo_remover_vertice(&s->grafo, id) != GRAFO_OK) {
        return SIS_INCONSISTENTE;          /* BST ainda intacta */
    }
    bst_remover(&s->bst, id);
    if (!pilha_empilhar(&s->historico, ACAO_REMOCAO, id, 0)) {
        return SIS_HISTORICO_FALHOU;       /* removeu, mas nao registrou */
    }
    return SIS_OK;
}

static int traduz_erro_grafo(int g) {
    switch (g) {
        case GRAFO_VERTICE_INEXISTENTE: return SIS_ID_INEXISTENTE;
        case GRAFO_LACO:                return SIS_LACO;
        case GRAFO_ARESTA_DUPLICADA:    return SIS_ARESTA_DUPLICADA;
        case GRAFO_ARESTA_INEXISTENTE:  return SIS_ARESTA_INEXISTENTE;
        default:                        return SIS_INCONSISTENTE;
    }
}

/* ---------- CONEXOES ---------- */
int sistema_adicionar_conexao(Sistema *s, int a, int b) {
    int g = grafo_adicionar_aresta(&s->grafo, a, b);
    if (g != GRAFO_OK) {
        return traduz_erro_grafo(g);
    }
    if (!pilha_empilhar(&s->historico, ACAO_ADD_CONEXAO, a, b)) {
        grafo_remover_aresta(&s->grafo, a, b);     /* ROLLBACK */
        return SIS_SEM_MEMORIA;
    }
    return SIS_OK;
}

int sistema_remover_conexao(Sistema *s, int a, int b) {
    int g = grafo_remover_aresta(&s->grafo, a, b);
    if (g != GRAFO_OK) {
        return traduz_erro_grafo(g);
    }
    if (!pilha_empilhar(&s->historico, ACAO_REM_CONEXAO, a, b)) {
        grafo_adicionar_aresta(&s->grafo, a, b);   /* ROLLBACK */
        return SIS_SEM_MEMORIA;
    }
    return SIS_OK;
}
