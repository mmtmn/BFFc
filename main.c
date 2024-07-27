#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAPE_SIZE 64

typedef struct {
    int *tape;
    int head0;
    int head1;
    int instruction_pointer;
    int initial_tape_size;
} BFFInterpreter;

BFFInterpreter* create_interpreter(int tape_size) {
    BFFInterpreter* interpreter = (BFFInterpreter*)malloc(sizeof(BFFInterpreter));
    interpreter->initial_tape_size = tape_size;
    interpreter->tape = (int*)calloc(tape_size, sizeof(int));
    interpreter->head0 = 0;
    interpreter->head1 = 0;
    interpreter->instruction_pointer = 0;
    return interpreter;
}

void reset(BFFInterpreter* interpreter) {
    memset(interpreter->tape, 0, interpreter->initial_tape_size * sizeof(int));
    interpreter->head0 = 0;
    interpreter->head1 = 0;
    interpreter->instruction_pointer = 0;
}

void load_program(BFFInterpreter* interpreter, const char* program) {
    int tape_size = interpreter->initial_tape_size > strlen(program) ? interpreter->initial_tape_size : strlen(program);
    interpreter->tape = (int*)realloc(interpreter->tape, tape_size * sizeof(int));
    memset(interpreter->tape, 0, tape_size * sizeof(int));

    for (int i = 0; i < strlen(program); i++) {
        interpreter->tape[i] = program[i];
    }
}

void move_head0(BFFInterpreter* interpreter, int step) {
    interpreter->head0 = (interpreter->head0 + step) % interpreter->initial_tape_size;
}

void move_head1(BFFInterpreter* interpreter, int step) {
    interpreter->head1 = (interpreter->head1 + step) % interpreter->initial_tape_size;
}

void modify_tape(BFFInterpreter* interpreter, int value) {
    interpreter->tape[interpreter->head0] = (interpreter->tape[interpreter->head0] + value) % 256;
}

void copy_head0_to_head1(BFFInterpreter* interpreter) {
    interpreter->tape[interpreter->head1] = interpreter->tape[interpreter->head0];
}

void copy_head1_to_head0(BFFInterpreter* interpreter) {
    interpreter->tape[interpreter->head0] = interpreter->tape[interpreter->head1];
}

void jump_forward(BFFInterpreter* interpreter) {
    int open_brackets = 1;
    while (open_brackets != 0) {
        interpreter->instruction_pointer++;
        if (interpreter->instruction_pointer >= interpreter->initial_tape_size) break;
        if (interpreter->tape[interpreter->instruction_pointer] == '[') open_brackets++;
        else if (interpreter->tape[interpreter->instruction_pointer] == ']') open_brackets--;
    }
}

void jump_backward(BFFInterpreter* interpreter) {
    int open_brackets = 1;
    while (open_brackets != 0) {
        interpreter->instruction_pointer--;
        if (interpreter->instruction_pointer < 0) break;
        if (interpreter->tape[interpreter->instruction_pointer] == ']') open_brackets++;
        else if (interpreter->tape[interpreter->instruction_pointer] == '[') open_brackets--;
    }
}

void execute(BFFInterpreter* interpreter) {
    while (interpreter->instruction_pointer < interpreter->initial_tape_size) {
        char command = interpreter->tape[interpreter->instruction_pointer];
        switch (command) {
            case '>':
                move_head0(interpreter, 1);
                break;
            case '<':
                move_head0(interpreter, -1);
                break;
            case '}':
                move_head1(interpreter, 1);
                break;
            case '{':
                move_head1(interpreter, -1);
                break;
            case '+':
                modify_tape(interpreter, 1);
                break;
            case '-':
                modify_tape(interpreter, -1);
                break;
            case '.':
                copy_head0_to_head1(interpreter);
                break;
            case ',':
                copy_head1_to_head0(interpreter);
                break;
            case '[':
                if (interpreter->tape[interpreter->head0] == 0) jump_forward(interpreter);
                break;
            case ']':
                if (interpreter->tape[interpreter->head0] != 0) jump_backward(interpreter);
                break;
            default:
                break;
        }
        interpreter->instruction_pointer++;
    }
}

void free_interpreter(BFFInterpreter* interpreter) {
    free(interpreter->tape);
    free(interpreter);
}
