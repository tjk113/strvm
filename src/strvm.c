#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "strvm.h"

static Register* get_operand_register(VM* vm, Operand op) {
    if (op.value < NUM_GP_REGISTERS) {
        if (op.is_register && !op.is_label)
            return &vm->registers[op.value];
    }
    else if (op.value >= NUM_GP_REGISTERS + NUM_SPECIAL_REGISTERS) {
        switch (op.value) {
            case NUM_GP_REGISTERS:
                return (Register*)&vm->status_register;
            case NUM_GP_REGISTERS+1:
                return (Register*)&vm->compare_register;
            case NUM_GP_REGISTERS+2:
                return &vm->zero_register;
        }
    }
    return NULL;
}

/* This function returns a signed 16-bit integer,
even though the VM's registers hold unsigned 8-bit
integers. This is so that it can return a unique
error code (-1), while also preserving the full
range of an unsigned 8-bit integer for values. */
static int16_t get_operand_value(VM* vm, Operand op) {
    // Operand is a register
    if (op.is_register)
        return vm->registers[op.value].value;
    // Operand is an immediate
    else if (!op.is_label)
        return op.value;

    return -1;
}

static VM_Error execute_instruction(VM* vm, Instruction instr) {
    switch (instr.type) {
        case NOP: {
            break;
        }
        case MOV: {
            Register* dst = get_operand_register(vm, instr.operands[0]);
            if (dst == NULL)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};
            int16_t value = get_operand_value(vm, instr.operands[1]);
            if (value == -1)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            dst->value = value;

            break;
        }
        case ADD: case ADC: case SUB: case MUL: case DIV: case SHL: case SHR: {
            Register* dst = get_operand_register(vm, instr.operands[0]);
            if (dst == NULL)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};
            int16_t value = get_operand_value(vm, instr.operands[1]);
            if (value == -1)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};
            int test = 0;
            switch (instr.type) {
                case ADD: dst->value += value; break;
                case ADC: {
                    dst->value |= vm->status_register.carry;
                    // Kind of cheating to detect carry/overflow
                    if (dst->value + value > UINT8_MAX)
                        vm->status_register.carry = 1;
                    if (dst->value + value > INT8_MAX)
                        vm->status_register.overflow = 1;
                    dst->value += value;
                    break;
                }
                case SUB: dst->value -= value; break;
                case SBC: {
                    dst->value |= -vm->status_register.carry;
                    if (dst->value - value > UINT8_MAX)
                        vm->status_register.carry = 1;
                    if (dst->value - value < INT8_MIN || dst->value - value > INT8_MAX)
                        vm->status_register.overflow = 1;
                    dst->value -= value;

                    break;
                }
                case MUL: dst->value *= value; break;
                case DIV: dst->value /= value; break;
                case SHL: dst->value <<= value; break;
                case SHR: dst->value >>= value; break;
            }

            vm->status_register.not_zero = dst->value != 0;

            break;
        }
        case CLC: {
            vm->status_register.carry = 0;

            break;
        }
        case CLV: {
            vm->status_register.overflow = 0;

            break;
        }
        case NEG: {
            if (instr.operands[1].is_register)
                vm->registers[instr.operands[1].value].value = -vm->registers[instr.operands[1].value].value;
            else
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            break;
        }
        case STR: {
            uint8_t address = get_operand_value(vm, instr.operands[0]);
            if (address == -1)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};
            int16_t value = get_operand_value(vm, instr.operands[1]);
            if (value == -1)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            vm->memory[address] = value;

            break;
        }
        case LD: {
            Register* dst = get_operand_register(vm, instr.operands[0]);
            if (dst == NULL)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};
            int16_t value = get_operand_value(vm, instr.operands[1]);
            if (value == -1)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            dst->value = vm->memory[value];

            break;
        }
        case CMP: {
            int16_t value_1 = get_operand_value(vm, instr.operands[0]);
            if (value_1 == -1)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};
            int16_t value_2 = get_operand_value(vm, instr.operands[1]);
            if (value_2 == -1)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            memset((uint8_t*)&vm->compare_register, 0, 1);
            if (value_1 != value_2)
                vm->compare_register.not_equal = true;
            if (value_1 == value_2)
                vm->compare_register.equal = true;
            if (value_1 > value_2)
                vm->compare_register.greater_than = true;
            if (value_1 < value_2)
                vm->compare_register.less_than = true;

            break;
        }
        case JMP: {
            if (!instr.operands[0].is_label)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            vm->instr_ptr = vm->labels[instr.operands[0].value].address - 1;

            break;
        }
        case JNE: {
            if (!instr.operands[0].is_label)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            if (vm->compare_register.not_equal)
                vm->instr_ptr = vm->labels[instr.operands[0].value].address - 1;

            break;
        }
        case JE: {
            if (!instr.operands[0].is_label)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            if (vm->compare_register.equal)
                vm->instr_ptr = vm->labels[instr.operands[0].value].address - 1;

            break;
        }
        case JGT: {
            if (!instr.operands[0].is_label)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            if (vm->compare_register.greater_than)
                vm->instr_ptr = vm->labels[instr.operands[0].value].address - 1;

            break;
        }
        case JLT: {
            if (!instr.operands[0].is_label)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            if (vm->compare_register.less_than)
                vm->instr_ptr = vm->labels[instr.operands[0].value].address - 1;

            break;
        }
        case JNZ: case JZ: {
            if (!instr.operands[0].is_label)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            if ((vm->status_register.not_zero && instr.type == JNZ)
                || (!vm->status_register.not_zero && instr.type == JZ))
            {
                vm->instr_ptr = vm->labels[instr.operands[0].value].address - 1;
            }

            break;
        }
        case PTC: {
            int16_t value = get_operand_value(vm, instr.operands[0]);
            if (value == -1)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            printf("%c", (uint8_t)value);
            
            break;
        }
        case PTN: case PTU: {
            int16_t value = get_operand_value(vm, instr.operands[0]);
            if (value == -1)
                return (VM_Error){.type = INVALID_OPERAND, .operands = instr.operands};

            printf(instr.type == PTN ? "%d" : "%u", value);
            
            break;
        }
        case HLT: {
            return (VM_Error){.type = HALT};
        }
        default: {
            // TODO: VM_Errors need to hold more descriptive data
            if (instr.type >= NUM_INSTR_TYPES || instr.type < 0)
                return (VM_Error){.type = INVALID_INSTRUCTION};
        }
    }
    return (VM_Error){.type = NONE};
}

VM vm_init(Label labels[MAX_LABELS], int num_labels) {
    VM vm = {0};
    memcpy(&vm.labels, labels, sizeof(Label[MAX_LABELS]));
    vm.num_labels = num_labels;
    return vm;
}

VM_Error vm_run(VM* vm, Instruction instrs[MAX_INSTRS], uint8_t num_instrs) {
    VM_Error error = {.type = NONE};
    for (; vm->instr_ptr < num_instrs; vm->instr_ptr++, vm->program_counter++) {
        error = execute_instruction(vm, instrs[vm->instr_ptr]);
        if (error.type != NONE || error.type == HALT)
            break;
    }
    return error;
}

void vm_print_state(VM vm) {
    for (int i = 0; i < NUM_GP_REGISTERS; i++)
        printf("r%d  : %d\n", i, vm.registers[i].value);
    printf("rst :\n");
    printf("   c: %d\n", vm.status_register.carry);
    printf("   v: %d\n", vm.status_register.overflow);
    printf("  nz: %s\n", vm.status_register.not_zero ? "true" : "false");
    printf("rcmp:\n");
    printf("  ne: %s\n", vm.compare_register.not_equal ? "true" : "false");
    printf("  e : %s\n", vm.compare_register.equal ? "true" : "false");
    printf("  gt: %s\n", vm.compare_register.greater_than ? "true" : "false");
    printf("  lt: %s\n", vm.compare_register.less_than ? "true" : "false");
    printf("rz  : %d\n", vm.zero_register.value);
    printf("pc  : %d\n", vm.program_counter);
    printf("ip  : %d\n", vm.instr_ptr);
}