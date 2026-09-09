#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h> //pular linha em branco 
#include "utils.h"

//tarefas[] é o array com todas as tarefas do meu arquivo de entrada, 
//sempre as mesmas n posições do começo ao fim da simulação. Não é uma "fila de prontos" que cresce e diminui.
int simular_instante(Task tarefas[], int n, int t, const char *algoritmo,
                      int lost_deadlines[], int completas[]) {

 
    /* Verificar chegada de novas instâncias */
    for (int i = 0; i < n; i++) {
        //se t bate com proxima_chegada, reseta o burst restante
        //, calcula o novo deadline absoluto (t + deadline) e já agenda a próxima chegada (+= periodo).
        if (t == tarefas[i].proxima_chegada) {
            tarefas[i].tempo_restante   = tarefas[i].burst;
            tarefas[i].deadline_absoluto = t + tarefas[i].deadline;
            tarefas[i].proxima_chegada  += tarefas[i].periodo;
        }
    }
 
    /* 2) Verificar deadlines perdidos (instância não terminou até o deadline absoluto) */
    for (int i = 0; i < n; i++) {
        if (tarefas[i].tempo_restante > 0 && t == tarefas[i].deadline_absoluto) {
            lost_deadlines[i]++;
            tarefas[i].tempo_restante = 0; /* descarta a rajada restante */
        }
    }
 
    /* 3) Escolher, entre as tarefas prontas, a de maior prioridade */
    int escolhida = -1;
    for (int i = 0; i < n; i++) {
        if (tarefas[i].tempo_restante <= 0) continue; /* não está pronta */
 
        if (escolhida == -1) {
            escolhida = i;
            continue;
        }
 
        int i_vence;
        if (strcmp(algoritmo, "rate") == 0) {
            /* Rate Monotonic: menor período = maior prioridade */
            if (tarefas[i].periodo < tarefas[escolhida].periodo)
                i_vence = 1;
            else if (tarefas[i].periodo == tarefas[escolhida].periodo)
                i_vence = tarefas[i].id_entrada < tarefas[escolhida].id_entrada;
            else
                i_vence = 0;
        } else {
            /* EDF: menor deadline absoluto = maior prioridade */
            if (tarefas[i].deadline_absoluto < tarefas[escolhida].deadline_absoluto)
                i_vence = 1;
            else if (tarefas[i].deadline_absoluto == tarefas[escolhida].deadline_absoluto)
                i_vence = tarefas[i].id_entrada < tarefas[escolhida].id_entrada;
            else
                i_vence = 0;
        }
 
        if (i_vence) escolhida = i;
    }
 
    /* 4) Executar 1 unidade de tempo da tarefa escolhida */
    if (escolhida != -1) {
        tarefas[escolhida].tempo_restante--;
        if (tarefas[escolhida].tempo_restante == 0) {
            completas[escolhida]++;
        }
    }
 
    /* 5) Registrar no log quem executou (ou idle) nesse instante */
    if (escolhida != -1) {
        fprintf(stderr, "[t=%d] %s executando (restante=%d)\n",
                t, tarefas[escolhida].nome, tarefas[escolhida].tempo_restante);
    } else {
        fprintf(stderr, "[t=%d] idle\n", t);
    }
 
    return escolhida;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

/*
 * Lê o arquivo de entrada e cadastra as tarefas.
 *
 * Formato esperado:
 *   [TEMPO TOTAL]
 *   [NOME] [PERIODO] [DEADLINE] [BURST]
 *   ...
 *
 * Parâmetros:
 *   caminho          - caminho do arquivo de entrada (argv[2])
 *   tarefas_out      - endereço de um ponteiro Task*; a função aloca o vetor
 *                       e devolve o endereço dele aqui (*tarefas_out = vetor)
 *   tempo_total_out  - endereço de um int onde a função guarda o tempo total lido
 *
 * Retorno:
 *   >= 0  -> sucesso, valor = quantidade de tarefas cadastradas (n)
 *   -1    -> erro (mensagem já foi escrita em stderr, nada foi alocado)
 *
 * Importante: quem chamar essa função é responsável por dar free(*tarefas_out)
 * depois de usar, e por checar erro ANTES de acessar *tarefas_out.
 */
int carregar_tarefas(const char *caminho, Task **tarefas_out, int *tempo_total_out) {
//carrega as tarefas e as cadastra 
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
    /* se sobrar algo além do número na linha, sscanf com %7s captura e denuncia erro */
    int campos = sscanf(linha, "%d %7s", &tempo_total, lixo);
    if (campos != 1 || tempo_total <= 0) {
        fprintf(stderr, "ERRO: tempo total invalido na primeira linha\n");
        fclose(f);
        return -1;
    }

    /* ---- Linhas seguintes: uma tarefa por linha ---- */
    int capacidade = 8;
    Task *tarefas = malloc(capacidade * sizeof(Task));
    if (tarefas == NULL) {
        fprintf(stderr, "ERRO: falha ao alocar memoria\n");
        fclose(f);
        return -1;
    }

    int n = 0;
    int numero_linha = 1; /* já lemos a linha 1 (tempo total) */

    while (fgets(linha, sizeof(linha), f) != NULL) {
        numero_linha++;

        /* pula linhas totalmente em branco no fim do arquivo */
        char so_espacos[256];
        strcpy(so_espacos, linha);
        int vazio = 1;
        for (int i = 0; so_espacos[i] != '\0'; i++) {
            if (!isspace((unsigned char)so_espacos[i])) { vazio = 0; break; }
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
            free(tarefas);
            fclose(f);
            return -1;
        }

        if (periodo <= 0 || deadline <= 0 || burst <= 0) {
            fprintf(stderr,
                "ERRO: linha %d tem valor nao positivo (P=%d D=%d C=%d)\n",
                numero_linha, periodo, deadline, burst);
            free(tarefas);
            fclose(f);
            return -1;
        }

        if (deadline > periodo || burst > deadline) {
            fprintf(stderr,
                "ERRO: linha %d viola C <= D <= P (tarefa '%s': P=%d D=%d C=%d)\n",
                numero_linha, nome, periodo, deadline, burst);
            free(tarefas);
            fclose(f);
            return -1;
        }

        /* cresce o vetor se necessario */
        if (n == capacidade) {
            capacidade *= 2;
            Task *tmp = realloc(tarefas, capacidade * sizeof(Task));
            if (tmp == NULL) {
                fprintf(stderr, "ERRO: falha ao realocar memoria\n");
                free(tarefas);
                fclose(f);
                return -1;
            }
            tarefas = tmp;
        }

        /* ---- Preenche a struct Task ---- */
        strcpy(tarefas[n].nome, nome);
        tarefas[n].periodo    = periodo;
        tarefas[n].deadline   = deadline;
        tarefas[n].burst      = burst;
        tarefas[n].id_entrada = n; /* ordem de aparicao no arquivo = criterio de desempate */

        /* dados dinamicos: todas as tarefas chegam pela 1a vez no instante 0 */
        tarefas[n].tempo_restante    = 0; /* so vira burst quando a instancia "chegar" no t=0 */
        tarefas[n].proxima_chegada   = 0;
        tarefas[n].deadline_absoluto = 0;

        n++;
    }

    fclose(f);

    if (n == 0) {
        fprintf(stderr, "ERRO: nenhuma tarefa encontrada no arquivo\n");
        free(tarefas);
        return -1;
    }

    *tarefas_out = tarefas;
    *tempo_total_out = tempo_total;
    return n;
}