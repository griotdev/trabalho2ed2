CC := gcc
CFLAGS := -std=c99 -fstack-protector-all -Wall -Wextra -pedantic -I../include

SRC_DIR := .
OBJ_DIR := ../obj
BIN_DIR := ../bin
TARGET := $(BIN_DIR)/ted.exe

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	if not exist ..\obj mkdir ..\obj

$(BIN_DIR):
	if not exist ..\bin mkdir ..\bin

test: all
	@echo "Nenhum teste unitario implementado ainda."

clean:
	-del /Q ..\obj\*.o 2>NUL
	-del /Q ..\bin\ted.exe 2>NUL
