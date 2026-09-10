#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "scheduler.h"
#include "parser.h"
#include "output.h"

/* TODO: troque pelas iniciais do seu e-mail em minusculas, ex: "mla" */
#define LOGIN "lsncb"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s [algoritmo] [arquivo]\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* resolve a string do algoritmo para um ponteiro de funcao UMA VEZ SO,
     * antes do laco principal */
    comparar_prioridade comparar = escolher_algoritmo(argv[1]);
    if (comparar == NULL) {
        fprintf(stderr, "ERRO: '%s' nao e' um algoritmo valido (use 'rate' ou 'edf')\n", argv[1]);
        return EXIT_FAILURE;
    }

    /* ---- Cadastro das tarefas ---- */
    TaskList *cadastradas = NULL;
    int tempo_total = 0;

    int n = carregar_tarefas(argv[2], &cadastradas, &tempo_total);
    if (n < 0) {
        return EXIT_FAILURE; /* erro já foi impresso em stderr por carregar_tarefas */
    }

    /* ---- Nome do arquivo de saida: rate_login.out ou edf_login.out ---- */
    char nome_saida[64];
    snprintf(nome_saida, sizeof(nome_saida), "%s_%s.out", argv[1], LOGIN);

    FILE *saida = fopen(nome_saida, "w");
    if (saida == NULL) {
        fprintf(stderr, "ERRO: nao foi possivel criar o arquivo de saida '%s'\n", nome_saida);
        liberar_cadastradas(cadastradas);
        return EXIT_FAILURE;
    }

    /* ---- Contadores de estatisticas, um por tarefa (indexados por id_entrada) ---- */
    int lost_deadlines[n], completas[n];
    memset(lost_deadlines, 0, sizeof(lost_deadlines));
    memset(completas, 0, sizeof(completas));

    /* copias do tick anterior, usadas so' para detectar o que MUDOU neste
     * instante (ninguem mais alem do main precisa saber disso) */
    int lost_antes[n], completas_antes[n];

    ready *fila_prontos = NULL;

    escrever_cabecalho(saida, argv[1]);
    Logger *log = logger_criar(saida);

    /* ---- Laço principal da simulação ---- */
    for (int t = 0; t < tempo_total; t++) {
        memcpy(lost_antes, lost_deadlines, sizeof(lost_antes));
        memcpy(completas_antes, completas, sizeof(completas_antes));

        int executada_id = simular_instante(cadastradas, &fila_prontos, t,
                                             comparar, lost_deadlines, completas);

        /* descobre quem completou/perdeu NESTE tick comparando antes/depois
         * (assume no maximo um evento de cada tipo por tick) */
        int completou_id = -1, perdeu_id = -1;
        for (int i = 0; i < n; i++) {
            if (completas[i] > completas_antes[i]) completou_id = i;
            if (lost_deadlines[i] > lost_antes[i]) perdeu_id = i;
        }

        logger_processar_tick(log, cadastradas, executada_id, completou_id, perdeu_id);
    }

    /* ---- Quem ainda tinha burst pendente quando o tempo acabou foi "morto" ---- */
    int killed[n];
    memset(killed, 0, sizeof(killed));
    for (TaskList *no = cadastradas; no != NULL; no = no->next) {
        if (no->tarefa.tempo_restante > 0) killed[no->tarefa.id_entrada] = 1;
    }

    logger_finalizar(log, cadastradas, killed);
    escrever_estatisticas(saida, cadastradas, lost_deadlines, completas, killed);

    /* ---- Limpeza ---- */
    logger_destruir(log);
    fclose(saida);
    liberar_prontas(fila_prontos);
    liberar_cadastradas(cadastradas);

    return EXIT_SUCCESS;
}