#include <stdbool.h>
#include <stdio.h>

#include "strvm.h"
#include "lexer.h"

#include "fiesta/str.h"

str read_file_to_str(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
        return (str){.data = NULL, .len = 0};
    
    fseek(file, 0, SEEK_END);
    int file_len = ftell(file);
    rewind(file);

    str string = str_create(file_len);

    int bytes_read = 0;
    bytes_read = fread(string.data, sizeof(uint8_t), file_len, file);
    fclose(file);

    str_to_lower(&string);

    return string;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Error: no source files provided\n");
        return 1;
    }

    str src = read_file_to_str(argv[1]);
    LexerState lexed = lexer_lex(src);
    if (lexed.had_error) {
        printf("Error: file \"%s\" couldn't be parsed\n", argv[1]);
        return 1;
    }

    VM vm = vm_init(lexed.labels, lexed.num_labels);
    VM_Error vm_result = vm_run(&vm, lexed.instrs, lexed.cur_instr + 1);
    if (vm_result.type != NONE && vm_result.type != HALT) {
        switch (vm_result.type) {
            case INVALID_INSTRUCTION:
                printf("Error: invalid instruction\n");
                break;
            case INVALID_OPERAND:
                printf("Error: invalid operand\n");
                break;
            default:
                printf("Error: an unknown error occurred\n");
        }
        return 1;
    }

    return 0;
}