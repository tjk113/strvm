CC := gcc
S := src/
B := bin/
FIESTA_PARENT_DIR := ..

FLAGS = -Iinclude -I"$(FIESTA_PARENT_DIR)" -std=c17

OBJ_FILES := $(B)main.o $(B)strvm.o $(B)lexer.o

$(B)strvm.exe: $(OBJ_FILES) $(FIESTA_PARENT_DIR)/fiesta/libfiesta.a
	mkdir -p $(B)
	$(CC) $^ -o $@ -L$(FIESTA_PARENT_DIR)/fiesta -lfiesta $(FLAGS)

$(B)%.o: $(S)%.c
	$(CC) -c $< -o $@ $(FLAGS)

$(FIESTA_PARENT_DIR)/fiesta/libfiesta.a:
	cd $(FIESTA_PARENT_DIR)/fiesta && make lib

dbg: FLAGS += -g
dbg: $(B)strvm.exe

opt: FLAGS += -O2
opt: $(B)strvm.exe

dbgopt: FLAGS += -Og
dbgopt: $(B)strvm.exe

exe: $(B)strvm.exe
	$(RM) $(OBJ_FILES)

clean:
	$(RM) $(B)strvm.exe $(OBJ_FILES)