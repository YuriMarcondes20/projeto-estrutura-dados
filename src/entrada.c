#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "registro.h"
#include "entrada.h"

#define LINHA_MAX 256   /* tamanho do buffer de leitura */

/* Le UMA linha com fgets, sem o '\n'. Devolve 0 em EOF.
   Se a linha for maior que o buffer, descarta o RESTANTE dela (para nao
   contaminar a proxima leitura) e marca *longa = 1. */
static int ler_linha(char *buf, size_t tam, int *longa) {
    size_t len;

    *longa = 0;
    if (fgets(buf, (int)tam, stdin) == NULL) {
        return 0;
    }
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[--len] = '\0';
    } else if (len == tam - 1) {          /* buffer cheio e sem '\n' */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
            /* descarta o resto da linha */
        }
        *longa = 1;
    }
    if (len > 0 && buf[len - 1] == '\r') {   /* arquivos com fim de linha do Windows */
        buf[--len] = '\0';
    }
    return 1;
}

static int so_espacos(const char *s) {
    while (*s != '\0') {
        if (!isspace((unsigned char)*s)) {
            return 0;
        }
        s++;
    }
    return 1;
}

int entrada_ler_texto(const char *pergunta, char *dest) {
    char buf[LINHA_MAX];
    int longa;

    for (;;) {
        printf("%s", pergunta);
        if (!ler_linha(buf, sizeof(buf), &longa)) {
            return 0;
        }
        if (longa || strlen(buf) > TAM_TEXTO - 1) {
            printf("Erro: o texto deve ter no maximo %d caracteres.\n",
                   TAM_TEXTO - 1);
            continue;
        }
        if (so_espacos(buf)) {
            printf("Erro: o campo nao pode ser vazio nem so espacos.\n");
            continue;
        }
        strcpy(dest, buf);
        return 1;
    }
}

int entrada_ler_inteiro(const char *pergunta, int min, int max, int *saida) {
    char buf[LINHA_MAX];
    char *fim;
    long v;
    int longa;

    for (;;) {
        printf("%s", pergunta);
        if (!ler_linha(buf, sizeof(buf), &longa)) {
            return 0;
        }
        if (longa) {
            printf("Erro: entrada muito longa.\n");
            continue;
        }
        if (so_espacos(buf)) {
            printf("Erro: digite um numero.\n");
            continue;
        }
        errno = 0;
        v = strtol(buf, &fim, 10);
        if (fim == buf) {                         /* nenhum digito lido */
            printf("Erro: valor invalido, digite apenas numeros.\n");
            continue;
        }
        while (isspace((unsigned char)*fim)) {    /* espacos no fim sao ok */
            fim++;
        }
        if (*fim != '\0') {                       /* sobrou lixo: "12abc" */
            printf("Erro: valor invalido, digite apenas numeros.\n");
            continue;
        }
        if (errno == ERANGE || v > INT_MAX || v < INT_MIN) {
            printf("Erro: numero grande demais.\n");
            continue;
        }
        if (v < min || v > max) {
            printf("Erro: valor fora do permitido (de %d a %d).\n", min, max);
            continue;
        }
        *saida = (int)v;
        return 1;
    }
}
