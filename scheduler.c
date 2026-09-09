#include <stdio.h>
#include <string.h>
#include "scheduler.h"
 
/* ---------------------------------------------------------------------
 * Rate Monotonic: prioridade fixa, definida pelo periodo.
 * Quanto MENOR o periodo, MAIOR a prioridade.
 * --------------------------------------------------------------------- */
int prioridade_rate(TaskList *a, TaskList *b) {
    if (a->tarefa.periodo < b->tarefa.periodo)
        return 1;
    if (a->tarefa.periodo == b->tarefa.periodo)
        return a->tarefa.id_entrada < b->tarefa.id_entrada; /* desempate */
    return 0;
}
 /* EDF: prioridade dinamica, definida pelo deadline absoluto da instancia atual.
 * Quanto MENOR o deadline absoluto, MAIOR a prioridade. */
int prioridade_edf(TaskList *a, TaskList *b) {
    if (a->tarefa.deadline_absoluto < b->tarefa.deadline_absoluto)
        return 1;
    if (a->tarefa.deadline_absoluto == b->tarefa.deadline_absoluto)
        return a->tarefa.id_entrada < b->tarefa.id_entrada; /* desempate */
    return 0;
}
 
/* ---------------------------------------------------------------------
 * Resolve a string do argv[1] para o ponteiro de funcao certo.
 * So faz strcmp aqui, uma vez -- nao dentro do laco de simulacao.
 * --------------------------------------------------------------------- */

/* ---------------------------------------------------------------------
 * Resolve a string do argv[1] para o ponteiro de funcao certo.
 * So faz strcmp aqui, uma vez -- nao dentro do laco de simulacao.
 * --------------------------------------------------------------------- */
comparar_prioridade escolher_algoritmo(const char *nome) {
    if (strcmp(nome, "rate") == 0) return prioridade_rate;
    if (strcmp(nome, "edf")  == 0) return prioridade_edf;
    return NULL; /* algoritmo invalido -- quem chamou deve checar isso */
}
int simular_instante(TaskList *cadastradas, ready **fila_prontos, int t,
                      comparar_prioridade comparar, int lost_deadlines[], int completas[]) {
 
    /* 1) Chegadas: percorre todas as cadastradas, insere na fila de prontos
     *    quem chegou agora */
    for (TaskList *no = cadastradas; no != NULL; no = no->next) {
        if (t == no->tarefa.proxima_chegada) {
            no->tarefa.tempo_restante    = no->tarefa.burst;
            no->tarefa.deadline_absoluto = t + no->tarefa.deadline;
            no->tarefa.proxima_chegada  += no->tarefa.periodo;
 
            *fila_prontos = inserir_pronta(*fila_prontos, no);
        }
    }
 
    /* 2) Deadlines perdidos: percorre so quem esta pronto */
    ready *atual = *fila_prontos;
    while (atual != NULL) {
        ready *proximo = atual->next; /* guarda antes, pois o no pode ser removido */
        TaskList *t_no = atual->pcb;
 
        if (t_no->tarefa.tempo_restante > 0 && t == t_no->tarefa.deadline_absoluto) {
            lost_deadlines[t_no->tarefa.id_entrada]++;
            t_no->tarefa.tempo_restante = 0;
            *fila_prontos = remover_pronta(*fila_prontos, t_no);
        }
        atual = proximo;
    }
 
    /* 3) Escolher a tarefa pronta de maior prioridade, usando o comparador
     *    ja resolvido -- nenhuma comparacao de string aqui */
    TaskList *escolhida = NULL;
    for (ready *no = *fila_prontos; no != NULL; no = no->next) {
        TaskList *cand = no->pcb;
        if (escolhida == NULL || comparar(cand, escolhida)) {
            escolhida = cand;
        }
    }
 
    /* 4) Executar 1 unidade */
    int indice_executada = -1;
    if (escolhida != NULL) {
        escolhida->tarefa.tempo_restante--;
        indice_executada = escolhida->tarefa.id_entrada;
 
        if (escolhida->tarefa.tempo_restante == 0) {
            completas[indice_executada]++;
            *fila_prontos = remover_pronta(*fila_prontos, escolhida);
        }
    }
 
    /* 5) Log */
    if (escolhida != NULL) {
        fprintf(stderr, "[t=%d] %s executando (restante=%d)\n",
                t, escolhida->tarefa.nome, escolhida->tarefa.tempo_restante);
    } else {
        fprintf(stderr, "[t=%d] idle\n", t);
    }
 
    return indice_executada;
}
 