/* I'm refusing to write a separate parser
for such a simple syntax. Therefore this
lexer, while a bit messy, produces AST nodes
(Instructions, to be specific) itself. :)

TODO:
- unicode support lol */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "lexer.h"
#include "strvm.h"

#include "fiesta/str.h"

static char next(LexerState* ls) {
    return ls->src.data[++ls->cur];
}

static char cur(LexerState* ls) {
    return ls->src.data[ls->cur];
}

static char peek(LexerState* ls) {
    return ls->src.data[ls->cur+1];
}

static char peek_n(LexerState* ls, int offset) {
    return ls->src.data[ls->cur+offset];
}

static bool is_at_end(LexerState* ls) {
    return ls->cur >= ls->src.len - 1;
}

static void handle_number(LexerState* ls) {
    dynstr number = dynstr_create();
    dynstr_append_char(&number, cur(ls));
    while (!is_at_end(ls) && isdigit(peek(ls)))
        dynstr_append_char(&number, next(ls));

    Operand* op = &ls->instrs[ls->cur_instr].operands[ls->cur_operand];
    op->is_register = false;
    op->is_label = false;
    op->value = atoi(number.data);
}

/* This function's return value indicates whether
an instruction was actually parsed or not */
static bool try_handle_instruction(LexerState* ls, dynstr* buffer) {
    for (int i = 0; i < NUM_INSTR_TYPES; i++) {
        int res = !dynstr_compare_n(buffer, &instruction_names[i],
                                    fmin(buffer->len, instruction_names[i].len))
                  && buffer->len == instruction_names[i].len;
        if (res) {
            ls->instrs[ls->cur_instr] = (Instruction){.type = i};
            return true;
        }
    }
    return false;
}

/* This function's return value indicates whether
a register was actually parsed or not */
static bool try_handle_register(LexerState* ls, dynstr* buffer) {
    for (int i = 0; i < NUM_GP_REGISTERS; i++) {
        if ((i < 10 ? !dynstr_compare_n(buffer, &gp_register_names[i], 2)
                    : !dynstr_compare_n(buffer, &gp_register_names[i], 3))) {
            Operand* op = &ls->instrs[ls->cur_instr].operands[ls->cur_operand];
            op->is_register = true;
            op->is_label = false;
            op->value = i;
            return true;
        }
    }
    for (int i = 0; i < NUM_SPECIAL_REGISTERS; i++) {
        if (!dynstr_compare_n(buffer, &special_register_names[i],
            fmin(buffer->len, special_register_names[i].len))) {
            Operand* op = &ls->instrs[ls->cur_instr].operands[ls->cur_operand];
            op->is_register = true;
            op->is_label = false;
            op->value = i;
            return true;
        }
    }
    return false;
}

/* This function's return value is acts as an
error code indicating if an unexpected character
was encountered */
static bool try_handle_label(LexerState* ls, dynstr* buffer) {
    // Labels as operands
    if (is_at_end(ls) || isspace(peek(ls)) || peek(ls) == ',') {
        Operand* op = &ls->instrs[ls->cur_instr].operands[ls->cur_operand];
        op->is_register = false;
        op->is_label = true;

        str name = dynstr_to_str(buffer);
        *buffer = dynstr_create();

        bool found_label = false;
        for (int i = 0; i < ls->num_labels; i++) {
            if (!str_compare(&name, &ls->labels[i].name)) {
                // `value` is set to the index of the label
                op->value = i;
                found_label = true;
                break;
            }
        }
        /* If we come across a label that we haven't seen yet,
        then create the struct for it with a placeholder address
        to be filled in later when we actually find the label */
        if (!found_label) {
            ls->labels[ls->num_labels] = (Label){.name = name,
                                                 .address = 0};
            op->value = ls->num_labels++;
        }

        return true;
    }
    // Labels as, well, labels
    else if (peek(ls) == ':') {
        str name = dynstr_to_str(buffer);
        *buffer = dynstr_create();

        bool found_label = false;
        for (int i = 0; i < ls->num_labels; i++) {
            /* If we find the label already has an existing
            struct, then just fill in the address */
            if (!str_compare(&name, &ls->labels[i].name)) {
                ls->labels[i].address = ls->cur_instr;
                ls->cur_label_index = i;
                found_label = true;
                break;
            }
        }
        // Otherwise, create the struct here
        if (!found_label) {
            ls->labels[ls->cur_label_index++] = (Label){.name = name,
                                                        .address = ls->cur_instr};
            ls->num_labels++;
        }

        return true;
    }
    return false;
}

/* This function basically just ensures that
the buffer being used for string comparisons
reaches the minimum length to ensure that
said comparisons are correct. It then hands
off the actual parsing to helper functions.
It propagates errors from those functions, too. */
static bool handle_text(LexerState* ls) {
    dynstr buffer = dynstr_create();
    dynstr_append_char(&buffer, cur(ls));
    while (!is_at_end(ls) && isalnum(peek(ls)))
        dynstr_append_char(&buffer, next(ls));

    if (try_handle_instruction(ls, &buffer))
        return true;

    if (try_handle_register(ls, &buffer))
        return true;

    return try_handle_label(ls, &buffer);
}

static void initialize_names() {
    int cur = 0;
    instruction_names[cur++] = dynstr_create_from("nop");
    instruction_names[cur++] = dynstr_create_from("mov");
    instruction_names[cur++] = dynstr_create_from("add");
    instruction_names[cur++] = dynstr_create_from("adc");
    instruction_names[cur++] = dynstr_create_from("sub");
    instruction_names[cur++] = dynstr_create_from("sbc");
    instruction_names[cur++] = dynstr_create_from("clc");
    instruction_names[cur++] = dynstr_create_from("clv");
    instruction_names[cur++] = dynstr_create_from("mul");
    instruction_names[cur++] = dynstr_create_from("div");
    instruction_names[cur++] = dynstr_create_from("neg");
    instruction_names[cur++] = dynstr_create_from("shl");
    instruction_names[cur++] = dynstr_create_from("shr");
    instruction_names[cur++] = dynstr_create_from("str");
    instruction_names[cur++] = dynstr_create_from("ld");
    instruction_names[cur++] = dynstr_create_from("cmp");
    instruction_names[cur++] = dynstr_create_from("jmp");
    instruction_names[cur++] = dynstr_create_from("jne");
    instruction_names[cur++] = dynstr_create_from("je");
    instruction_names[cur++] = dynstr_create_from("jgt");
    instruction_names[cur++] = dynstr_create_from("jlt");
    instruction_names[cur++] = dynstr_create_from("jnz");
    instruction_names[cur++] = dynstr_create_from("jz");
    instruction_names[cur++] = dynstr_create_from("ptc");
    instruction_names[cur++] = dynstr_create_from("ptn");
    instruction_names[cur++] = dynstr_create_from("ptu");
    instruction_names[cur++] = dynstr_create_from("hlt");
    instruction_names[cur++] = dynstr_create_from("none");
    cur = 0;
    gp_register_names[cur++] = dynstr_create_from("r0");
    gp_register_names[cur++] = dynstr_create_from("r1");
    gp_register_names[cur++] = dynstr_create_from("r2");
    gp_register_names[cur++] = dynstr_create_from("r3");
    gp_register_names[cur++] = dynstr_create_from("r4");
    gp_register_names[cur++] = dynstr_create_from("r5");
    gp_register_names[cur++] = dynstr_create_from("r6");
    gp_register_names[cur++] = dynstr_create_from("r7");
    cur = 0;
    special_register_names[cur++] = dynstr_create_from("rst");
    special_register_names[cur++] = dynstr_create_from("rz");
    special_register_names[cur++] = dynstr_create_from("rcmp");
    special_register_names[cur++] = dynstr_create_from("pc");
    special_register_names[cur++] = dynstr_create_from("ip");
}

LexerState lexer_lex(str src) {
    if (!names_initialized)
        initialize_names();

    LexerState ls = {.src = src, .cur = -1, .line_num = 1, .instrs = {0}, .cur_instr = 0, 
                     .cur_operand = 0, .labels = {0}, .cur_label_index = 0,
                     .num_labels = 0, .had_error = false};
    while (!is_at_end(&ls)) {
        char c = next(&ls);
        switch (c) {
            // Ignore comments
            case ';': {
                ls.cur--;
                while ((c = next(&ls) != '\n'));
                ls.cur--;

                break;
            }
            case ',': {
                if (ls.cur_operand == 2)
                    ls.cur_operand = 0;
                else
                    ls.cur_operand++;

                break;
            }
            case '\n': {
                // Don't count labels as instructions
                int offset = -1;
                if (peek_n(&ls, offset) == '\r')
                    offset--;

                if (peek_n(&ls, offset) != ':') {
                    ls.cur_operand = 0;
                    ls.cur_instr++;
                }
                ls.line_num++;

                break;
            }
            default: {
                if (isdigit(c))
                    handle_number(&ls);
                else if (isalnum(c)) {
                    if (!handle_text(&ls)) {
                        ls.had_error = true;
                        break;
                    }
                }
            }
        }
    }
    return ls;
}