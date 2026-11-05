#include <stdio.h>
#include <string.h>
#include "sistema.h"

static int falhas = 0;

static void confere(const char *nome, int condicao) {
    printf("[%s] %s\n", condicao ? "OK" : "FALHOU", nome);
    if (!condicao) {
        falhas++;
    }
}

static Registro reg(int id) {
    Registro r;
    r.id = id;
    sprintf(r.nome, "Equip %d", id);
    strcpy(r.tipo, "Switch");
    strcpy(r.localizacao, "Sala");
    return r;
}

int main(void) {
    static Sistema s;
    Registro r;
    Acao a;
    int i, hist;

    sistema_inicializar(&s);

    /* ---- Cadastro cria vertice ---- */
    r = reg(10); confere("cadastra 10", sistema_cadastrar(&s, &r) == SIS_OK);
    r = reg(25); sistema_cadastrar(&s, &r);
    r = reg(40); sistema_cadastrar(&s, &r);
    r = reg(60); sistema_cadastrar(&s, &r);
    confere("cadastro cria o vertice", grafo_existe_vertice(&s.grafo, 10));
    confere("BST e grafo com 4 itens", s.bst.tamanho == 4 && s.grafo.qtd == 4);
    confere("historico tem 4 cadastros", s.historico.tamanho == 4);

    r = reg(25);
    confere("ID duplicado rejeitado", sistema_cadastrar(&s, &r) == SIS_ID_DUPLICADO);
    confere("duplicado nao altera nada",
            s.bst.tamanho == 4 && s.grafo.qtd == 4 && s.historico.tamanho == 4);

    /* ---- Conexoes e historico so de sucessos ---- */
    confere("conecta 10-25", sistema_adicionar_conexao(&s, 10, 25) == SIS_OK);
    confere("conecta 10-40", sistema_adicionar_conexao(&s, 10, 40) == SIS_OK);
    confere("conecta 25-60", sistema_adicionar_conexao(&s, 25, 60) == SIS_OK);
    hist = s.historico.tamanho;
    confere("duplicata (25,10) rejeitada",
            sistema_adicionar_conexao(&s, 25, 10) == SIS_ARESTA_DUPLICADA);
    confere("laco rejeitado", sistema_adicionar_conexao(&s, 10, 10) == SIS_LACO);
    confere("ID inexistente rejeitado",
            sistema_adicionar_conexao(&s, 10, 999) == SIS_ID_INEXISTENTE);
    confere("remover conexao inexistente rejeitado",
            sistema_remover_conexao(&s, 40, 60) == SIS_ARESTA_INEXISTENTE);
    confere("falhas nao entram no historico", s.historico.tamanho == hist);

    pilha_peek(&s.historico, &a);
    confere("peek: ultima acao e conexao 25-60",
            a.tipo == ACAO_ADD_CONEXAO && a.id1 == 25 && a.id2 == 60);
    printf("  historico (ultimas 5):\n");
    pilha_listar_ultimas(&s.historico, 5);
    confere("consulta nao remove acoes", s.historico.tamanho == hist);

    /* ---- Remocao de vertice com varias arestas ---- */
    confere("remove 10 (tem 2 arestas)", sistema_remover(&s, 10) == SIS_OK);
    confere("10 saiu da BST", bst_buscar(&s.bst, 10) == NULL);
    confere("10 saiu do grafo", !grafo_existe_vertice(&s.grafo, 10));
    confere("arestas do 10 sumiram",
            !grafo_existe_aresta(&s.grafo, 10, 25)
            && !grafo_existe_aresta(&s.grafo, 10, 40));
    confere("aresta 25-60 continua", grafo_existe_aresta(&s.grafo, 25, 60));
    hist = s.historico.tamanho;
    confere("remover ID inexistente rejeitado",
            sistema_remover(&s, 10) == SIS_ID_INEXISTENTE);
    confere("remocao rejeitada nao altera nada",
            s.bst.tamanho == 3 && s.grafo.qtd == 3 && s.historico.tamanho == hist);
    confere("remove conexao 25-60", sistema_remover_conexao(&s, 25, 60) == SIS_OK);

    /* ---- Simulacao de falha na criacao do vertice: rollback ---- */
    hist = s.historico.tamanho;
    s.simular_falha_vertice = 1;
    r = reg(77);
    confere("falha simulada devolve erro",
            sistema_cadastrar(&s, &r) == SIS_SEM_MEMORIA);
    s.simular_falha_vertice = 0;
    confere("rollback: 77 nao ficou na BST", bst_buscar(&s.bst, 77) == NULL);
    confere("rollback: 77 nao ficou no grafo", !grafo_existe_vertice(&s.grafo, 77));
    confere("rollback: historico intacto", s.historico.tamanho == hist);
    confere("rollback: tamanhos consistentes", s.bst.tamanho == s.grafo.qtd);

    /* ---- Capacidade de 100: o 101o e rejeitado com rollback ---- */
    sistema_liberar(&s);
    sistema_inicializar(&s);
    for (i = 1; i <= MAX_VERTICES; i++) {
        r = reg(i * 10);
        sistema_cadastrar(&s, &r);
    }
    confere("100 cadastros aceitos", s.bst.tamanho == 100 && s.grafo.qtd == 100);
    hist = s.historico.tamanho;
    r = reg(5);
    confere("101o cadastro rejeitado",
            sistema_cadastrar(&s, &r) == SIS_CAPACIDADE_CHEIA);
    confere("rollback: nada do 101o ficou na BST", bst_buscar(&s.bst, 5) == NULL);
    confere("BST segue com 100 e historico intacto",
            s.bst.tamanho == 100 && s.historico.tamanho == hist);
    printf("  historico com mais de 5 acoes (mostra so as 5 ultimas):\n");
    pilha_listar_ultimas(&s.historico, 5);
    confere("historico completo preservado", s.historico.tamanho == 100);

    sistema_liberar(&s);
    confere("tudo liberado", s.bst.raiz == NULL && s.historico.topo == NULL);

    if (falhas == 0) {
        printf("\nTodos os testes passaram.\n");
        return 0;
    }
    printf("\n%d teste(s) falharam.\n", falhas);
    return 1;
}
