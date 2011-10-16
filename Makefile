
PROG_NAME = passageiro
FLAGS = -Isrc -D_XOPEN_SOURCE=600

SRC_DIR = src
OBJ_DIR = .temp

CC = gcc
CFLAGS = -ansi -pedantic -Wall -O2 $(FLAGS)

OUTPUT = $(PROG_NAME)

include objs.makefile

.PHONY: default
default: withnano

$(PROG_NAME): $(OBJ_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUTPUT)

.PHONY: withnano
withnano: CFLAGS += -D_POSIX_C_SOURCE=199309L -DUSE_NANO
withnano: $(PROG_NAME)

$(OBJ_DIR):
	mkdir $@

.temp/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

include deps.makefile

.PHONY: debug
debug: CFLAGS += -g
debug: $(PROG_NAME)

.PHONY: withnano_debug
withnano_debug: CFLAGS += -g
withnano_debug: withnano

.PHONY: clean
clean:
	rm -rf $(OUTPUT)
	rm -rf $(OBJ_DIR)


