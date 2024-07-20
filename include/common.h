#pragma once

#include <stdint.h>

#include "fiesta/str.h"

#define MAX_INSTRS  1024
#define MAX_LABELS  1024

#define NUM_GP_REGISTERS      8
#define NUM_SPECIAL_REGISTERS 5
#define NUM_OPERANDS          3
#define MEMORY_SIZE        1024

typedef struct {
    str name;
    uint16_t address;
} Label;

typedef enum {
    NOP, // NO OPERATION
    MOV, // MOVE (R), (R/I)
    ADD, // ADD WITHOUT CARRY (R) (R/I)
    ADC, // ADD WITH CARRY (R) (R/I)
    SUB, // SUBTRACT WITHOUT CARRY (R) (R/I)
    SBC, // SUBTRACT WITH CARRY (R) (R/I)
    CLC, // CLEAR CARRY BIT
    CLV, // CLEAR OVERFLOW BIT
    MUL, // MULTIPLY (R) (R/I)
    DIV, // DIVIDE (R) (R/I)
    NEG, // NEGATE (R)
    SHL, // SHIFT LEFT (R) (R/I)
    SHR, // SHIFT RIGHT (R) (R/I)
    STR, // STORE IN MEMORY (R/I) (R/I)
    LD,  // LOAD FROM MEMORY (R) (R/I)
    CMP, // COMPARE (R/I) (R/I)
    JMP, // JUMP (L)
    JNE, // JUMP IF NOT EQUAL (L)
    JE,  // JUMP IF EQUAL (L)
    JGT, // JUMP IF GREATER THAN (L)
    JLT, // JUMP IF LESS THAN (L)
    JNZ, // JUMP IF NOT ZERO (L)
    JZ,  // JUMP IF ZERO (L)
    PTC, // PRINT AS CHAR (R/I)
    PTN, // PRINT AS SIGNED NUMBER (R/I)
    PTU, // PRINT AS UNSIGNED NUMBER (R/I)
    HLT, // HALT EXECUTION
    NUM_INSTR_TYPES
} InstructionType;

typedef struct {
    bool is_register;
    bool is_label;
    uint8_t value;
} Operand;

typedef struct {
    InstructionType type;
    Operand operands[NUM_OPERANDS];
} Instruction;