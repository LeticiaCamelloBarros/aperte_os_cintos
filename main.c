#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]){
if (argc != 3) {
    //o nome do executável também conta como argumento  
        fprintf(stderr, "Uso: %s [algoritmo] [arquivo] \n", argv[0]);
        return EXIT_FAILURE;
    }
}