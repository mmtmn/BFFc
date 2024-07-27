#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
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

double calculate_high_order_entropy(char **programs, int num_programs, int tape_size) {
    int total_length = num_programs * tape_size;
    int freq[256] = {0};
    double shannon_entropy = 0.0, kolmogorov_complexity = 0.0;

    for (int i = 0; i < num_programs; i++) {
        for (int j = 0; j < tape_size; j++) {
            freq[(unsigned char)programs[i][j]]++;
        }
    }

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            double p = (double)freq[i] / total_length;
            shannon_entropy -= p * log2(p);
        }
    }

    for (int i = 0; i < num_programs; i++) {
        kolmogorov_complexity += (double)strlen(programs[i]) / tape_size;
    }

    double normalized_kolmogorov = kolmogorov_complexity / num_programs;
    return shannon_entropy - normalized_kolmogorov;
}

void print_grid(char **programs, int num_programs) {
    int side_length = (int)ceil(sqrt(num_programs));
    for (int i = 0; i < side_length; i++) {
        for (int j = 0; j < side_length; j++) {
            int index = i * side_length + j;
            if (index < num_programs) {
                char *program = programs[index];
                for (int k = 0; k < TAPE_SIZE; k++) {
                    if (program[k] < 64) {
                        printf(".");
                    } else if (program[k] < 128) {
                        printf(":");
                    } else if (program[k] < 192) {
                        printf("o");
                    } else {
                        printf("@");
                    }
                }
            } else {
                for (int k = 0; k < TAPE_SIZE; k++) {
                    printf(" ");
                }
            }
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

int main() {
    srand(time(NULL));

    char *programs[NUM_PROGRAMS];
    for (int i = 0; i < NUM_PROGRAMS; i++) {
        programs[i] = random_program(TAPE_SIZE);
    }

    BFFInterpreter interpreter;
    bff_init(&interpreter);

    FILE *data_file = fopen("complexity_data.txt", "w");
    if (!data_file) {
        perror("Failed to open file");
        return 1;
    }

    for (int epoch = 0; epoch < 10000; epoch++) {
        for (int i = 0; i < NUM_PROGRAMS; i++) {
            for (int j = i + 1; j < NUM_PROGRAMS; j++) {
                run_interaction(&interpreter, programs, i, j);
            }
        }
        mutate(programs, NUM_PROGRAMS, TAPE_SIZE);
        double complexity = calculate_high_order_entropy(programs, NUM_PROGRAMS, TAPE_SIZE);
        //fprintf(data_file, "%d %f\n", epoch, complexity);
        //printf("Epoch: %d, Complexity: %f\n", epoch, complexity);

        // Print grid to terminal
        // printf("Epoch %d:\n", epoch);
        print_grid(programs, NUM_PROGRAMS);
        // printf("\n");
    }

    fclose(data_file);

    // print_programs(programs, NUM_PROGRAMS);

    for (int i = 0; i < NUM_PROGRAMS; i++) {
        free(programs[i]);
    }

    return 0;
}
