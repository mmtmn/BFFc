#ifndef BFF_INTERPRETER_H
#define BFF_INTERPRETER_H

#define TAPE_SIZE 64

typedef struct {
    int tape[TAPE_SIZE];
    int head0;
    int head1;
    int instruction_pointer;
} BFFInterpreter;

void bff_init(BFFInterpreter *interpreter);
void bff_reset(BFFInterpreter *interpreter);
void bff_load_program(BFFInterpreter *interpreter, const char *program);
void bff_execute(BFFInterpreter *interpreter);

#endif
