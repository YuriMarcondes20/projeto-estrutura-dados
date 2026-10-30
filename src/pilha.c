#include <stdio.h>
#include <stdlib.h>
#include "pilha.h"

void pilha_inicializar(Pilha *p) {
    p->topo = NULL;
    p->tamanho = 0;
}

int pilha_vazia(const Pilha *p) {
    return p->topo == NULL;
}

int pilha_empilhar(Pilha *p, TipoAcao tipo, int id1, int id2) {
    NoPilha *novo = (NoPilha *)malloc(sizeof(NoPilha));
    if (novo == NULL) {
        return 0;
    }
    novo->acao.tipo = tipo;
    novo->acao.id1 = id1;
    novo->acao.id2 = id2;
    novo->prox = p->topo;     /* o novo aponta para o antigo topo */
    p->topo = novo;
    p->tamanho++;
    return 1;
}

int pilha_desempilhar(Pilha *p, Acao *saida) {
    NoPilha *removido;

    if (p->topo == NULL) {
        return 0;
    }
    removido = p->topo;
    *saida = removido->acao;
    p->topo = removido->prox;
    free(removido);
    p->tamanho--;
    return 1;
}

/* Percorre a partir do topo (mais recente) sem remover nada. */
void pilha_listar_ultimas(const Pilha *p, int n) {
    const NoPilha *atual = p->topo;
    int i = 1;

    if (atual == NULL) {
        printf("Historico vazio.\n");
        return;
    }
    while (atual != NULL && i <= n) {
        switch (atual->acao.tipo) {
            case ACAO_CADASTRO:
                printf("%d. Cadastro do ID %d\n", i, atual->acao.id1);
                break;
            case ACAO_REMOCAO:
                printf("%d. Remocao do ID %d\n", i, atual->acao.id1);
                break;
            case ACAO_ADD_CONEXAO:
                printf("%d. Conexao adicionada: %d - %d\n",
                       i, atual->acao.id1, atual->acao.id2);
                break;
            case ACAO_REM_CONEXAO:
                printf("%d. Conexao removida: %d - %d\n",
                       i, atual->acao.id1, atual->acao.id2);
                break;
        }
        atual = atual->prox;
        i++;
    }
}

void pilha_liberar(Pilha *p) {
    Acao descartada;
    while (pilha_desempilhar(p, &descartada)) {
        /* so esvazia */
    }
}
