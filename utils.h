#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
//Instância de uma tarefa = cada nova ativação (chegada) daquela tarefa periódica.
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
typedef struct ready_queue {
Task pcb ; 
struct ready_queue *next ; 
}ready;
/* ---------------------------------------------------------------------
 * LISTA 1: Tarefas CADASTRADAS
 * Construida uma vez, a partir do arquivo. Nao muda depois disso.
 * E' o "dono" de cada Task -- o estado real de cada tarefa mora aqui.
 * --------------------------------------------------------------------- */
typedef struct task_list {
    Task tarefa;
    struct task_list *next;
} TaskList;
int simular_instante(Task tarefas[], int n, int t, const char *algoritmo,
                      int lost_deadlines[], int completas[]);
TaskList *inserir_tarefa(TaskList *head, Task t);
int carregar_tarefas(const char *caminho, Task **tarefas_out, int *tempo_total_out) ;
