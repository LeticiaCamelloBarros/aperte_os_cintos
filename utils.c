#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
int simular_instante(Task tarefas[], int n, int t, const char *algoritmo,
                      int lost_deadlines[], int completas[]) {
 
    /* 1) Verificar chegada de novas instâncias */
    for (int i = 0; i < n; i++) {
        if (t == tarefas[i].proxima_chegada) {
            tarefas[i].tempo_restante   = tarefas[i].burst;
            tarefas[i].deadline_absoluto = t + tarefas[i].deadline;
            tarefas[i].proxima_chegada  += tarefas[i].periodo;
        }
    }
 
    /* 2) Verificar deadlines perdidos (instância não terminou até o deadline absoluto) */
    for (int i = 0; i < n; i++) {
        if (tarefas[i].tempo_restante > 0 && t == tarefas[i].deadline_absoluto) {
            lost_deadlines[i]++;
            tarefas[i].tempo_restante = 0; /* descarta a rajada restante */
        }
    }
 
    /* 3) Escolher, entre as tarefas prontas, a de maior prioridade */
    int escolhida = -1;
    for (int i = 0; i < n; i++) {
        if (tarefas[i].tempo_restante <= 0) continue; /* não está pronta */
 
        if (escolhida == -1) {
            escolhida = i;
            continue;
        }
 
        int i_vence;
        if (strcmp(algoritmo, "rate") == 0) {
            /* Rate Monotonic: menor período = maior prioridade */
            if (tarefas[i].periodo < tarefas[escolhida].periodo)
                i_vence = 1;
            else if (tarefas[i].periodo == tarefas[escolhida].periodo)
                i_vence = tarefas[i].id_entrada < tarefas[escolhida].id_entrada;
            else
                i_vence = 0;
        } else {
            /* EDF: menor deadline absoluto = maior prioridade */
            if (tarefas[i].deadline_absoluto < tarefas[escolhida].deadline_absoluto)
                i_vence = 1;
            else if (tarefas[i].deadline_absoluto == tarefas[escolhida].deadline_absoluto)
                i_vence = tarefas[i].id_entrada < tarefas[escolhida].id_entrada;
            else
                i_vence = 0;
        }
 
        if (i_vence) escolhida = i;
    }
 
    /* 4) Executar 1 unidade de tempo da tarefa escolhida */
    if (escolhida != -1) {
        tarefas[escolhida].tempo_restante--;
        if (tarefas[escolhida].tempo_restante == 0) {
            completas[escolhida]++;
        }
    }
 
    /* 5) Registrar no log quem executou (ou idle) nesse instante */
    if (escolhida != -1) {
        fprintf(stderr, "[t=%d] %s executando (restante=%d)\n",
                t, tarefas[escolhida].nome, tarefas[escolhida].tempo_restante);
    } else {
        fprintf(stderr, "[t=%d] idle\n", t);
    }
 
    return escolhida;
}
 