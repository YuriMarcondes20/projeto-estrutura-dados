#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "registro.h"
#include "sistema.h"
#include "entrada.h"
#include "ordenacao.h"
#include "lista.h"

static void mostrar_menu(void) {
    printf("\n============================================\n");
    printf("      SISTEMA DE ESTRUTURA DE DADOS\n");
    printf("============================================\n");
    printf(" 1 - Cadastrar equipamento (cria vertice)\n");
    printf(" 2 - Buscar equipamento (BST)\n");
    printf(" 3 - Remover equipamento e vertice/arestas\n");
    printf(" 4 - Listar equipamentos (BST em-ordem)\n");
    printf(" 5 - Ordenar e exibir equipamentos por nome\n");
    printf(" 6 - Adicionar conexao (aresta)\n");
    printf(" 7 - Remover conexao (aresta)\n");
    printf(" 8 - Consultar conexao direta\n");
    printf(" 9 - Listar alcancaveis (BFS + lista)\n");
    printf("10 - Verificar caminho entre dois IDs\n");
    printf("11 - Consultar historico (pilha)\n");
    printf(" 0 - Encerrar\n");
    printf("============================================\n");
}

static void imprimir_registro(const Registro *r) {
    printf("ID %d | Nome: %s | Tipo: %s | Local: %s\n",
           r->id, r->nome, r->tipo, r->localizacao);
}

/* Traduz um codigo de retorno do sistema em mensagem para o usuario. */
static void mostrar_erro(int codigo) {
    switch (codigo) {
        case SIS_ID_DUPLICADO:
            printf("Erro: ja existe um equipamento com esse ID.\n"); break;
        case SIS_CAPACIDADE_CHEIA:
            printf("Erro: capacidade maxima de %d equipamentos atingida. "
                   "Cadastro desfeito.\n", MAX_VERTICES); break;
        case SIS_SEM_MEMORIA:
            printf("Erro: memoria insuficiente. Operacao desfeita.\n"); break;
        case SIS_ID_INEXISTENTE:
            printf("Erro: ID inexistente. Nada foi alterado.\n"); break;
        case SIS_LACO:
            printf("Erro: nao e permitido ligar um equipamento a ele mesmo.\n"); break;
        case SIS_ARESTA_DUPLICADA:
            printf("Erro: essa conexao ja existe.\n"); break;
        case SIS_ARESTA_INEXISTENTE:
            printf("Erro: essa conexao nao existe.\n"); break;
        default:
            printf("Erro: inconsistencia interna.\n"); break;
    }
}

static int ler_id(const char *pergunta, int *id) {
    return entrada_ler_inteiro(pergunta, 1, INT_MAX, id);
}

/* 1 - Cadastrar */
static int op_cadastrar(Sistema *s) {
    Registro r;
    int codigo;

    if (!ler_id("ID (inteiro positivo): ", &r.id)) return 0;
    if (!entrada_ler_texto("Nome: ", r.nome)) return 0;
    if (!entrada_ler_texto("Tipo de equipamento: ", r.tipo)) return 0;
    if (!entrada_ler_texto("Localizacao: ", r.localizacao)) return 0;

    codigo = sistema_cadastrar(s, &r);
    if (codigo == SIS_OK) {
        printf("Equipamento %d cadastrado e vertice criado.\n", r.id);
    } else {
        mostrar_erro(codigo);
    }
    return 1;
}

/* 2 - Buscar */
static int op_buscar(Sistema *s) {
    int id;
    const Registro *r;

    if (!ler_id("ID a buscar: ", &id)) return 0;
    r = bst_buscar(&s->bst, id);
    if (r == NULL) {
        printf("Equipamento %d nao encontrado.\n", id);
    } else {
        imprimir_registro(r);
    }
    return 1;
}

/* 3 - Remover */
static int op_remover(Sistema *s) {
    int id, codigo;

    if (!ler_id("ID a remover: ", &id)) return 0;
    codigo = sistema_remover(s, id);
    if (codigo == SIS_OK) {
        printf("Equipamento %d removido (vertice e arestas tambem).\n", id);
    } else if (codigo == SIS_HISTORICO_FALHOU) {
        printf("Equipamento %d removido, mas o historico nao foi atualizado.\n", id);
    } else {
        mostrar_erro(codigo);
    }
    return 1;
}

/* 4 - Listar */
static void op_listar(Sistema *s) {
    const Registro *menor = bst_menor(&s->bst);
    const Registro *maior = bst_maior(&s->bst);

    bst_listar_em_ordem(&s->bst);
    if (menor != NULL && maior != NULL) {
        printf("Total: %d | Menor ID: %d | Maior ID: %d\n",
               s->bst.tamanho, menor->id, maior->id);
    }
}

/* 5 - Ordenar (copia para vetor auxiliar; a BST nao e alterada) */
static void op_ordenar(Sistema *s) {
    Registro *vetor;
    int n, i;

    if (s->bst.tamanho == 0) {
        printf("Nenhum equipamento cadastrado.\n");
        return;
    }
    vetor = (Registro *)malloc((size_t)s->bst.tamanho * sizeof(Registro));
    if (vetor == NULL) {
        printf("Erro: memoria insuficiente.\n");
        return;
    }
    n = bst_copiar_para_vetor(&s->bst, vetor);
    ordenar_por_nome(vetor, n);
    printf("Equipamentos ordenados por nome:\n");
    for (i = 0; i < n; i++) {
        imprimir_registro(&vetor[i]);
    }
    free(vetor);
}

/* 6 e 7 - Adicionar / remover conexao */
static int op_conexao(Sistema *s, int adicionar) {
    int a, b, codigo;

    if (!ler_id("ID do primeiro equipamento: ", &a)) return 0;
    if (!ler_id("ID do segundo equipamento: ", &b)) return 0;
    if (adicionar) {
        codigo = sistema_adicionar_conexao(s, a, b);
        if (codigo == SIS_OK) printf("Conexao %d - %d adicionada.\n", a, b);
        else mostrar_erro(codigo);
    } else {
        codigo = sistema_remover_conexao(s, a, b);
        if (codigo == SIS_OK) printf("Conexao %d - %d removida.\n", a, b);
        else mostrar_erro(codigo);
    }
    return 1;
}

/* 8 - Conexao direta */
static int op_conexao_direta(Sistema *s) {
    int a, b;

    if (!ler_id("ID do primeiro equipamento: ", &a)) return 0;
    if (!ler_id("ID do segundo equipamento: ", &b)) return 0;
    if (!grafo_existe_vertice(&s->grafo, a) || !grafo_existe_vertice(&s->grafo, b)) {
        mostrar_erro(SIS_ID_INEXISTENTE);
    } else if (grafo_existe_aresta(&s->grafo, a, b)) {
        printf("Existe conexao direta entre %d e %d.\n", a, b);
    } else {
        printf("NAO existe conexao direta entre %d e %d.\n", a, b);
    }
    return 1;
}

/* 9 - Alcancaveis (BFS) */
static int op_bfs(Sistema *s) {
    int origem, r;
    Lista l;

    if (!ler_id("ID de origem: ", &origem)) return 0;
    lista_inicializar(&l);
    r = grafo_bfs(&s->grafo, origem, &l);
    if (r == GRAFO_OK) {
        printf("Alcancaveis a partir de %d (%d): ", origem, l.tamanho);
        lista_imprimir(&l);
    } else if (r == GRAFO_VERTICE_INEXISTENTE) {
        mostrar_erro(SIS_ID_INEXISTENTE);
    } else {
        mostrar_erro(SIS_SEM_MEMORIA);
    }
    lista_liberar(&l);
    return 1;
}

/* 10 - Existe caminho? */
static int op_caminho(Sistema *s) {
    int a, b, r;

    if (!ler_id("ID de origem: ", &a)) return 0;
    if (!ler_id("ID de destino: ", &b)) return 0;
    r = grafo_existe_caminho(&s->grafo, a, b);
    if (r == 1) {
        printf("Existe caminho entre %d e %d.\n", a, b);
    } else if (r == 0 && grafo_existe_vertice(&s->grafo, a)
               && grafo_existe_vertice(&s->grafo, b)) {
        printf("NAO existe caminho entre %d e %d.\n", a, b);
    } else if (r == GRAFO_VERTICE_INEXISTENTE) {
        mostrar_erro(SIS_ID_INEXISTENTE);
    } else {
        mostrar_erro(SIS_SEM_MEMORIA);
    }
    return 1;
}

/* 11 - Historico (peek + ultimas 5, sem remover nada) */
static void op_historico(Sistema *s) {
    Acao topo;

    if (!pilha_peek(&s->historico, &topo)) {
        printf("Historico vazio.\n");
        return;
    }
    printf("Ultimas acoes (da mais recente para a mais antiga):\n");
    pilha_listar_ultimas(&s->historico, 5);
}

int main(void) {
    Sistema s;
    int opcao;

    sistema_inicializar(&s);

    for (;;) {
        mostrar_menu();
        if (!entrada_ler_inteiro("Escolha uma opcao: ", 0, 11, &opcao)) {
            break;                         /* fim da entrada (EOF) */
        }
        if (opcao == 0) {
            break;
        }
        switch (opcao) {
            case 1:  if (!op_cadastrar(&s)) goto fim; break;
            case 2:  if (!op_buscar(&s)) goto fim; break;
            case 3:  if (!op_remover(&s)) goto fim; break;
            case 4:  op_listar(&s); break;
            case 5:  op_ordenar(&s); break;
            case 6:  if (!op_conexao(&s, 1)) goto fim; break;
            case 7:  if (!op_conexao(&s, 0)) goto fim; break;
            case 8:  if (!op_conexao_direta(&s)) goto fim; break;
            case 9:  if (!op_bfs(&s)) goto fim; break;
            case 10: if (!op_caminho(&s)) goto fim; break;
            case 11: op_historico(&s); break;
        }
    }
fim:
    sistema_liberar(&s);
    printf("\nMemoria liberada. Ate logo!\n");
    return 0;
}
