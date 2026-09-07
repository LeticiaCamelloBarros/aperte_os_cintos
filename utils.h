//Instância de uma tarefa = cada nova ativação (chegada) daquela tarefa periódica.
typedef struct {
    char nome[2];
    int periodo;          // P
    int deadline;         // D (relativo)
    int burst;            // C (tempo de CPU necessário)
    int id_entrada;       // Posição no arquivo (usado para desempate)
    
    // Estado dinâmico da instância atual
    int tempo_restante;   // Quantas u.t. ainda precisa executar nesta instância
    int proxima_chegada;  // Próximo instante t em que uma nova instância chegará
    int deadline_absoluto;// Instante limite para concluir (t_chegada + D)
} Task;

typedef struct ready_queue {
Task pcb ; 
struct ready_queue *next ; 
}ready;

