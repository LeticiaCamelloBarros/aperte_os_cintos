#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include "utils.h"

/* Logger: acumula, instante a instante, quem esta executando, e decide
 * quando um "trecho" de execucao termina (troca de tarefa, conclusao,
 * ou perda de deadline), escrevendo a linha correspondente no arquivo
 * assim que o trecho fecha. */
typedef struct Logger Logger;

/* Cria um logger que escreve no arquivo ja aberto 'saida'. */
Logger *logger_criar(FILE *saida);

/* Chamar UMA VEZ por unidade de tempo, depois de simular_instante.
 *
 * executada_id  - id_entrada de quem executou neste t, ou -1 se idle
 *                 (e' o retorno de simular_instante)
 * completou_id  - id_entrada de quem TERMINOU sua instancia neste t
 *                 (tempo_restante chegou a 0), ou -1 se ninguem terminou
 * perdeu_id     - id_entrada de quem PERDEU o deadline neste t, ou -1
 *                 se ninguem perdeu
 *
 * completou_id e perdeu_id sao descobertos comparando completas[]/
 * lost_deadlines[] antes e depois da chamada de simular_instante (ver
 * exemplo de uso no main.c). */
void logger_processar_tick(Logger *log, TaskList *cadastradas,
                            int executada_id, int completou_id, int perdeu_id);

/* Fecha o ultimo trecho pendente ao final da simulacao (chamado 1x, apos
 * o laco principal). 'killed' e' um vetor (tamanho n) com 1 para quem
 * ainda tinha tempo_restante > 0 quando a simulacao acabou. */
void logger_finalizar(Logger *log, TaskList *cadastradas, int killed[]);

/* Libera a memoria do logger (nao fecha o FILE*, isso e' responsabilidade
 * de quem abriu). */
void logger_destruir(Logger *log);

/* Escreve a primeira linha do arquivo: "EXECUTION BY RATE" ou "EXECUTION BY EDF" */
void escrever_cabecalho(FILE *saida, const char *algoritmo);

/* Escreve as tres secoes finais: LOST DEADLINES, COMPLETE EXECUTION, KILLED.
 * Percorre 'cadastradas' (ja esta na ordem do arquivo de entrada) e usa
 * o id_entrada de cada uma para indexar os vetores de estatisticas. */
void escrever_estatisticas(FILE *saida, TaskList *cadastradas,
                            int lost_deadlines[], int completas[], int killed[]);

#endif