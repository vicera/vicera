#ifndef CPU_H
#define CPU_H

// types definitions
typedef unsigned char BYTE;
typedef unsigned short int WORD;

// Argument types
typedef enum {
    ARG_RN,
    ARG_RR,
    ARG_PR,
    ARG_RP
} Args;

// Misc
#define Z_FLAG  0x01
#define C_FLAG  0x02

// Instructions
typedef enum {
    HALT,
    NOP,
    
    // Stack operations
    PUSH,   // Pushes and Pops from HL
    POP,

    // MOV operations
    // R = Register, N = Byte, P = (HL)
    
    // mov a, *
    MOV_AA, MOV_AB, MOV_AC, MOV_AD, MOV_AE, MOV_AH, MOV_AL,
    // mov b, *
    MOV_BA, MOV_BB, MOV_BC, MOV_BD, MOV_BE, MOV_BH, MOV_BL,
    // mov c, *
    MOV_CA, MOV_CB, MOV_CC, MOV_CD, MOV_CE, MOV_CH, MOV_CL,
    // mov d, *
    MOV_DA, MOV_DB, MOV_DC, MOV_DD, MOV_DE, MOV_DH, MOV_DL,
    // mov e, *
    MOV_EA, MOV_EB, MOV_EC, MOV_ED, MOV_EE, MOV_EH, MOV_EL,
    // mov h, *
    MOV_HA, MOV_HB, MOV_HC, MOV_HD, MOV_HE, MOV_HH, MOV_HL,
    // mov l, *
    MOV_LA, MOV_LB, MOV_LC, MOV_LD, MOV_LE, MOV_LH, MOV_LL,
    // mov (HL), *
    MOV_PA, MOV_PB, MOV_PC, MOV_PD, MOV_PE, MOV_PH, MOV_PL,

    // mov r, n
    MOV_AN, MOV_BN, MOV_CN, MOV_DN, MOV_EN, MOV_HN, MOV_LN,
    // mov r, (HL)
    MOV_AP, MOV_BP, MOV_CP, MOV_DP, MOV_EP, MOV_HP, MOV_LP,

    // ALU operations
    // ADD
    ADD_A, ADD_B, ADD_C, ADD_D, ADD_E, ADD_H, ADD_L, ADD_N, ADD_P,

    // SUB
    SUB_A, SUB_B, SUB_C, SUB_D, SUB_E, SUB_H, SUB_L, SUB_N, SUB_P,

    // AND
    AND_A, AND_B, AND_C, AND_D, AND_E, AND_H, AND_L, AND_N, AND_P,

    // OR
    OR_A, OR_B, OR_C, OR_D, OR_E, OR_H, OR_L, OR_N, OR_P,

    // XOR
    XOR_A, XOR_B, XOR_C, XOR_D, XOR_E, XOR_H, XOR_L, XOR_N, XOR_P,

    // INC
    INC_A, INC_B, INC_C, INC_D, INC_E, INC_H, INC_L, 
    // DEC
    DEC_A, DEC_B, DEC_C, DEC_D, DEC_E, DEC_H, DEC_L, 

    // SL (Shift Left)
    SL_A, SL_B, SL_C, SL_D, SL_E, SL_H, SL_L, 
    // SR (Shift Right)
    SR_A, SR_B, SR_C, SR_D, SR_E, SR_H, SR_L, 
    
    // Comparision
    CP_A, CP_B, CP_C, CP_D, CP_E, CP_H, CP_L, CP_N, CP_P,

    // Jumps
    // Jump
    JP_NN,
    JP_P,

    // Jump if Carry
    JC_NN,
    JC_P,

    // Jump if Zero
    JZ_NN,
    JZ_P,

    // Jump if not Zero
    JN_NN,
    JN_P,

    // Call
    CALL_NN,
    CALL_P,
    
    // Return
    RET,

    // Debug dump instructions
    // Dump registers
    DUMP_R,
    // Dump memory
    DUMP_M

} Instructions;

// Registers
typedef enum {
    REG_A,
    REG_B,
    REG_C,
    REG_D,
    REG_E,
    REG_H,
    REG_L,
    REGSIZE,
    REG_HL, // (HL)
} Registers;

// useful structs
struct CPU {
    // 64kb Memory bank.
    BYTE    memory[0x10000];
    // Registers
    BYTE    registers[REGSIZE];
    // Program Counter
    WORD    pc;
    // Stack Pointer
    WORD    sp;
    // Flags
    BYTE    flags;
    // Is it running?
    BYTE    running;
};

// Functions
void run(struct CPU* cpu);
void execute(struct CPU* cpu);
void init_cpu(struct CPU* cpu);

#endif
