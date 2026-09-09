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
/* SIMULACAO: usa as duas listas
 * --------------------------------------------------------------------- */
int simular_instante(TaskList *cadastradas, ready **fila_prontos, int t,
                      const char *algoritmo, int lost_deadlines[], int completas[]) {
 
    /* 1) Chegadas: percorre TODAS as cadastradas, insere na fila de prontos
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
 
    /* 3) Escolher a tarefa pronta de maior prioridade */
    TaskList *escolhida = NULL;
    for (ready *no = *fila_prontos; no != NULL; no = no->next) {
        TaskList *cand = no->pcb;
        if (escolhida == NULL) {
            escolhida = cand;
            continue;
        }
 
        int cand_vence;
        if (strcmp(algoritmo, "rate") == 0) {
            if (cand->tarefa.periodo < escolhida->tarefa.periodo)
                cand_vence = 1;
            else if (cand->tarefa.periodo == escolhida->tarefa.periodo)
                cand_vence = cand->tarefa.id_entrada < escolhida->tarefa.id_entrada;
            else
                cand_vence = 0;
        } else { /* edf */
            if (cand->tarefa.deadline_absoluto < escolhida->tarefa.deadline_absoluto)
                cand_vence = 1;
            else if (cand->tarefa.deadline_absoluto == escolhida->tarefa.deadline_absoluto)
                cand_vence = cand->tarefa.id_entrada < escolhida->tarefa.id_entrada;
            else
                cand_vence = 0;
        }
 
        if (cand_vence) escolhida = cand;
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
 

int carregar_tarefas(const char *caminho, TaskList **cadastradas_out, int *tempo_total_out) {
    FILE *f = fopen(caminho, "r");
    if (f == NULL) {
        fprintf(stderr, "ERRO: nao foi possivel abrir o arquivo '%s'\n", caminho);
        return -1;
    }

    char linha[256];

    /* ---- Primeira linha: tempo total de simulacao ---- */
    if (fgets(linha, sizeof(linha), f) == NULL) {
        fprintf(stderr, "ERRO: arquivo vazio ou ilegivel\n");
        fclose(f);
        return -1;
    }

    int tempo_total;
    char lixo[8];
    int campos = sscanf(linha, "%d %7s", &tempo_total, lixo);
    if (campos != 1 || tempo_total <= 0) {
        fprintf(stderr, "ERRO: tempo total invalido na primeira linha\n");
        fclose(f);
        return -1;
    }

    /* ---- Linhas seguintes: uma tarefa por linha ---- */
    TaskList *cadastradas = NULL; /* head da lista, comeca vazia */
    int n = 0;
    int numero_linha = 1; /* já lemos a linha 1 (tempo total) */

    while (fgets(linha, sizeof(linha), f) != NULL) {
        numero_linha++;

        /* pula linhas totalmente em branco no fim do arquivo */
        int vazio = 1;
        for (int i = 0; linha[i] != '\0'; i++) {
            if (!isspace((unsigned char)linha[i])) { vazio = 0; break; }
        }
        if (vazio) continue;

        char nome[20];
        int periodo, deadline, burst;
        char extra[8];

        int lidos = sscanf(linha, "%19s %d %d %d %7s",
                            nome, &periodo, &deadline, &burst, extra);

        if (lidos != 4) {
            fprintf(stderr,
                "ERRO: linha %d malformada (esperado NOME PERIODO DEADLINE BURST)\n",
                numero_linha);
            liberar_cadastradas(cadastradas);
            fclose(f);
            return -1;
        }

        if (periodo <= 0 || deadline <= 0 || burst <= 0) {
            fprintf(stderr,
                "ERRO: linha %d tem valor nao positivo (P=%d D=%d C=%d)\n",
                numero_linha, periodo, deadline, burst);
            liberar_cadastradas(cadastradas);
            fclose(f);
            return -1;
        }

        if (deadline > periodo || burst > deadline) {
            fprintf(stderr,
                "ERRO: linha %d viola C <= D <= P (tarefa '%s': P=%d D=%d C=%d)\n",
                numero_linha, nome, periodo, deadline, burst);
            liberar_cadastradas(cadastradas);
            fclose(f);
            return -1;
        }

        /* ---- Monta a Task e insere na lista ---- */
        Task t;
        strcpy(t.nome, nome);
        t.periodo    = periodo;
        t.deadline   = deadline;
        t.burst      = burst;
        t.id_entrada = n; /* ordem de aparicao no arquivo = criterio de desempate */

        /* dados dinamicos: todas as tarefas "chegam" pela 1a vez no instante 0 */
        t.tempo_restante    = 0; /* so vira burst quando a instancia chegar de fato */
        t.proxima_chegada   = 0;
        t.deadline_absoluto = 0;

        cadastradas = inserir_tarefa(cadastradas, t);
        n++;
    }

    fclose(f);

    if (n == 0) {
        fprintf(stderr, "ERRO: nenhuma tarefa encontrada no arquivo\n");
        return -1; /* cadastradas ja esta NULL, nada a liberar */
    }

    *cadastradas_out = cadastradas;
    *tempo_total_out = tempo_total;
    return n;
}