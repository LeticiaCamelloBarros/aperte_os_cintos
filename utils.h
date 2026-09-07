typedef struct PCB{
//dados estáticos (lidos do arquivo de entrada)
char nome[20];//nome da tarefa
int periodo ;//periodo da tarefa
int burst ; //Tempo de computação/execução necessário
int deadline ;// Deadline relativo 
int id_entrada ;//A ordem em que a tarefa apareceu no arquivo (crucial para aplicar a regra de desempate do projeto)
// Dados Dinâmicos (atualizados a cada unidade de tempo na simulação)
int tempo_restante; //Quanto tempo de CPU a instância atual ainda precisa para terminar (essencial para tratar a preempção, onde uma tarefa é interrompida e retoma depois de onde parou)
int proxima_chegada;// O próximo instante de tempo em que uma nova instância da tarefa vai chegar (com base no período $P$)
int deadline_absoluto;// O tempo limite exato para a instância atual terminar. No EDF, a prioridade é definida por quem tem o menor deadline absoluto
}PCB;

typedef struct ready_queue {
PCB processo ; 
struct ready_queue *next ; 
}ready;