C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
 
/* Verifica se uma linha contem apenas espacos em branco (ou esta vazia).
 * Usado para pular linhas em branco no fim do arquivo, sem tratar isso
 * como erro de formatacao. */
static int linha_em_branco(const char *linha) {
    for (int i = 0; linha[i] != '\0'; i++) {
        if (!isspace((unsigned char)linha[i])) return 0;
    }
    return 1;
}
 /* Le e valida a primeira linha do arquivo (tempo total de simulacao).
 * Retorna 1 em sucesso (preenche *tempo_total), 0 em erro (ja imprime a mensagem). */
static int ler_tempo_total(FILE *f, int *tempo_total) {
    char linha[256];
 
    if (fgets(linha, sizeof(linha), f) == NULL) {
        fprintf(stderr, "ERRO: arquivo vazio ou ilegivel\n");
        return 0;
    }
 
    char lixo[8];
    int campos = sscanf(linha, "%d %7s", tempo_total, lixo);
    if (campos != 1 || *tempo_total <= 0) {
        fprintf(stderr, "ERRO: tempo total invalido na primeira linha\n");
        return 0;
    }
 
    return 1;
}