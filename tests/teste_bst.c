#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst.h"
#include "ordenacao.h"

static int falhas = 0;

static void confere(const char *nome, int condicao) {
    printf("[%s] %s\n", condicao ? "OK" : "FALHOU", nome);
    if (!condicao) {
        falhas++;
    }
}

static Registro novo(int id, const char *nome) {
    Registro r;
    r.id = id;
    strcpy(r.nome, nome);
    strcpy(r.tipo, "Switch");
    strcpy(r.localizacao, "Sala X");
    return r;
}

/* Confere que a lista em-ordem esta crescente e tem "esperado" itens. */
static int em_ordem_ok(const BST *a, int esperado) {
    Registro v[100];
    int n = bst_copiar_para_vetor(a, v);
    int i;
    if (n != esperado) {
        return 0;
    }
    for (i = 1; i < n; i++) {
        if (v[i - 1].id >= v[i].id) {
            return 0;
        }
    }
    return 1;
}

int main(void) {
    BST a;
    Registro r;
    int ids[] = {50, 30, 70, 20, 40, 60, 80};
    int i;

    bst_inicializar(&a);
    confere("vazia: menor e maior sao NULL",
            bst_menor(&a) == NULL && bst_maior(&a) == NULL);
    confere("vazia: remover falha", bst_remover(&a, 10) == 0);
    confere("vazia: buscar devolve NULL", bst_buscar(&a, 10) == NULL);

    for (i = 0; i < 7; i++) {
        r = novo(ids[i], "Equip");
        bst_inserir(&a, &r);
    }
    confere("inseriu 7", a.tamanho == 7);
    confere("em-ordem crescente", em_ordem_ok(&a, 7));

    r = novo(40, "Repetido");
    confere("ID duplicado e rejeitado", bst_inserir(&a, &r) == BST_DUPLICADO);
    confere("tamanho continua 7", a.tamanho == 7);

    confere("buscar 60 acha", bst_buscar(&a, 60) != NULL
            && bst_buscar(&a, 60)->id == 60);
    confere("buscar 99 nao acha", bst_buscar(&a, 99) == NULL);
    confere("menor e 20", bst_menor(&a)->id == 20);
    confere("maior e 80", bst_maior(&a)->id == 80);

    /* Caso 1: folha */
    confere("remove folha (20)", bst_remover(&a, 20) == 1);
    confere("20 sumiu", bst_buscar(&a, 20) == NULL);
    confere("ordem ok apos folha", em_ordem_ok(&a, 6));

    /* Caso 2: um filho (30 ficou so com o 40) */
    confere("remove no com 1 filho (30)", bst_remover(&a, 30) == 1);
    confere("40 continua la", bst_buscar(&a, 40) != NULL);
    confere("ordem ok apos 1 filho", em_ordem_ok(&a, 5));

    /* Caso 3: dois filhos (a raiz 50 tem 40 e 70) */
    confere("remove raiz com 2 filhos (50)", bst_remover(&a, 50) == 1);
    confere("50 sumiu", bst_buscar(&a, 50) == NULL);
    confere("raiz virou 60 (sucessor)", a.raiz->dado.id == 60);
    confere("ordem ok apos 2 filhos", em_ordem_ok(&a, 4));

    confere("remover inexistente falha", bst_remover(&a, 999) == 0);
    confere("tamanho 4", a.tamanho == 4);

    printf("\nListagem em-ordem:\n");
    bst_listar_em_ordem(&a);

    bst_liberar(&a);
    confere("vazia apos liberar", a.raiz == NULL && a.tamanho == 0);

    /* Ordenacao por nome, com desempate pelo ID */
    {
        Registro v[5];
        v[0] = novo(5, "Switch B");
        v[1] = novo(2, "Roteador");
        v[2] = novo(9, "Switch A");
        v[3] = novo(1, "Switch B");
        v[4] = novo(7, "Access Point");
        ordenar_por_nome(v, 5);
        confere("ordenacao: 1o Access Point", v[0].id == 7);
        confere("ordenacao: 2o Roteador", v[1].id == 2);
        confere("ordenacao: 3o Switch A", v[2].id == 9);
        confere("ordenacao: empate desempata por ID (1 antes de 5)",
                v[3].id == 1 && v[4].id == 5);
    }

    if (falhas == 0) {
        printf("\nTodos os testes passaram.\n");
        return 0;
    }
    printf("\n%d teste(s) falharam.\n", falhas);
    return 1;
}
