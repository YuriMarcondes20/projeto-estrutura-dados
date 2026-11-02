#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "registro.h"

/* Ordena o vetor por NOME (strcmp), desempatando pelo ID.
   Algoritmo: Insertion Sort. Nao usa qsort. */
void ordenar_por_nome(Registro *vetor, int n);

#endif
