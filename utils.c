#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h> //pular linha em branco 
#include "utils.h"
/*Remove da fila de prontos o no cujo ponteiro ->tarefa == alvo.
 * Retorna a nova head. Nao mexe na lista de cadastradas.*/  
ready *remover_pronta(ready *head, TaskList *alvo) {
    ready *atual = head, *anterior = NULL;
 
    while (atual != NULL) {
        if (atual->pcb == alvo) {
            if (anterior == NULL) {
                ready *proximo = atual->next;
                free(atual);
                return proximo;
            } else {
                anterior->next = atual->next;
                free(atual);
                return head;
            }
        }
        anterior = atual;
        atual = atual->next;
    }
    return head; /* nao encontrou, nada a fazer */
}
 
void liberar_prontas(ready *head) {
    while (head != NULL) {
        ready *proximo = head->next;
        free(head);
        head = proximo;
    }
}
 
/* Insere no INICIO da fila de prontos (ordem nao importa, a escolha por
 * prioridade eh feita percorrendo a lista inteira depois) */

ready *inserir_pronta(ready *head, TaskList *alvo) {
    ready *novo = malloc(sizeof(ready));
    if (novo == NULL) {
        fprintf(stderr, "ERRO: falha ao alocar memoria\n");
        exit(EXIT_FAILURE);
    }
    novo->pcb = alvo;
    novo->next = head;
    return novo;
}
void liberar_cadastradas(TaskList *head) {
    while (head != NULL) {
        TaskList *proximo = head->next;
        free(head);
        head = proximo;
    }
}
int contar_tarefas(TaskList *head) {
    int n = 0;
    for (TaskList *atual = head; atual != NULL; atual = atual->next) n++;
    return n;
}
/*Insere uma tarefa no fim da lista de cadastradas (mantem ordem do arquivo)*/
TaskList *inserir_tarefa(TaskList *head, Task t) {
    TaskList *novo = malloc(sizeof(TaskList));
    if (novo == NULL) {
        fprintf(stderr, "ERRO: falha ao alocar memoria\n");
        exit(EXIT_FAILURE);
    }
    novo->tarefa = t;
    novo->next = NULL;
    if (head == NULL) {
        return novo; }
    TaskList *atual = head;
    while (atual->next != NULL){
         atual = atual->next;}
    atual->next = novo;
    return head;
}