#ifndef ENTRADA_H
#define ENTRADA_H

/* Leitura segura de entradas do teclado (ou de um arquivo redirecionado).
   Ambas as funcoes repetem o pedido ate receber um valor valido e
   devolvem 1. Devolvem 0 somente se a entrada acabar (EOF). */

/* Le um texto com espacos. Rejeita: vazio, so espacos e mais de 79
   caracteres (nesse caso descarta o resto da linha). "dest" deve ter
   espaco para 80 bytes (TAM_TEXTO). */
int entrada_ler_texto(const char *pergunta, char *dest);

/* Le um inteiro entre min e max (inclusive). Rejeita: letras, lixo apos
   o numero, linha vazia, overflow e valores fora do intervalo. */
int entrada_ler_inteiro(const char *pergunta, int min, int max, int *saida);

#endif
