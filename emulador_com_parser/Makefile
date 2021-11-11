BIN_DIR=./bin
TRANSPILER_DIR=./transpiler
EMULATOR_DIR=./emulator

TRANSPILER_BIN=parser
TRANSPILER_IN=exemple.asm
TRANSPILER_OUT=prog.bin

EMULATOR_BIN=emulator
EMULATOR_CPP=main.cpp

FLEX=main.l
FLEX_C=lex.yy.c

BISON=main.y
BISON_C=main.tab.c
BISON_H=main.tab.h

all: run

run: compile_parser compile_emulator
	@ echo "Running"
	@ $(BIN_DIR)/$(TRANSPILER_BIN) $(TRANSPILER_IN) > $(TRANSPILER_OUT)
	@ $(BIN_DIR)/$(EMULATOR_BIN) ./$(TRANSPILER_OUT)

compile_emulator:
	@ g++ $(EMULATOR_DIR)/$(EMULATOR_CPP) -o $(BIN_DIR)/$(EMULATOR_BIN)

compile_parser: flex bison
	@ gcc $(TRANSPILER_DIR)/$(BISON_C) -o ${BIN_DIR}/${TRANSPILER_BIN}

flex:
	@ mkdir $(BIN_DIR) -p
	@ $@ -o $(TRANSPILER_DIR)/${FLEX_C} $(TRANSPILER_DIR)/$(FLEX)

bison:
	@ $@ -o $(TRANSPILER_DIR)/$(BISON_C) -d $(TRANSPILER_DIR)/${BISON}

clean:
	@ rm  $(TRANSPILER_DIR)/${FLEX_C} $(TRANSPILER_DIR)/$(BISON_C) $(TRANSPILER_DIR)/$(BISON_H) -f
	@ rm $(BIN_DIR)/$(TRANSPILER_BIN) $(BIN_DIR)/$(EMULATOR_BIN) -f
	@ rm ./$(TRANSPILER_OUT) -f
