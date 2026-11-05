#ifndef SISTEMA_H
#define SISTEMA_H

#include "registro.h"
#include "bst.h"
#include "grafo.h"
#include "pilha.h"

/* Sistema integrado: BST (registros) + Grafo (conexoes) + Pilha (historico).
   Os tres se relacionam apenas pelo ID.

   ESTRATEGIA DE CONSISTENCIA / ROLLBACK
   - Cadastro: 1) insere na BST; 2) cria o vertice; 3) empilha no historico.
     Se a etapa 2 ou 3 falhar, desfaz o que ja foi feito (remove da BST e,
     se for o caso, o vertice). Nunca fica registro sem vertice.
   - Remocao: so ocorre se o ID existe. Remove o vertice (e suas arestas) e
     depois o no da BST.
   - Conexoes: se nao for possivel registrar no historico, a aresta e
     desfeita.
   O historico recebe somente operacoes concluidas com sucesso. */

typedef struct {
    BST   bst;
    Grafo grafo;
    Pilha historico;
    int   simular_falha_vertice;   /* 1 = simula falha na criacao do vertice (testes) */
} Sistema;

/* Codigos de retorno */
#define SIS_OK                 1
#define SIS_HISTORICO_FALHOU   2    /* operacao feita, mas sem registro no historico */
#define SIS_ID_DUPLICADO      -1
#define SIS_CAPACIDADE_CHEIA  -2
#define SIS_SEM_MEMORIA       -3
#define SIS_ID_INEXISTENTE    -4
#define SIS_LACO              -5
#define SIS_ARESTA_DUPLICADA  -6
#define SIS_ARESTA_INEXISTENTE -7
#define SIS_INCONSISTENTE     -8

void sistema_inicializar(Sistema *s);
void sistema_liberar(Sistema *s);

int  sistema_cadastrar(Sistema *s, const Registro *r);
int  sistema_remover(Sistema *s, int id);
int  sistema_adicionar_conexao(Sistema *s, int a, int b);
int  sistema_remover_conexao(Sistema *s, int a, int b);

#endif
