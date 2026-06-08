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
#define OPCODE_SBR 8
#define OPCODE_MLR 9
#define OPCODE_DVR 10
#define OPCODE_MDR 11
#define OPCODE_AND 12
#define OPCODE_OR 13
#define OPCODE_XOR 14
#define OPCODE_NOT 15
#define OPCODE_SHL 16
#define OPCODE_SHR 17

#define PROGRAM_SIZE sizeof(program)
#define INSTRUCTIONS_COUNT 18
#define INSTRUCTION_SIZE 3

#define LEFT_OPERAND IP + 1
#define RIGHT_OPERAND IP + 2

#define RX_COUNT 4

// static unsigned char program[] = {
//     0, 9, 7,  /* ADD 9, 7   */
//     1, 8, 4,  /* SUB 8, 4   */
//     6, 2, 77, /* LDI R2, 77 */
//     7, 2, 23, /* ADR R2, 23 */
//     2, 3, 6,  /* MUL 3, 6   */
//     3, 9, 3,  /* DIV 9, 3   */
//     4, 7, 2   /* MOD 7, 2   */
// };

static unsigned char program[] = {
    6, 0, 10,
    8, 0, 7,  
    9, 0, 4,  
    10, 0, 5, 
    11, 0, 4, 
    12, 3, 2,  
    13, 5, 3,  
    14, 7, 2,
    15, 7, 2,
    16, 7, 2,    
    17, 7, 2      
};

/* Registers */
static int IP = 0;
static unsigned char IR[INSTRUCTION_SIZE] = {0, 0, 0};
static int OUTPUT = 0;

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

    unsigned char Rx = get_rx_value(left_operand);

    return Rx + right_operand;
}

int opcode_sbr(unsigned char left_operand, unsigned char right_operand)
{
    if (left_operand >= RX_COUNT)
    {
        printf("Invlaid Rx register address\n");
        exit(0);
    }

    unsigned char Rx = get_rx_value(left_operand);

    return Rx - right_operand;
}

int opcode_mlr(unsigned char left_operand, unsigned char right_operand)
{
    if (left_operand >= RX_COUNT)
    {
        printf("Invlaid Rx register address\n");
        exit(0);
    }

    unsigned char Rx = get_rx_value(left_operand);

    return Rx * right_operand;
}

int opcode_dvr(unsigned char left_operand, unsigned char right_operand)
{
    if (left_operand >= RX_COUNT)
    {
        printf("Invlaid Rx register address\n");
        exit(0);
    }
    if (right_operand == 0)
    {
        printf("Exception: divide by zero\n");
        return false;
    }

    unsigned char Rx = get_rx_value(left_operand);

    return Rx / right_operand;
}

int opcode_mdr(unsigned char left_operand, unsigned char right_operand)
{
    if (left_operand >= RX_COUNT)
    {
        printf("Invlaid Rx register address\n");
        exit(0);
    }

    unsigned char Rx = get_rx_value(left_operand);

    return Rx % right_operand;
}

int opcode_and(unsigned char left_operand, unsigned char right_operand)
{
    return left_operand & right_operand;
}

int opcode_or(unsigned char left_operand, unsigned char right_operand)
{
    return left_operand | right_operand;
}

int opcode_xor(unsigned char left_operand, unsigned char right_operand)
{
    return left_operand ^ right_operand;
}

int opcode_not(unsigned char left_operand, unsigned char right_operand)
{
    return ~left_operand;
}

int opcode_shl(unsigned char left_operand, unsigned char right_operand)
{
    return left_operand << right_operand;
}

int opcode_shr(unsigned char left_operand, unsigned char right_operand)
{
    return left_operand >> right_operand;
}

static const opcode_function_t opcode_functions[INSTRUCTIONS_COUNT] = {
    opcode_add, opcode_sub, opcode_mul,
    opcode_div, opcode_mod, opcode_stp,
    opcode_ldi, opcode_adr, opcode_sbr,
    opcode_mlr, opcode_dvr, opcode_mdr,
    opcode_and, opcode_or , opcode_xor,
    opcode_not, opcode_shl, opcode_shr
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
    IP = 0;
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