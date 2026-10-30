#ifndef PILHA_H
#define PILHA_H

/* Pilha encadeada (LIFO: o ultimo a entrar e o primeiro a sair).
   Usada como historico das acoes do sistema. */

typedef enum {
    ACAO_CADASTRO = 1,
    ACAO_REMOCAO,
    ACAO_ADD_CONEXAO,
    ACAO_REM_CONEXAO
} TipoAcao;

typedef struct {
    TipoAcao tipo;
    int id1;
    int id2;          /* so usado nas acoes de conexao; senao fica 0 */
} Acao;

typedef struct NoPilha {
    Acao acao;
    struct NoPilha *prox;
} NoPilha;

typedef struct {
    NoPilha *topo;
    int tamanho;
} Pilha;

void pilha_inicializar(Pilha *p);
int  pilha_vazia(const Pilha *p);
int  pilha_empilhar(Pilha *p, TipoAcao tipo, int id1, int id2); /* 1 = ok */
int  pilha_desempilhar(Pilha *p, Acao *saida);                  /* 1 = ok */
void pilha_listar_ultimas(const Pilha *p, int n);  /* nao remove nada */
void pilha_liberar(Pilha *p);

#endif
