#ifndef PARSER_H
#define PARSER_H

#include "utils.h"

/* Lê o arquivo de entrada, valida cada linha e cadastra as tarefa numa lista encadeada (TaskList).
 * Formato esperado do arquivo:
 * Parâmetros:caminho (caminho do arquivo de entrada (argv[2])) ,cadastradas_out ( endereço de um ponteiro TaskList*; a função constrói
 *                       a lista e devolve a head dela aqui)
 *  e  tempo_total_out  (endereço de um int onde a função guarda o tempo total lido) 
 *
 * Retorno:>= 0  (sucesso, valor = quantidade de tarefas cadastradas (n)) e
 * -1    ( erro (mensagem já foi escrita em stderr, nada fica alocado))
 *
 * Validações aplicadas a cada linha de tarefa:
 *   - exatamente 4 campos (NOME PERIODO DEADLINE BURST)
 *   - periodo, deadline e burst positivos (> 0)
 *   - burst <= deadline <= periodo (C <= D <= P)
 *
 * Importante: quem chamar essa função é responsável por dar
 * liberar_cadastradas(*cadastradas_out) depois de usar, e por checar
 * o retorno ANTES de acessar *cadastradas_out. */
int carregar_tarefas(const char *caminho, TaskList **cadastradas_out, int *tempo_total_out);

#endif