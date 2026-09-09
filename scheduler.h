#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
int prioridade_rate(TaskList *a, TaskList *b) ;
int prioridade_edf(TaskList *a, TaskList *b) ;
//ponteiro para uma função 
typedef int (*comparar_prioridade)(TaskList *a, TaskList *b); // (B) tipo de PONTEIRO para função
comparar_prioridade escolher_algoritmo(const char *nome) ;