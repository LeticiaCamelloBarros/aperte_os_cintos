#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
ready *inserir_pronta(ready *head, TaskList *alvo) ;
//Instância de uma tarefa = cada nova ativação (chegada) daquela tarefa periódica.
void liberar_cadastradas(TaskList *head) ;
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

/* ---------------------------------------------------------------------
 * LISTA 1: Tarefas CADASTRADAS
 * Construida uma vez, a partir do arquivo. Nao muda depois disso.
 * E' o "dono" de cada Task -- o estado real de cada tarefa mora aqui.
 * --------------------------------------------------------------------- */
typedef struct task_list {
    Task tarefa;
    struct task_list *next;
} TaskList;
typedef struct ready_queue {
TaskList *pcb ; 
struct ready_queue *next ; 
}ready;
ready *remover_pronta(ready *head, TaskList *alvo) ;
void liberar_prontas(ready *head) ;
ready *inserir_pronta(ready *head, TaskList *alvo) ;
void liberar_cadastradas(TaskList *head) ;
TaskList *inserir_tarefa(TaskList *head, Task t);
int contar_tarefas(TaskList *head) ;
