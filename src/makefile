CC := gcc
CFLAGS := -std=c99 -fstack-protector-all -Wall -Wextra -pedantic -I../include
TEST_CFLAGS := $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -I../tests/unity

SRC_DIR := .
TEST_DIR := ../tests
OBJ_DIR := ../obj
BIN_DIR := ../bin
TARGET := $(BIN_DIR)/ted.exe
TEST_TARGETS := $(BIN_DIR)/test_args.exe $(BIN_DIR)/test_block.exe $(BIN_DIR)/test_geometry.exe

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
APP_SOURCES := $(filter-out $(SRC_DIR)/main.c,$(SOURCES))
APP_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(APP_SOURCES))
UNITY_OBJECT := $(OBJ_DIR)/unity.o

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

test: $(TEST_TARGETS)
	$(BIN_DIR)/test_args.exe
	$(BIN_DIR)/test_block.exe
	$(BIN_DIR)/test_geometry.exe

$(BIN_DIR)/test_args.exe: $(OBJ_DIR)/test_args.o $(APP_OBJECTS) $(UNITY_OBJECT) | $(BIN_DIR)
	$(CC) $(TEST_CFLAGS) $(OBJ_DIR)/test_args.o $(APP_OBJECTS) $(UNITY_OBJECT) -o $@

$(BIN_DIR)/test_block.exe: $(OBJ_DIR)/test_block.o $(APP_OBJECTS) $(UNITY_OBJECT) | $(BIN_DIR)
	$(CC) $(TEST_CFLAGS) $(OBJ_DIR)/test_block.o $(APP_OBJECTS) $(UNITY_OBJECT) -o $@

$(BIN_DIR)/test_geometry.exe: $(OBJ_DIR)/test_geometry.o $(APP_OBJECTS) $(UNITY_OBJECT) | $(BIN_DIR)
	$(CC) $(TEST_CFLAGS) $(OBJ_DIR)/test_geometry.o $(APP_OBJECTS) $(UNITY_OBJECT) -o $@

$(OBJ_DIR)/test_args.o: $(TEST_DIR)/test_args.c | $(OBJ_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(OBJ_DIR)/test_block.o: $(TEST_DIR)/test_block.c | $(OBJ_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(OBJ_DIR)/test_geometry.o: $(TEST_DIR)/test_geometry.c | $(OBJ_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(UNITY_OBJECT): $(TEST_DIR)/unity/unity.c | $(OBJ_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

clean:
	-del /Q ..\obj\*.o 2>NUL
	-del /Q ..\bin\ted.exe 2>NUL
	-del /Q ..\bin\test_args.exe 2>NUL
	-del /Q ..\bin\test_block.exe 2>NUL
	-del /Q ..\bin\test_geometry.exe 2>NUL
