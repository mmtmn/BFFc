#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bff_interpreter.h"

#define NUM_PROGRAMS 128
#define TAPE_SIZE 64

char *random_program(int length) {
    char *program = (char *)malloc(length + 1);
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < length; i++) {
        int key = rand() % (int)(sizeof(charset) - 1);
        program[i] = charset[key];
    }
    program[length] = '\0';
    return program;
}

void run_interaction(BFFInterpreter *interpreter, char **programs, int index_a, int index_b) {
    char *program_a = programs[index_a];
    char *program_b = programs[index_b];
    char concatenated[TAPE_SIZE * 2];
    snprintf(concatenated, sizeof(concatenated), "%s%s", program_a, program_b);

    bff_reset(interpreter);
    bff_load_program(interpreter, concatenated);
    bff_execute(interpreter);

    int split_point = strlen(concatenated) / 2;
    strncpy(program_a, (char *)interpreter->tape, split_point);
    strncpy(program_b, (char *)interpreter->tape + split_point, TAPE_SIZE);
}

void mutate(char **programs, int num_programs, int tape_size) {
    for (int i = 0; i < num_programs; i++) {
        if ((rand() / (double)RAND_MAX) < 0.05) {
            int index = rand() % tape_size;
            programs[i][index] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"[rand() % 62];
        }
    }
}

void print_programs(char **programs, int num_programs) {
    for (int i = 0; i < num_programs; i++) {
        printf("%s\n", programs[i]);
    }
}

int main() {
    srand(time(NULL));

    char *programs[NUM_PROGRAMS];
    for (int i = 0; i < NUM_PROGRAMS; i++) {
        programs[i] = random_program(TAPE_SIZE);
    }

    BFFInterpreter interpreter;
    bff_init(&interpreter);

    for (int epoch = 0; epoch < 1000; epoch++) {
        for (int i = 0; i < NUM_PROGRAMS; i++) {
            for (int j = i + 1; j < NUM_PROGRAMS; j++) {
                run_interaction(&interpreter, programs, i, j);
            }
        }
        mutate(programs, NUM_PROGRAMS, TAPE_SIZE);
        printf("Epoch %d\n", epoch);
    }

    print_programs(programs, NUM_PROGRAMS);

    for (int i = 0; i < NUM_PROGRAMS; i++) {
        free(programs[i]);
    }

    return 0;
}
