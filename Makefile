CC := gcc
CFLAGS := -std=c99 -fstack-protector-all -Wall -Wextra -pedantic -Iinclude
TEST_CFLAGS := $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -Itests/unity

SRC_DIR := src
TEST_DIR := tests
OBJ_DIR := obj
BIN_DIR := bin

TARGET := $(SRC_DIR)/ted

MODULE_SOURCES := $(filter-out $(SRC_DIR)/main.c,$(wildcard $(SRC_DIR)/*.c))
APP_SOURCES := $(wildcard $(SRC_DIR)/*.c)
APP_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(APP_SOURCES))
MODULE_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(MODULE_SOURCES))
UNITY_OBJECT := $(OBJ_DIR)/unity.o

TEST_NAMES := args block geometry file_paths geo output graph via registers qry route road_components road_expansion
TEST_TARGETS := $(patsubst %,$(BIN_DIR)/test_%,$(TEST_NAMES))

ifeq ($(OS),Windows_NT)
MKDIR_P = if not exist "$(subst /,\,$@)" mkdir "$(subst /,\,$@)"
TEST_RUNNER = cd $(SRC_DIR) && ..\$(BIN_DIR)\test_$1
else
MKDIR_P = mkdir -p $@
TEST_RUNNER = cd $(SRC_DIR) && ../$(BIN_DIR)/test_$1
endif

.PHONY: all ted test clean

all: ted

ted: $(TARGET)

$(TARGET): $(APP_OBJECTS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(APP_OBJECTS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/test_%.o: $(TEST_DIR)/test_%.c | $(OBJ_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(UNITY_OBJECT): $(TEST_DIR)/unity/unity.c | $(OBJ_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(BIN_DIR)/test_%: $(OBJ_DIR)/test_%.o $(MODULE_OBJECTS) $(UNITY_OBJECT) | $(BIN_DIR)
	$(CC) $(TEST_CFLAGS) $^ -o $@

$(OBJ_DIR) $(BIN_DIR):
	$(MKDIR_P)

test: $(TEST_TARGETS)
	$(call TEST_RUNNER,args)
	$(call TEST_RUNNER,block)
	$(call TEST_RUNNER,geometry)
	$(call TEST_RUNNER,file_paths)
	$(call TEST_RUNNER,geo)
	$(call TEST_RUNNER,output)
	$(call TEST_RUNNER,graph)
	$(call TEST_RUNNER,via)
	$(call TEST_RUNNER,registers)
	$(call TEST_RUNNER,qry)
	$(call TEST_RUNNER,route)
	$(call TEST_RUNNER,road_components)
	$(call TEST_RUNNER,road_expansion)

clean:
ifeq ($(OS),Windows_NT)
	-del /Q $(subst /,\,$(OBJ_DIR))\*.o 2>NUL
	-del /Q $(subst /,\,$(TARGET)) 2>NUL
	-del /Q $(subst /,\,$(TARGET)).exe 2>NUL
	-del /Q ted 2>NUL
	-del /Q ted.exe 2>NUL
	-del /Q $(subst /,\,$(BIN_DIR))\test_* 2>NUL
else
	rm -f $(OBJ_DIR)/*.o
	rm -f $(TARGET)
	rm -f ted
	rm -f ted.exe
	rm -f $(TEST_TARGETS)
endif
