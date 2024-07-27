#include "bff_interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void bff_init(BFFInterpreter *interpreter) {
    interpreter->head0 = 0;
    interpreter->head1 = 0;
    interpreter->instruction_pointer = 0;
    memset(interpreter->tape, 0, sizeof(interpreter->tape));
}

void bff_reset(BFFInterpreter *interpreter) {
    interpreter->head0 = 0;
    interpreter->head1 = 0;
    interpreter->instruction_pointer = 0;
    memset(interpreter->tape, 0, sizeof(interpreter->tape));
}

void bff_load_program(BFFInterpreter *interpreter, const char *program) {
    int i;
    for (i = 0; i < TAPE_SIZE && program[i] != '\0'; i++) {
        interpreter->tape[i] = (int)program[i];
    }
}

void bff_execute(BFFInterpreter *interpreter) {
    while (interpreter->instruction_pointer < TAPE_SIZE) {
        char command = (char)interpreter->tape[interpreter->instruction_pointer];
        switch (command) {
            case '>':
                interpreter->head0 = (interpreter->head0 + 1) % TAPE_SIZE;
                break;
            case '<':
                interpreter->head0 = (interpreter->head0 - 1 + TAPE_SIZE) % TAPE_SIZE;
                break;
            case '}':
                interpreter->head1 = (interpreter->head1 + 1) % TAPE_SIZE;
                break;
            case '{':
                interpreter->head1 = (interpreter->head1 - 1 + TAPE_SIZE) % TAPE_SIZE;
                break;
            case '+':
                interpreter->tape[interpreter->head0] = (interpreter->tape[interpreter->head0] + 1) % 256;
                break;
            case '-':
                interpreter->tape[interpreter->head0] = (interpreter->tape[interpreter->head0] - 1 + 256) % 256;
                break;
            case '.':
                interpreter->tape[interpreter->head1] = interpreter->tape[interpreter->head0];
                break;
            case ',':
                interpreter->tape[interpreter->head0] = interpreter->tape[interpreter->head1];
                break;
            case '[':
                if (interpreter->tape[interpreter->head0] == 0) {
                    int open_brackets = 1;
                    while (open_brackets != 0) {
                        interpreter->instruction_pointer++;
                        if (interpreter->instruction_pointer >= TAPE_SIZE) break;
                        if ((char)interpreter->tape[interpreter->instruction_pointer] == '[') open_brackets++;
                        if ((char)interpreter->tape[interpreter->instruction_pointer] == ']') open_brackets--;
                    }
                }
                break;
            case ']':
                if (interpreter->tape[interpreter->head0] != 0) {
                    int open_brackets = 1;
                    while (open_brackets != 0) {
                        interpreter->instruction_pointer--;
                        if (interpreter->instruction_pointer < 0) break;
                        if ((char)interpreter->tape[interpreter->instruction_pointer] == ']') open_brackets++;
                        if ((char)interpreter->tape[interpreter->instruction_pointer] == '[') open_brackets--;
                    }
                }
                break;
            default:
                break;
        }
        interpreter->instruction_pointer++;
    }
}
