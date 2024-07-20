#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "common.h"

#include "fiesta/str.h"

typedef struct {
    str src;
    int cur;
    int line_num;
    Instruction instrs[MAX_INSTRS];
    int cur_instr;
    uint8_t cur_operand;
    Label labels[MAX_LABELS];
    int cur_label_index;
    int num_labels;
    bool had_error;
} LexerState;

static bool names_initialized = false;
static dynstr instruction_names[NUM_INSTR_TYPES];
static dynstr gp_register_names[NUM_GP_REGISTERS];
static dynstr special_register_names[NUM_SPECIAL_REGISTERS];

LexerState lexer_lex(str src);
void lexer_free(LexerState* ls);