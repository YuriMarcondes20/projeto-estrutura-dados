#include <stdio.h>
#include "lista.h"
#include "fila.h"
#include "pilha.h"

static int falhas = 0;

static void confere(const char *nome, int condicao) {
    printf("[%s] %s\n", condicao ? "OK" : "FALHOU", nome);
    if (!condicao) {
        falhas++;
    }
}

static void teste_lista(void) {
    Lista l;
    lista_inicializar(&l);

    confere("lista: comeca vazia", l.tamanho == 0 && l.inicio == NULL);
    lista_inserir_fim(&l, 10);
    lista_inserir_fim(&l, 25);
    lista_inserir_fim(&l, 40);
    confere("lista: tamanho 3", l.tamanho == 3);
    confere("lista: primeiro e 10", l.inicio->id == 10);
    confere("lista: ultimo e 40", l.fim->id == 40);
    printf("  ordem: ");
    lista_imprimir(&l);
    lista_liberar(&l);
    confere("lista: vazia apos liberar", l.tamanho == 0 && l.inicio == NULL);
}

static void teste_fila(void) {
    Fila f;
    int v;
    fila_inicializar(&f);

    confere("fila: comeca vazia", fila_vazia(&f));
    confere("fila: desenfileirar vazia falha", fila_desenfileirar(&f, &v) == 0);
    fila_enfileirar(&f, 1);
    fila_enfileirar(&f, 2);
    fila_enfileirar(&f, 3);
    confere("fila: sai o 1 primeiro", fila_desenfileirar(&f, &v) && v == 1);
    confere("fila: sai o 2", fila_desenfileirar(&f, &v) && v == 2);
    fila_enfileirar(&f, 4);
    confere("fila: sai o 3", fila_desenfileirar(&f, &v) && v == 3);
    confere("fila: sai o 4", fila_desenfileirar(&f, &v) && v == 4);
    confere("fila: vazia no fim", fila_vazia(&f) && f.fim == NULL);
    fila_enfileirar(&f, 9);
    fila_liberar(&f);
    confere("fila: vazia apos liberar", fila_vazia(&f));
}

static void teste_pilha(void) {
    Pilha p;
    Acao a;
    pilha_inicializar(&p);

    confere("pilha: comeca vazia", pilha_vazia(&p));
    confere("pilha: desempilhar vazia falha", pilha_desempilhar(&p, &a) == 0);
    pilha_empilhar(&p, ACAO_CADASTRO, 10, 0);
    pilha_empilhar(&p, ACAO_CADASTRO, 25, 0);
    pilha_empilhar(&p, ACAO_ADD_CONEXAO, 10, 25);
    printf("  historico (mais recente primeiro):\n");
    pilha_listar_ultimas(&p, 5);
    confere("pilha: listar nao remove", p.tamanho == 3);
    confere("pilha: topo e a conexao",
            pilha_desempilhar(&p, &a) && a.tipo == ACAO_ADD_CONEXAO
            && a.id1 == 10 && a.id2 == 25);
    pilha_liberar(&p);
    confere("pilha: vazia apos liberar", pilha_vazia(&p));
}

int main(void) {
    teste_lista();
    teste_fila();
    teste_pilha();

    if (falhas == 0) {
        printf("\nTodos os testes passaram.\n");
        return 0;
    }
    printf("\n%d teste(s) falharam.\n", falhas);
    return 1;
}
