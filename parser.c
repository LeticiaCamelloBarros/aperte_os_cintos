
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
};
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

/* Le e valida UMA linha de tarefa, preenchendo *t em caso de sucesso.
 * 'numero_linha' e' so para mensagens de erro mais uteis.
 * Retorna 1 em sucesso, 0 em erro (ja imprime a mensagem), -1 se a linha
 * deve ser ignorada (linha em branco). */
static int ler_linha_tarefa(const char *linha, int numero_linha, Task *t) {
    if (linha_em_branco(linha)) return -1;
 
    char nome[20];
    int periodo, deadline, burst;
    char extra[8];
 
    int lidos = sscanf(linha, "%19s %d %d %d %7s",
                        nome, &periodo, &deadline, &burst, extra);
 
    if (lidos != 4) {
        fprintf(stderr,
            "ERRO: linha %d malformada (esperado NOME PERIODO DEADLINE BURST)\n",
            numero_linha);
        return 0;
    }
 
    if (periodo <= 0 || deadline <= 0 || burst <= 0) {
        fprintf(stderr,
            "ERRO: linha %d tem valor nao positivo (P=%d D=%d C=%d)\n",
            numero_linha, periodo, deadline, burst);
        return 0;
    }
 
    if (deadline > periodo || burst > deadline) {
        fprintf(stderr,
            "ERRO: linha %d viola C <= D <= P (tarefa '%s': P=%d D=%d C=%d)\n",
            numero_linha, nome, periodo, deadline, burst);
        return 0;
    }
 
    strcpy(t->nome, nome);
    t->periodo    = periodo;
    t->deadline   = deadline;
    t->burst      = burst;
 
    /* dados dinamicos: toda tarefa "chega" pela 1a vez no instante 0.
     * id_entrada e' preenchido por quem chama, pois depende de 'n'. */
    t->tempo_restante    = 0;
    t->proxima_chegada   = 0;
    t->deadline_absoluto = 0;
 
    return 1;
}

int carregar_tarefas(const char *caminho, TaskList **cadastradas_out, int *tempo_total_out) {
    FILE *f = fopen(caminho, "r");
    if (f == NULL) {
        fprintf(stderr, "ERRO: nao foi possivel abrir o arquivo '%s'\n", caminho);
        return -1;
    }
 
    int tempo_total;
    if (!ler_tempo_total(f, &tempo_total)) {
        fclose(f);
        return -1;
    }
 
    TaskList *cadastradas = NULL;
    int n = 0;
    int numero_linha = 1; /* linha 1 ja foi lida (tempo total) */
    char linha[256];
 
    while (fgets(linha, sizeof(linha), f) != NULL) {
        numero_linha++;
 
        Task t;
        int resultado = ler_linha_tarefa(linha, numero_linha, &t);
 
        if (resultado == -1) continue; /* linha em branco, ignora */
 
        if (resultado == 0) {
            liberar_cadastradas(cadastradas);
            fclose(f);
            return -1;
        }
 
        t.id_entrada = n; /* ordem de aparicao no arquivo = criterio de desempate */
        cadastradas = inserir_tarefa(cadastradas, t);
        n++;
    }
 
    fclose(f);
 
    if (n == 0) {
        fprintf(stderr, "ERRO: nenhuma tarefa encontrada no arquivo\n");
        return -1; /* cadastradas ja esta NULL, nada a liberar */
    }
 
    *cadastradas_out = cadastradas;
    *tempo_total_out = tempo_total;
    return n;
}