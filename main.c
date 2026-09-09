#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        /* o nome do executável também conta como argumento */
        fprintf(stderr, "Uso: %s [algoritmo] [arquivo]\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* valida o algoritmo ANTES de gastar trabalho carregando o arquivo */
    if (strcmp(argv[1], "rate") != 0 && strcmp(argv[1], "edf") != 0) {
        fprintf(stderr, "ERRO: '%s' nao e' um algoritmo valido (use 'rate' ou 'edf')\n", argv[1]);
        return EXIT_FAILURE;
    }

    /* ---- Cadastro das tarefas (lista 1) ---- */
    TaskList *cadastradas = NULL;
    int tempo_total = 0;

    int n = carregar_tarefas(argv[2], &cadastradas, &tempo_total);
    if (n < 0) {
        return EXIT_FAILURE; /* erro já foi impresso em stderr por carregar_tarefas */
    }

    /* ---- Contadores de estatisticas, um por tarefa (indexados por id_entrada) ---- */
    int lost_deadlines[n];
    int completas[n];
    memset(lost_deadlines, 0, sizeof(lost_deadlines));
    memset(completas, 0, sizeof(completas));

    /* ---- Fila de prontos (lista 2), comeca vazia ---- */
    ready *fila_prontos = NULL;

    /* ---- Laço principal da simulação: uma unidade de tempo por iteração ---- */
    for (int t = 0; t < tempo_total; t++) {
        /* ambos os algoritmos são preemptivos: uma tarefa de maior prioridade
         * que chega ao sistema interrompe imediatamente a que está executando */
        simular_instante(cadastradas, &fila_prontos, t, argv[1], lost_deadlines, completas);
    }

    /* ---- Aqui entraria a geração do arquivo de saida (.out) com base em
     * lost_deadlines[], completas[] e no log que simular_instante produziu ---- */

    /* ---- Libera tudo que foi alocado dinamicamente ---- */
    liberar_prontas(fila_prontos);      /* deve estar vazia aqui, mas por seguranca */
    liberar_cadastradas(cadastradas);

    return EXIT_SUCCESS;
}