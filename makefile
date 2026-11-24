CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = mypreprocessor
OBJS = main.o comment_removal.o file_inclusion.o macro_handler.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

main.o: main.c 
	$(CC) -c main.c

comment_removal.o: comment_removal.c
	$(CC) -c comment_removal.c

file_inclusion.o: file_inclusion.c
	$(CC) -c file_inclusion.c

macro_handler.o: macro_handler.c
	$(CC) -c macro_handler.c

clear:
	@echo "clearing"
	rm -f $(OBJS)

.PHONY: all clean
