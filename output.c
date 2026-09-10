#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "output.h"
 
/* Estado interno do logger: qual "trecho" esta aberto agora.
 *   task_atual == -2  -> nenhum trecho aberto ainda (inicio da simulacao)
 *   task_atual == -1  -> trecho aberto e' de CPU ociosa (idle)
 *   task_atual >= 0   -> trecho aberto e' de execucao da tarefa com esse id_entrada
 */
struct Logger {
    FILE *saida;
    int task_atual;
    int unidades;
};
 
Logger *logger_criar(FILE *saida) {
    Logger *log = malloc(sizeof(Logger));
    if (log == NULL) {
        fprintf(stderr, "ERRO: falha ao alocar memoria (logger)\n");
        exit(EXIT_FAILURE);
    }
    log->saida = saida;
    log->task_atual = -2; /* nada aberto ainda */
    log->unidades = 0;
    return log;
}