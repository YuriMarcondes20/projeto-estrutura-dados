#include <string.h>
#include "ordenacao.h"

/* Retorna < 0 se a deve vir antes de b, > 0 se depois, 0 se iguais. */
static int comparar(const Registro *a, const Registro *b) {
    int c = strcmp(a->nome, b->nome);
    if (c != 0) {
        return c;
    }
    return (a->id > b->id) - (a->id < b->id);   /* desempate pelo ID */
}

/* Insertion Sort: pega cada elemento e o "encaixa" no lugar certo
   entre os anteriores, empurrando para a direita os que sao maiores.
   Custo: O(n^2) no pior caso, O(n) se ja estiver ordenado.
   E estavel e simples, bom para vetores pequenos (max. 100 registros). */
void ordenar_por_nome(Registro *vetor, int n) {
    int i, j;
    Registro chave;

    for (i = 1; i < n; i++) {
        chave = vetor[i];
        j = i - 1;
        while (j >= 0 && comparar(&vetor[j], &chave) > 0) {
            vetor[j + 1] = vetor[j];
            j--;
        }
        vetor[j + 1] = chave;
    }
}
