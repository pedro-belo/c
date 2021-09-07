BIN_DIR=./bin
OBJ_DIR=./obj
SRC_DIR=./src

BIN_NAME=huffman
CC=gcc
FLAGS=-Wall

SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
OBJ_FILES=$(subst $(SRC_DIR),$(OBJ_DIR),$(SRC_FILES:%.c=%.o))

TEST_NAME=test
TEST_CONTENT=abacaxi
TEST_DIR=/tmp

all: compile

compile: $(OBJ_DIR)/main.o $(OBJ_FILES)
	@ $(CC) -o $(BIN_DIR)/$(BIN_NAME) $(OBJ_DIR)/main.o $(OBJ_FILES)
	@ echo "Compiled !"

$(OBJ_DIR)/main.o: main.c
	@ $(CC) -c $< -o $@ ${FLAGS}

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@ $(CC) -c $< -o $@ ${FLAGS}

clean:
	@ rm $(OBJ_DIR)/*.o $(BIN_DIR)/${BIN_NAME} -f
	@ echo "Binary and object files were removed."

create_task_file:
	@ echo ${TEST_CONTENT} > $(TEST_DIR)/$(TEST_NAME)

run_task_s: create_task_file
	@ $(BIN_DIR)/$(BIN_NAME) -s $(TEST_DIR)/$(TEST_NAME)

run_task_c: create_task_file
	@ $(BIN_DIR)/$(BIN_NAME) -c $(TEST_DIR)/$(TEST_NAME)

run_task_d: create_task_file
	@ $(BIN_DIR)/$(BIN_NAME) -d ./file.pn
