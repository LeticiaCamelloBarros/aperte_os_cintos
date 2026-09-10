#ifndef UTILS_H
#define UTILS_H
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct {
    char nome[20];
    int periodo;
    int deadline;
    int burst;
    int id_entrada;

    int tempo_restante;
    int proxima_chegada;
    int deadline_absoluto;
} Task;


typedef struct task_list {
    Task tarefa;
    struct task_list *next;
} TaskList;
typedef struct ready_queue {
TaskList *pcb ; 
struct ready_queue *next ; 
}ready;
ready *inserir_pronta(ready *head, TaskList *alvo) ;
ready *remover_pronta(ready *head, TaskList *alvo) ;
void liberar_prontas(ready *head) ;
void liberar_cadastradas(TaskList *head) ;
TaskList *inserir_tarefa(TaskList *head, Task t);
int contar_tarefas(TaskList *head) ;
#endif