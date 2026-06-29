CC := gcc
CFLAGS := -std=c99 -fstack-protector-all -Wall -Wextra -pedantic -I../include

SRC_DIR := .
TEST_DIR := ../tests
OBJ_DIR := ../obj
BIN_DIR := ../bin
TARGET := $(BIN_DIR)/ted.exe
TEST_TARGET := $(BIN_DIR)/test_args.exe

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
APP_SOURCES := $(filter-out $(SRC_DIR)/main.c,$(SOURCES))
TEST_SOURCES := $(TEST_DIR)/test_args.c $(APP_SOURCES)
TEST_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(APP_SOURCES)) $(OBJ_DIR)/test_args.o

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

test: $(TEST_TARGET)
	$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(TEST_OBJECTS) -o $@

$(OBJ_DIR)/test_args.o: $(TEST_DIR)/test_args.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-del /Q ..\obj\*.o 2>NUL
	-del /Q ..\bin\ted.exe 2>NUL
	-del /Q ..\bin\test_args.exe 2>NUL
