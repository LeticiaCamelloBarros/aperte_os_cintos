# ---------------------------------------------------------------------
# Makefile do simulador de escalonamento (Rate Monotonic / EDF)
# ---------------------------------------------------------------------

CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Requisito 2: o executavel final tem que se chamar exatamente "scheduler"
TARGET = scheduler

SRCS = main.c parser.c scheduler.c utils.c output.c
OBJS = $(SRCS:.c=.o)

# Requisito 1: "make" sozinho (sem alvo) tem que compilar o projeto.
# Como "all" e' o PRIMEIRO alvo do arquivo, e' ele que roda por padrao.
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regra generica: como gerar qualquer .o a partir do .c correspondente
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencias extras: se um .h mudar, os .c que o incluem recompilam
main.o:      main.c      utils.h parser.h scheduler.h output.h
parser.o:    parser.c    parser.h utils.h
scheduler.o: scheduler.c scheduler.h utils.h
utils.o:     utils.c     utils.h
output.o:    output.c    output.h utils.h

# Requisito 3: alvo de limpeza dos arquivos compilados e intermediarios
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean