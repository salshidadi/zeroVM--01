#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef int (*opcode_function_t)(unsigned char, unsigned char);

#define OPCODE_ADD 0
#define OPCODE_SUB 1
#define OPCODE_MUL 2
#define OPCODE_DIV 3
#define OPCODE_MOD 4
#define OPCODE_STP 5
#define OPCODE_LDI 6
#define OPCODE_ADR 7
#define OPCODE_SUR 8
#define OPCODE_INC 9
#define OPCODE_DEC 10
#define OPCODE_JMP 11
#define OPCODE_CMP 12
#define OPCODE_JE 13
#define OPCODE_JZ 13
#define OPCODE_JL 14
#define OPCODE_JG 15

#define OPCODE_LDM 16
#define OPCODE_STI 17
#define OPCODE_STR 18

#define OPCODE_ADDMR 19
#define OPCODE_SUBMR 20
#define OPCODE_MULMR 21
#define OPCODE_DIVMR 22

#define OPCODE_ADDMI 23
#define OPCODE_SUBMI 24
#define OPCODE_MULMI 25
#define OPCODE_DIVMI 26

#define PROGRAM_SIZE sizeof(program)
#define INSTRUCTIONS_COUNT 27
#define INSTRUCTION_SIZE 3

#define LEFT_OPERAND IP + 1
#define RIGHT_OPERAND IP + 2

#define RX_COUNT 4

#define CODE_OFFSET 5

static unsigned char program[] = {
    2, 4, 6, 8, 9,

    /* 0 */ 16, 0, 10,  /* LDI R0, 10 */
    /* 3 */ 16, 1, 20,  /* LDI R1, 20 */
    /* 9 */ 19, 0, 0,  /* LDI R0, 70 */ //12
    /* 9 */ 20, 1, 2,  /* LDI R0, 70 */ //14
    /* 9 */ 21, 0, 0,  /* LDI R0, 70 */ //24
    /* 9 */ 22, 0, 3,  /* LDI R0, 70 */ //3
};

/* Registers */
static int IP = CODE_OFFSET;
static unsigned char IR[INSTRUCTION_SIZE] = {0, 0, 0};
static int OUTPUT = 0;

static unsigned char FLAGS = 0;

#define FLAG_ZERO 1
#define FLAG_NEGATIVE 2
#define FLAG_POSITIVE 4
#define FLAG_EVEN 8


/* 0000 0000 */
/* xxxx xxNZ */

static unsigned char R0 = 0;
static unsigned char R1 = 0;
static unsigned char R2 = 0;
static unsigned char R3 = 0;

static opcode_function_t current_instruction = NULL;

int opcode_add(unsigned char left_operand, unsigned char right_operand)
{
    return left_operand + right_operand;
}

int opcode_sub(unsigned char left_operand, unsigned char right_operand)
{
    return left_operand - right_operand;
}

int opcode_mul(unsigned char left_operand, unsigned char right_operand)
{
    return left_operand * right_operand;
}

int opcode_div(unsigned char left_operand, unsigned char right_operand)
{
    if (right_operand == 0)
    {
        printf("Exception: divide by zero\n");
        return false;
    }
    return left_operand / right_operand;
}

int opcode_mod(unsigned char left_operand, unsigned char right_operand)
{
    return left_operand % right_operand;
}

static unsigned char get_rx_value(unsigned char operand)
{
    switch (operand)
    {
    case 0:
        return R0;
    case 1:
        return R1;
    case 2:
        return R2;
    case 3:
        return R3;
        // skip default case
    }

    return 255;
}

static void set_rx_value(unsigned char operand, unsigned char value)
{
    switch (operand)
    {
    case 0:
        R0 = value;
        return;
    case 1:
        R1 = value;
        return;
    case 2:
        R2 = value;
        return;
    case 3:
        R3 = value;
        return;
        // skip default case
    }
}

int opcode_ldi(unsigned char left_operand, unsigned char right_operand)
{
    switch (left_operand)
    {
    case 0:
        R0 = right_operand;
        break;
    case 1:
        R1 = right_operand;
        break;
    case 2:
        R2 = right_operand;
        break;
    case 3:
        R3 = right_operand;
        break;
    default:
        printf("Invalid register address\n");
        exit(0);
    }

    return right_operand;
}

int opcode_ldm(unsigned char left_operand, unsigned char right_operand)
{
    switch (left_operand)
    {
    case 0:
        R0 = program[right_operand];
        break;
    case 1:
        R1 = program[right_operand];
        break;
    case 2:
        R2 = program[right_operand];
        break;
    case 3:
        R3 = program[right_operand];
        break;
    default:
        printf("Invalid register address\n");
        exit(0);
    }

    return right_operand;
}

int opcode_sti(unsigned char left_operand, unsigned char right_operand)
{
    program[left_operand] = right_operand;

    return right_operand;
}

int opcode_str(unsigned char left_operand, unsigned char right_operand)
{
    switch (left_operand)
    {
    case 0:
        program[right_operand] = R0;
        break;
    case 1:
        program[right_operand] = R1;
        break;
    case 2:
        program[right_operand] = R2;
        break;
    case 3:
        program[right_operand] = R3;
        break;
    default:
        printf("Invalid register address\n");
        exit(0);
    }
    return right_operand;
}

int opcode_stp(unsigned char left_operand, unsigned char right_operand)
{
    OUTPUT = left_operand;
    exit(left_operand);
}

int opcode_adr(unsigned char left_operand, unsigned char right_operand)
{
    if (left_operand >= RX_COUNT)
    {
        printf("Invlaid Rx register address\n");
        exit(0);
    }

    unsigned char Rx = get_rx_value(left_operand) + right_operand;
    set_rx_value(left_operand, Rx);
    return Rx;
}

int opcode_sur(unsigned char left_operand, unsigned char right_operand)
{
    if (left_operand >= RX_COUNT)
    {
        printf("Invlaid Rx register address\n");
        exit(0);
    }

    unsigned char Rx = get_rx_value(left_operand) - right_operand;
    set_rx_value(left_operand, Rx);
    return Rx;
}

int opcode_inc(unsigned char left_operand, unsigned char right_operand)
{
    if (left_operand >= RX_COUNT)
    {
        printf("Invlaid Rx register address\n");
        exit(0);
    }

    unsigned char Rx = get_rx_value(left_operand);
    Rx++;
    set_rx_value(left_operand, Rx);
    return Rx;
}

int opcode_dec(unsigned char left_operand, unsigned char right_operand)
{
    if (left_operand >= RX_COUNT)
    {
        printf("Invlaid Rx register address\n");
        exit(0);
    }

    unsigned char Rx = get_rx_value(left_operand);
    Rx--;
    set_rx_value(left_operand, Rx);
    return Rx;
}

int opcode_jmp(unsigned char left_operand, unsigned char right_operand)
{
    IP = left_operand;
    return left_operand;
}

int opcode_cmp(unsigned char left_operand, unsigned char right_operand)
{
    unsigned char Rx = get_rx_value(left_operand);
    unsigned char Ry = get_rx_value(right_operand);

    OUTPUT = Rx - Ry;
    FLAGS = FLAG_POSITIVE;

    if (OUTPUT == 0)
    {
        FLAGS = FLAG_ZERO;
    }
    else if (OUTPUT < 0)
    {
        FLAGS = FLAG_NEGATIVE;
    }

    if(OUTPUT % 2 == 0){
        FLAGS |= FLAG_EVEN;
    }

    return FLAGS;
}

int opcode_je(unsigned char left_operand, unsigned char right_operand)
{

    if ((FLAGS & FLAG_ZERO) == FLAG_ZERO)
    {
        IP = left_operand;
        return left_operand;
    }

    return 255;
}

int opcode_jl(unsigned char left_operand, unsigned char right_operand)
{

    if ((FLAGS & FLAG_NEGATIVE) == FLAG_NEGATIVE)
    {
        IP = left_operand;
        return left_operand;
    }

    return 255;
}

int opcode_jg(unsigned char left_operand, unsigned char right_operand)
{

    if ((FLAGS & FLAG_POSITIVE) == FLAG_POSITIVE)
    {
        IP = left_operand;
        return left_operand;
    }

    return 255;
}

int opcode_addmr(unsigned char left_operand, unsigned char right_operand)
{
    if (left_operand >= RX_COUNT)
    {
        printf("Invlaid Rx register address\n");
        exit(0);
    }

    unsigned char Rx = get_rx_value(left_operand) + program[right_operand];
    set_rx_value(left_operand, Rx);
    return Rx;
}

int opcode_submr(unsigned char left_operand, unsigned char right_operand)
{
    if (left_operand >= RX_COUNT)
    {
        printf("Invlaid Rx register address\n");
        exit(0);
    }

    unsigned char Rx = get_rx_value(left_operand) - program[right_operand];
    set_rx_value(left_operand, Rx);
    return Rx;
}

int opcode_mulmr(unsigned char left_operand, unsigned char right_operand)
{
    if (left_operand >= RX_COUNT)
    {
        printf("Invlaid Rx register address\n");
        exit(0);
    }

    unsigned char Rx = get_rx_value(left_operand) * program[right_operand];
    set_rx_value(left_operand, Rx);
    return Rx;
}

int opcode_divmr(unsigned char left_operand, unsigned char right_operand)
{
    if (left_operand >= RX_COUNT)
    {
        printf("Invlaid Rx register address\n");
        exit(0);
    }
    if (program[right_operand] == 0)
    {
        printf("Exception: divide by zero\n");
        return 1;
    }

    unsigned char Rx = get_rx_value(left_operand) / program[right_operand];
    set_rx_value(left_operand, Rx);
    return Rx;
}

int opcode_addmi(unsigned char left_operand, unsigned char right_operand)
{
    program[left_operand] = program[left_operand] + right_operand;
    OUTPUT = program[left_operand];
    return OUTPUT;
}

int opcode_submi(unsigned char left_operand, unsigned char right_operand)
{
    program[left_operand] = program[left_operand] - right_operand;
    OUTPUT = program[left_operand];
    return OUTPUT;
}

int opcode_mulmi(unsigned char left_operand, unsigned char right_operand)
{
    program[left_operand] = program[left_operand] * right_operand;
    OUTPUT = program[left_operand];
    return OUTPUT;
}

int opcode_divmi(unsigned char left_operand, unsigned char right_operand)
{
    if(right_operand == 0){
        printf("Expetion: divide by zero\n");
        return 1;
    }
    program[left_operand] = program[left_operand] / right_operand;
    OUTPUT = program[left_operand];
    return OUTPUT;
}

static const opcode_function_t opcode_functions[INSTRUCTIONS_COUNT] = {
    opcode_add, opcode_sub, opcode_mul,
    opcode_div, opcode_mod, opcode_stp,
    opcode_ldi, opcode_adr, opcode_sur,
    opcode_inc, opcode_dec, opcode_jmp,
    opcode_cmp, opcode_je, opcode_jl,
    opcode_jg, opcode_ldm, opcode_sti,
    opcode_str, opcode_addmr, opcode_submr,
    opcode_mulmr, opcode_divmr, opcode_addmi,
    opcode_submi, opcode_mulmi, opcode_divmi        
};

static bool cpu_fetch(void)
{
    int byte = 0;
    if (IP >= PROGRAM_SIZE)
    {
        /* HALT */
        exit(0);
    }

    /* Fetch */
    IR[0] = program[IP];
    IR[1] = program[LEFT_OPERAND];
    IR[2] = program[RIGHT_OPERAND];

    /* Move to the next insturction */
    IP += INSTRUCTION_SIZE;

    return true;
}

static bool cpu_decode(void)
{
    if (IR[0] >= INSTRUCTIONS_COUNT)
    {
        printf("Invalid instruction\n");
        // IR[0] = 10; // rewrite
        IR[0] = IR[1] = IR[2] = 0;
        exit(0);
    }

    current_instruction = opcode_functions[IR[0]];

    return true;
}

static bool cpu_execute(void)
{
    OUTPUT = current_instruction(IR[1], IR[2]);
    return true;
}

int main(void)
{
    // reset
    IP = CODE_OFFSET;
    IR[0] = IR[1] = IR[2] = 0;

    while (true)
    {
        cpu_fetch();
        cpu_decode();
        cpu_execute();
        printf("output -> %d\n", OUTPUT);
    }

    return 0;
}