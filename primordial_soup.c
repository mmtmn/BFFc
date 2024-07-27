#include <GL/glew.h>
#include <GLFW/glfw3.h>
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

void print_grid(char **programs, int num_programs) {
    int side_length = (int)ceil(sqrt(num_programs));
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < side_length; i++) {
        for (int j = 0; j < side_length; j++) {
            int index = i * side_length + j;
            if (index < num_programs) {
                char *program = programs[index];
                for (int k = 0; k < TAPE_SIZE; k++) {
                    float x = (float)j / side_length * 2 - 1;
                    float y = (float)i / side_length * 2 - 1;
                    float size = 2.0 / side_length;

                    float intensity = (float)program[k] / 255.0;
                    glColor3f(intensity, intensity, intensity);
                    glBegin(GL_QUADS);
                    glVertex2f(x, y);
                    glVertex2f(x + size, y);
                    glVertex2f(x + size, y + size);
                    glVertex2f(x, y + size);
                    glEnd();
                }
            }
        }
    }
    glfwSwapBuffers(glfwGetCurrentContext());
}

int main() {
    srand(time(NULL));

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 800, "Primordial Soup", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    char *programs[NUM_PROGRAMS];
    for (int i = 0; i < NUM_PROGRAMS; i++) {
        programs[i] = random_program(TAPE_SIZE);
    }

    BFFInterpreter interpreter;
    bff_init(&interpreter);

    while (!glfwWindowShouldClose(window)) {
        for (int epoch = 0; epoch < 1000; epoch++) {
            for (int i = 0; i < NUM_PROGRAMS; i++) {
                for (int j = i + 1; j < NUM_PROGRAMS; j++) {
                    run_interaction(&interpreter, programs, i, j);
                }
            }
            mutate(programs, NUM_PROGRAMS, TAPE_SIZE);

            // Print grid to OpenGL window
            print_grid(programs, NUM_PROGRAMS);

            glfwPollEvents();
        }
    }

    for (int i = 0; i < NUM_PROGRAMS; i++) {
        free(programs[i]);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
