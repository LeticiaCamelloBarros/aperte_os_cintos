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
 