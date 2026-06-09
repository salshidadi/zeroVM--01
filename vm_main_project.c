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

#define OPCODE_JLOE 27
#define OPCODE_JGOE 28

#define OPCODE_AND 29
#define OPCODE_OR 30
#define OPCODE_XOR 31
#define OPCODE_NOT 32
#define OPCODE_SHL 33
#define OPCODE_SHR 34

#define OPCODE_PUSH 35
#define OPCODE_POP 36
#define OPCODE_PRINT_STACK 37

#define OPCODE_CALL 38
#define OPCODE_RETURN 39


//////////////////////
#define PROGRAM_SIZE sizeof(program)
#define INSTRUCTIONS_COUNT 40
#define INSTRUCTION_SIZE 3

#define LEFT_OPERAND IP + 1
#define RIGHT_OPERAND IP + 2

#define RX_COUNT 4

#define CODE_OFFSET 5
#define STACK_SIZE 5
#define STACK_BOUNDARY (PROGRAM_SIZE - STACK_SIZE)

static unsigned char program[] = {
    // Data
    2, 4, 6, 8, 9,
///////////////////////////////////
    // Code
    // /* 0 */ 16, 0,1,  /* LDM R0, 2 */
    // /* 3 */ 16, 1, 2,  /* LDM R1, 6 */
    // /* 0 */ 12, 0,1,  /* CMP R0, R1 */
    // /* 3 */ 28, 21, 0,  /* JMP 21   */

    // /* 0 */ 16, 0,0,  /* LDM R0, 2 */
    // /* 0 */ 16, 0,0,  /* LDM R0, 2 */
    // /* 0 */ 16, 0,0,  /* LDM R0, 2 */

    // /* 3 */ 16, 1, 2,  /* LDM R1, 6 */

   /* 0 */  35, 10,0,
   /* 3 */  38, 21, 0, //call
   /* 6 */  35, 20, 0,
   /* 9 */  35, 30, 0,
   /* 12 */  35, 40, 0,
   /* 15 */  35, 50, 0,
   /* 18 */  11, 33, 0, //jmp
   /* 21 */  37,0, 0, // called
  /* 24 */  0, 40, 16,
   /* 27 */  0, 50, 13,   
   /* 30 */  39, 0, 0, //return 
    /* 33 */  35, 30, 0,
    //////////////////////////////////////////
    //stack
    0,
    0,
    0,
    0,
    0
};

/* Registers */
static int IP = CODE_OFFSET;
static unsigned char IR[INSTRUCTION_SIZE] = {0, 0, 0};
static int OUTPUT = 0;
static int stack_ptr = PROGRAM_SIZE;

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
    IP = left_operand + CODE_OFFSET;
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
        IP = left_operand + CODE_OFFSET;
        return left_operand;
    }

    return 255;
}

int opcode_jl(unsigned char left_operand, unsigned char right_operand)
{

    if ((FLAGS & FLAG_NEGATIVE) == FLAG_NEGATIVE)
    {
        IP = left_operand + CODE_OFFSET;
        return left_operand;
    }

    return 255;
}

int opcode_jg(unsigned char left_operand, unsigned char right_operand)
{

    if ((FLAGS & FLAG_POSITIVE) == FLAG_POSITIVE)
    {
        IP = left_operand + CODE_OFFSET;
        return left_operand;
    }

    return 255;
}

int opcode_jloe(unsigned char left_operand, unsigned char right_operand)
{

    if ((FLAGS & FLAG_NEGATIVE) == FLAG_NEGATIVE || (FLAGS & FLAG_ZERO) == FLAG_ZERO)
    {
        IP = left_operand + CODE_OFFSET;
        return left_operand;
    }

    return 255;
}

int opcode_jgoe(unsigned char left_operand, unsigned char right_operand)
{
    if ((FLAGS & FLAG_POSITIVE) == FLAG_POSITIVE || (FLAGS & FLAG_ZERO) == FLAG_ZERO)
    {
        IP = left_operand + CODE_OFFSET;
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

int opcode_push(unsigned char left_operand, unsigned char right_operand)
{
    if(stack_ptr == STACK_BOUNDARY){
        printf("Stack is full\n");
        return left_operand;
    }
    stack_ptr--;
    program[stack_ptr] = left_operand;
    return left_operand;
}

int opcode_pop(unsigned char left_operand, unsigned char right_operand)
{
    unsigned char tmp = program[stack_ptr];
    program[stack_ptr] = 0;
    stack_ptr++;

    return tmp;
}

int opcode_print_stack(unsigned char left_operand, unsigned char right_operand){
    for(int i = STACK_BOUNDARY; i < PROGRAM_SIZE; i++){
        printf("Value at index: %d -> %d\n", i, program[i]);
    }
    return 255;
}

int opcode_call(unsigned char left_operand, unsigned char right_operand)
{
    opcode_push(IP, 0);
    IP  = left_operand + CODE_OFFSET;

    return IP;
}

int opcode_return(unsigned char left_operand, unsigned char right_operand)
{
    IP = opcode_pop(0,0);
    return IP;
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
    opcode_submi, opcode_mulmi, opcode_divmi,
    opcode_jloe, opcode_jgoe,opcode_and, 
    opcode_or , opcode_xor,opcode_not,
    opcode_shl, opcode_shr, opcode_push,
    opcode_pop, opcode_print_stack, opcode_call,
    opcode_return        
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
