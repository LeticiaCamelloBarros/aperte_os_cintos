#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
int prioridade_rate(TaskList *a, TaskList *b) ;
int prioridade_edf(TaskList *a, TaskList *b) ;

typedef struct  CompararPrioridade {
    TaskList *a ;
     TaskList *b;
}comparar_prioridade;
comparar_prioridade escolher_algoritmo(const char *nome) ;