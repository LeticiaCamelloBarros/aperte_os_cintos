

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g
TARGET  = scheduler

# Lista de todos os .c do projeto
SRCS = main.c utils.c scheduler.c parser.c

# Gera automaticamente a lista de .o a partir dos .c (main.c -> main.o, etc.)
OBJS = $(SRCS:.c=.o)

# Regra padrao: "make" sozinho constroi o executavel
all: $(TARGET)

# Linkagem final: junta todos os .o no executavel
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regra generica: como compilar qualquer .c em .o
# $< = o primeiro pre-requisito (o .c), $@ = o alvo (o .o)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencias extras: se um .h mudar, os .c que o incluem devem recompilar
main.o: main.c utils.h scheduler.h parser.h
utils.o: utils.c utils.h
algoritms.o: scheduler.c scheduler.h utils.h
parser.o: parser.c parser.h utils.h

# Remove tudo que foi gerado pela compilacao
clean:
	rm -f $(OBJS) $(TARGET)

# Recompila do zero
rebuild: clean all

# Roda o executavel com os algoritmos, exemplo de uso:
#   make run ARGS="rate voo.txt"
run: $(TARGET)
	./$(TARGET) $(ARGS)

.PHONY: all clean rebuild run