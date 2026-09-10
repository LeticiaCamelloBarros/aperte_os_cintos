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

void logger_processar_tick(Logger *log, TaskList *cadastradas,
                            int executada_id, int completou_id, int perdeu_id) {
 
    /*  Se a tarefa que estava com o trecho aberto perdeu o deadline
     *    agora, fecha esse trecho com 'L'. (Isso acontece ANTES da troca
     *    de trecho abaixo, porque a perda de deadline tira a tarefa da
     *    disputa antes de qualquer nova escolha.) */
    if (perdeu_id != -1 && log->task_atual == perdeu_id) {
        fechar_segmento(log, cadastradas, 'L');
    }
 
    /*  Se quem executa agora e' diferente do trecho aberto, fecha o
     *    trecho anterior (se ainda estiver aberto -- pode ja ter sido
     *    fechado no passo 1) com 'H', e abre um trecho novo. */
    if (log->task_atual != executada_id) {
        if (log->task_atual != -2) {
            fechar_segmento(log, cadastradas, 'H');
        }
        log->task_atual = executada_id;
        log->unidades = 1;
    } else {
        log->unidades++;
    }
 
    /* Se a tarefa que executou agora terminou (chegou a burst 0),
     *    fecha o trecho imediatamente com 'F'. */
    if (completou_id != -1 && executada_id == completou_id) {
        fechar_segmento(log, cadastradas, 'F');
    }
}
void logger_finalizar(Logger *log, TaskList *cadastradas, int killed[]) {
    if (log->task_atual == -2) return; /* nada pendente */
 
    if (log->task_atual == -1) {
        fechar_segmento(log, cadastradas, ' '); /* idle nao usa codigo */
    } else {
        /* Se sobrou um trecho de execucao aberto ate' o fim da simulacao
         * sem ter sido fechado por F ou L, e' porque a tarefa ainda tinha
         * tempo_restante > 0 quando o tempo total acabou -- ou seja, foi
         * morta (killed). */
        char codigo = killed[log->task_atual] ? 'K' : 'F';
        fechar_segmento(log, cadastradas, codigo);
    }
}
 
void logger_destruir(Logger *log) {
    free(log);
}
 