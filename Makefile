# Copyright Andrei-Dragos Badoiu 315CAa 2022-2023

CC = gcc
FLAGS = -Wall -Wextra -std=c99
OUT = image_editor
TARGETS = $(OUT).c commands.c commands.h

build: $(TARGETS)
	$(CC) $(FLAGS) $(TARGETS) -lm -o $(OUT)

test: $(TARGETS)
	$(CC) $(FLAGS) -g $(TARGETS) -lm -o $(OUT)
	valgrind --leak-check=full --max-stackframe=3000000 -s ./$(OUT)

clean:
	rm ./$(OUT)

pack:
	zip zip.zip $(TARGETS) Makefile README

.PHONY: clean
