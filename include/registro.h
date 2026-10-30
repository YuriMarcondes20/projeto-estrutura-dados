#ifndef REGISTRO_H
#define REGISTRO_H

#define TAM_TEXTO 80   /* 79 caracteres uteis + o '\0' */

/* Tema A - Redes: equipamentos e conexoes */
typedef struct {
    int  id;                       /* positivo e unico */
    char nome[TAM_TEXTO];
    char tipo[TAM_TEXTO];          /* tipo de equipamento */
    char localizacao[TAM_TEXTO];
} Registro;

#endif
