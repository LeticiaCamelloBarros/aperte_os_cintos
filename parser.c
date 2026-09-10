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
 