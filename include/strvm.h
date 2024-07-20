#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "common.h"

typedef struct {
    uint8_t value;
} Register;

typedef enum {
    NONE,
    INVALID_INSTRUCTION,
    INVALID_OPERAND,
    HALT
} VM_ErrorType;

typedef struct {
    VM_ErrorType type;
    Operand operands[NUM_OPERANDS];
} VM_Error;

#pragma pack(push, 1)

typedef struct {
    bool not_equal    : 1;
    bool equal        : 1;
    bool greater_than : 1;
    bool less_than    : 1;
} CompareFlags;

typedef struct {
    bool carry    : 1;
    bool overflow : 1;
    bool not_zero : 1;
} StatusRegister;

#pragma pack(pop)

typedef struct {
    Register registers[NUM_GP_REGISTERS]; // r0, r1, r2, r3, r4, r5, r6, r7
    StatusRegister status_register;       // rst
    Register zero_register;               // rz
    CompareFlags compare_register;        // rcmp
    uint16_t program_counter;             // pc
    uint16_t instr_ptr;                   // ip
    Label labels[MAX_LABELS];
    int num_labels;
    uint8_t memory[MEMORY_SIZE];
} VM;

VM vm_init(Label labels[MAX_LABELS], int num_labels);
VM_Error vm_run(VM* vm, Instruction instrs[MAX_LABELS], uint8_t num_instrs);