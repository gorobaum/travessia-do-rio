PROG_NAME = passageiro
FLAGS = -D_GNU_SOURCE -D_XOPEN_SOURCE=600 -Isrc

SRC_DIR = src
OBJ_DIR = .temp
#OUTPUT_DIR = bin

CC = gcc
CFLAGS = -ansi -pedantic -Wall -O2 $(FLAGS)

#OUTPUT = bin/$(PROG_NAME)
OUTPUT = $(PROG_NAME)

include objs.makefile

#$(PROG_NAME): $(OBJ_DIR) $(OUTPUT_DIR) $(OBJS)
$(PROG_NAME): $(OBJ_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUTPUT)

$(OBJ_DIR):
	mkdir $@

#$(OUTPUT_DIR):
#	mkdir $@

.temp/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

include deps.makefile

.PHONY: debug
debug: CFLAGS += -g
debug: $(PROG_NAME)

.PHONY: clean
clean:
#	rm -rf $(OUTPUT)
	rm -rf $(OBJ_DIR)


