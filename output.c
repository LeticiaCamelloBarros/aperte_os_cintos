#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "output.h"
 
/* Estado interno do logger: qual "trecho" esta aberto agora.
 *   task_atual == -2  -> nenhum trecho aberto ainda (inicio da simulacao)
 *   task_atual == -1  -> trecho aberto e' de CPU ociosa (idle)
 *   task_atual >= 0   -> trecho aberto e' de execucao da tarefa com esse id_entrada
 */
struct Logger {
    FILE *saida;
    int task_atual;
    int unidades;
};
 
Logger *logger_criar(FILE *saida) {
    Logger *log = malloc(sizeof(Logger));
    if (log == NULL) {
        fprintf(stderr, "ERRO: falha ao alocar memoria (logger)\n");
        exit(EXIT_FAILURE);
    }
    log->saida = saida;
    log->task_atual = -2; /* nada aberto ainda */
    log->unidades = 0;
    return log;
}
 
/* Acha o no da lista de cadastradas cujo id_entrada bate com 'id'. */
static TaskList *buscar_por_id(TaskList *cadastradas, int id) {
    for (TaskList *no = cadastradas; no != NULL; no = no->next) {
        if (no->tarefa.id_entrada == id) return no;
    }
    return NULL; /* nao deveria acontecer se id veio de uma chamada valida */
}

/* Fecha o trecho atualmente aberto, escrevendo a linha no arquivo,
 * e reseta o estado do logger para "nada aberto". Se nao havia nada
 * aberto, nao faz nada (seguro chamar mesmo sem trecho pendente). */
static void fechar_segmento(Logger *log, TaskList *cadastradas, char codigo) {
    if (log->task_atual == -2) return; /* nada para fechar */
 
    if (log->task_atual == -1) {
        fprintf(log->saida, "idle for %d units\n", log->unidades);
    } else {
        TaskList *no = buscar_por_id(cadastradas, log->task_atual);
        fprintf(log->saida, "[%s] for %d units - %c\n",
                no->tarefa.nome, log->unidades, codigo);
    }
 
    log->task_atual = -2;
    log->unidades = 0;
}