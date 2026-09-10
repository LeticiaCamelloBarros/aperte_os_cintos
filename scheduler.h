#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
int prioridade_rate(TaskList *a, TaskList *b) ;
int prioridade_edf(TaskList *a, TaskList *b) ;
//ponteiro para uma função 
typedef int (*comparar_prioridade)(TaskList *a, TaskList *b); // (B) tipo de PONTEIRO para função
comparar_prioridade escolher_algoritmo(const char *nome) ;
int simular_instante(TaskList *cadastradas, ready **fila_prontos, int t,
                      comparar_prioridade comparar, int lost_deadlines[], int completas[]); 
#endif 