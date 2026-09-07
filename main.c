#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]){
if (argc != 3) {
    //o nome do executável também conta como argumento  
        fprintf(stderr, "Uso: %s [algoritmo] [arquivo] \n", argv[0]);
        return EXIT_FAILURE;
    }
    // ambos os algoritmos são preemptivos, uma tarefa de maior prioridade que chega ao sistema interrompe imediatamente a que está executando
    if (argv[1] == "rate") {
//...
    }else if (argv[1]=="edf"){
//..;
    }else{
        fprintf(stderr, "ERROR : o argumento %s não é o nome de um algoritmo válido", argv[1]);
        return EXIT_FAILURE;
    }
}