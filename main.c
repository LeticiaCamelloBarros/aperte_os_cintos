#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
int main (int argc, char *argv[]){
if (argc != 3) {
    //o nome do executável também conta como argumento  
        fprintf(stderr, "Uso: %s [algoritmo] [arquivo] \n", argv[0]);
        return EXIT_FAILURE;
    }
    // ambos os algoritmos são preemptivos, uma tarefa de maior prioridade que chega ao sistema interrompe imediatamente a que está executando
    if (strcmp(argv[1] , "rate")==0){
        Task *tarefas = NULL;
        int tempo_total = 0;
        
        int n = carregar_tarefas(argv[2], &tarefas, &tempo_total);
        if (n < 0) {
            return EXIT_FAILURE; /* erro já foi impresso em stderr */
        }
        
        /* aqui você já posso chamar simular_instante() dentro de um for (t=0; t<tempo_total; t++) */
        
        free(tarefas); /* não esquecer no final */
//...
    }else if (strcmp(argv[1] , "edf")==0){
        Task *tarefas = NULL;
        int tempo_total = 0;
        
        int n = carregar_tarefas(argv[2], &tarefas, &tempo_total);
        if (n < 0) {
            return EXIT_FAILURE; /* erro já foi impresso em stderr */
        }
        
        
        free(tarefas); /* não esquecer no final */
//..;
    }else{
        fprintf(stderr, "ERROR : o argumento %s não é o nome de um algoritmo válido", argv[1]);
        return EXIT_FAILURE;
    }
}