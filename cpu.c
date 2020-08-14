/*
 * VICERA by h34ting4ppliance
 *
 * This is the CPU part of
 * the project.
 */
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "logging.h"

#define FNAME   "cpu.c"
#define Z_FLAG  0x01
#define C_FLAG  0x02

#define JC      0
#define JZ      1
#define JNZ     2

/*
 * Logging functions
 */

// String -> None
// Prints log.
void plog(const char* msg)
{
    logging_log(FNAME, msg);
}

// String -> None
void pwarn(const char* msg)
{
    logging_warn(FNAME, msg);
}

// String -> None
// Returns an error then exit with status 1
void die(const char* msg)
{
    logging_error(FNAME, msg);
    // perror(msg);

    exit(1);
}

// String -> None
// Returns an error then exit with status 1
// in addition of a perror() call
void pdie(const char* msg)
{
    logging_error(FNAME, msg);
    perror(msg);

    exit(1);
}

// struct CPU, String -> None
// Halts the CPU with a message.
void halt(struct CPU *cpu, const char *msg)
{
    logging_error(FNAME, msg);
    cpu->running = 0;
}

/*
 * Utility functions
 */

// BYTE, BYTE -> WORD
// converts two bytes into a word.
// example : btoword(0x12, 0x34) produces 0x1234
WORD btoword(BYTE byte_a, BYTE byte_b)
{
    return (byte_a * 0x100) + byte_b;
}


// struct CPU, Integer -> Integer
// If the register is not a thing, return -1
BYTE *get_register(struct CPU* cpu, int reg)
{
    if (reg >= 0 && reg < REGSIZE)
        return &(cpu->registers[reg]);
    else if (reg == REG_HL)
    {
        WORD addr = btoword(cpu->registers[REG_H], cpu->registers[REG_L]);
        return &(cpu->memory[addr]);
    }
    else
        die("Invalid register.");
}

void update_flags(struct CPU* cpu, BYTE before, BYTE after)
{
    // TODO: Add carry flag support.
    cpu->flags |= (after == 0x00) * Z_FLAG;
}

// struct CPU -> None
// inits the CPU
void init_cpu(struct CPU *cpu)
{
    int i;
    plog("Initializing CPU...");
    
    for (i = 0; i < 0x10000; ++i)
        cpu->memory[i] = 0x0;   // Filling the memory with zeros
    for (i = 0; i < REGSIZE; ++i)
        cpu->registers[i] = 0x0; // Filling the registers with zeros
    cpu->pc = 0x0000;           // BIOS Location
    cpu->sp = 0xffff;           // Stack pointer at top of RAM
    cpu->flags = 0x00;          // Flags
    cpu->running = 0x00;        // is it running?

    plog("CPU Initialized!");
}

/*
 * Opcode functions
 */

// mov r, n
void mov_rn(struct CPU* cpu, int reg_a, BYTE byte_a)
{
    BYTE *r = get_register(cpu, reg_a);
    *r = byte_a;
}

// mov r, r
void mov_rr(struct CPU* cpu, int reg_a, int reg_b)
{
    BYTE *ra = get_register(cpu, reg_a);
    BYTE *rb = get_register(cpu, reg_b);

    *ra = *rb;
}

// add r
void add_r(struct CPU* cpu, int reg_a)
{
    BYTE *r = get_register(cpu, reg_a);
    cpu->registers[REG_A] += *r;
}

// add n
void add_n(struct CPU* cpu, BYTE byte_a)
{
    cpu->registers[REG_A] += byte_a;
}

// sub r
void sub_r(struct CPU* cpu, int reg_a)
{
    BYTE *r = get_register(cpu, reg_a);
    cpu->registers[REG_A] -= *r;
}

// sub n
void sub_n(struct CPU* cpu, BYTE byte_a)
{
    cpu->registers[REG_A] -= byte_a;
}

// and r
void and_r(struct CPU* cpu, int reg_a)
{
    BYTE *r = get_register(cpu, reg_a);
    cpu->registers[REG_A] &= *r;
}

// and n
void and_n(struct CPU* cpu, BYTE byte_a)
{
    cpu->registers[REG_A] &= byte_a;
}

// or r
void or_r(struct CPU* cpu, int reg_a)
{
    BYTE *r = get_register(cpu, reg_a);
    cpu->registers[REG_A] |= *r;
}

// or n
void or_n(struct CPU* cpu, BYTE byte_a)
{
    cpu->registers[REG_A] |= byte_a;
}

// xor r
void xor_r(struct CPU* cpu, int reg_a)
{
    BYTE *r = get_register(cpu, reg_a);
    cpu->registers[REG_A] ^= *r;
}

// xor n
void xor_n(struct CPU* cpu, BYTE byte_a)
{
    cpu->registers[REG_A] ^= byte_a;
}

// inc
void inc_r(struct CPU* cpu, int reg_a)
{
    cpu->registers[reg_a]++;
}

// dec
void dec_r(struct CPU* cpu, int reg_a)
{
    cpu->registers[reg_a]--;
}

// sl r
void sl_r(struct CPU* cpu, int reg_a)
{
    cpu->registers[reg_a] <<= 1;
}

// sr r
void sr_r(struct CPU* cpu, int reg_a)
{
    cpu->registers[reg_a] >>= 1;
}

// cp r
void cp_r(struct CPU* cpu, int reg_a)
{
    BYTE *rega = get_register(cpu, reg_a);
    BYTE acc = cpu->registers[REG_A];

    BYTE nflags = (acc == *rega) * Z_FLAG + (acc < *rega) * C_FLAG;
    cpu->flags = nflags;
}

// pop
WORD stack_pop(struct CPU* cpu)
{
    BYTE h = cpu->memory[++(cpu->sp)];
    BYTE l = cpu->memory[++(cpu->sp)];

    return btoword(h, l);
}

void stack_pop_p(struct CPU* cpu)
{
    WORD pop = stack_pop(cpu);
    cpu->memory[REG_H] = pop / 0x100;
    cpu->memory[REG_L] = pop % 0x100;
}

// push
void stack_push(struct CPU* cpu, WORD word_a)
{
    WORD pop = stack_pop(cpu);
    cpu->memory[(cpu->sp)--] = pop % 0x100;
    cpu->memory[(cpu->sp)--] = pop / 0x100;
}

void stack_push_p(struct CPU* cpu)
{
    stack_push(cpu, btoword(cpu->registers[REG_H], cpu->registers[REG_L]));
}

// jump
void mem_jump(struct CPU* cpu, int type, WORD addr)
{
    int b;
    switch (type)
    {
        case 0:     // jc
            b = C_FLAG & cpu->flags;
            break;
        case 1:     // jz
            b = Z_FLAG & cpu->flags;
            break;
        case 2:     // jn
            b = !(Z_FLAG & cpu->flags);
            break;
        default:    // jp
            b = 1;
            break;
    }

    if (b)
    {
        cpu->pc = addr - 1;
    }
}

// call
void mem_call(struct CPU* cpu, WORD addr)
{
    mem_jump(cpu, -1, addr);
    stack_push(cpu, cpu->pc);
}

// ret
void mem_ret(struct CPU* cpu)
{
    WORD addr = stack_pop(cpu);
    mem_jump(cpu, -1, addr + 1);
}

// DUMP FUNCTIONS FOR DEBUGGING
// dump registers
void dump_registers(struct CPU* cpu)
{
    int i;

    pwarn("-- REGISTER DUMP --");
    for (i = 0; i < REGSIZE; ++i)
        fprintf(stderr, "Register %2x = %02x\n", i, cpu->registers[i]);
    fprintf(stderr, "\n");
    pwarn("-- END OF DUMP --");
}

// dump 16 bytes of memory
void dump_memory(struct CPU* cpu, WORD start)
{
    WORD i;
    pwarn("-- MEMORY DUMP --");

    fprintf(stderr, "0x%04x ", start);
    for (i = 0; i < 0x10; i++)
        fprintf(stderr, "%02x ", cpu->memory[start + i]);
    fprintf(stderr, "\n");
    
    pwarn("-- END OF DUMP --");
}

// struct CPU -> Integer
// Executes one instruction at PC
void execute(struct CPU *cpu)
{
    BYTE instr = cpu->memory[cpu->pc];

    switch (instr)
    {
        case HALT:
            fprintf(stderr, "System halt at %4x.\n", cpu->pc);
            halt(cpu, "System halted!");
            break;
        case NOP:
            break;
        
        // Stack commands
        case PUSH:
            stack_push_p(cpu);
            break;
        case POP:
            stack_pop_p(cpu);
            break;

        // MOV commands
        case MOV_AA:
        case MOV_AB:
        case MOV_AC:
        case MOV_AD:
        case MOV_AE:
        case MOV_AH:
        case MOV_AL:
            mov_rr(cpu, REG_A, instr - MOV_AA);
            break;

        case MOV_BA:
        case MOV_BB:
        case MOV_BC:
        case MOV_BD:
        case MOV_BE:
        case MOV_BH:
        case MOV_BL:
            mov_rr(cpu, REG_B, instr - MOV_BA);
            break;

        case MOV_CA:
        case MOV_CB:
        case MOV_CC:
        case MOV_CD:
        case MOV_CE:
        case MOV_CH:
        case MOV_CL:
            mov_rr(cpu, REG_C, instr - MOV_CA);
            break;

        case MOV_DA:
        case MOV_DB:
        case MOV_DC:
        case MOV_DD:
        case MOV_DE:
        case MOV_DH:
        case MOV_DL:
            mov_rr(cpu, REG_D, instr - MOV_DA);
            break;

        case MOV_EA:
        case MOV_EB:
        case MOV_EC:
        case MOV_ED:
        case MOV_EE:
        case MOV_EH:
        case MOV_EL:
            mov_rr(cpu, REG_E, instr - MOV_EA);
            break;

        case MOV_HA:
        case MOV_HB:
        case MOV_HC:
        case MOV_HD:
        case MOV_HE:
        case MOV_HH:
        case MOV_HL:
            mov_rr(cpu, REG_H, instr - MOV_HA);
            break;
    
        case MOV_LA:
        case MOV_LB:
        case MOV_LC:
        case MOV_LD:
        case MOV_LE:
        case MOV_LH:
        case MOV_LL:
            mov_rr(cpu, REG_L, instr - MOV_LA);
            break;

        case MOV_AN:
        case MOV_BN:
        case MOV_CN:
        case MOV_DN:
        case MOV_EN:
        case MOV_HN:
        case MOV_LN:
            mov_rn(cpu, instr - MOV_AN, cpu->memory[++(cpu->pc)]);
            break;

        case MOV_AP:
        case MOV_BP:
        case MOV_CP:
        case MOV_DP:
        case MOV_EP:
        case MOV_HP:
        case MOV_LP:
            mov_rr(cpu, instr - MOV_AP, REG_HL);
            break;

        case MOV_PA:
        case MOV_PB:
        case MOV_PC:
        case MOV_PD:
        case MOV_PE:
        case MOV_PH:
        case MOV_PL:
            mov_rr(cpu, REG_HL, instr - MOV_PA);
            break;

        case ADD_A:
        case ADD_B:
        case ADD_C:
        case ADD_D:
        case ADD_E:
        case ADD_H:
        case ADD_L:
            add_r(cpu, instr - ADD_A);
            break;
        case ADD_N:
            add_n(cpu, cpu->memory[++(cpu->pc)]);
            break;
        case ADD_P:
            add_r(cpu, REG_HL);
            break;

        case SUB_A:
        case SUB_B:
        case SUB_C:
        case SUB_D:
        case SUB_E:
        case SUB_H:
        case SUB_L:
            sub_r(cpu, instr - SUB_A);
            break;
        case SUB_N:
            sub_n(cpu, cpu->memory[++(cpu->pc)]);
            break;
        case SUB_P:
            sub_r(cpu, REG_HL);
            break;

        case AND_A:
        case AND_B:
        case AND_C:
        case AND_D:
        case AND_E:
        case AND_H:
        case AND_L:
            and_r(cpu, instr - AND_A);
            break;
        case AND_N:
            and_n(cpu, cpu->memory[++(cpu->pc)]);
            break;
        case AND_P:
            and_r(cpu, REG_HL);
            break;
        
        case OR_A:
        case OR_B:
        case OR_C:
        case OR_D:
        case OR_E:
        case OR_H:
        case OR_L:
            or_r(cpu, instr - OR_A);
            break;
        case OR_N:
            or_n(cpu, cpu->memory[++(cpu->pc)]);
            break;
        case OR_P:
            or_r(cpu, REG_HL);
            break;

        case XOR_A:
        case XOR_B:
        case XOR_C:
        case XOR_D:
        case XOR_E:
        case XOR_H:
        case XOR_L:
            xor_r(cpu, instr - XOR_A);
            break;
        case XOR_N:
            xor_n(cpu, cpu->memory[++(cpu->pc)]);
            break;
        case XOR_P:
            xor_r(cpu, REG_HL);
            break;

        case INC_A:
        case INC_B:
        case INC_C:
        case INC_D:
        case INC_E:
        case INC_H:
        case INC_L:
            inc_r(cpu, instr - INC_A);
            break;

        case DEC_A:
        case DEC_B:
        case DEC_C:
        case DEC_D:
        case DEC_E:
        case DEC_H:
        case DEC_L:
            dec_r(cpu, instr - DEC_A);
            break;

        case SL_A:
        case SL_B:
        case SL_C:
        case SL_D:
        case SL_E:
        case SL_H:
        case SL_L:
            inc_r(cpu, instr - SL_A);
            break;

        case SR_A:
        case SR_B:
        case SR_C:
        case SR_D:
        case SR_E:
        case SR_H:
        case SR_L:
            sr_r(cpu, instr - SR_A);
            break;

        // Jump and Conditions
        case CP_A:
        case CP_B:
        case CP_C:
        case CP_D:
        case CP_E:
        case CP_H:
        case CP_L:
            cp_r(cpu, instr - SR_A);
            break;
        case CP_N:
            // TODO Make cp_n
            // cp_n(cpu, cpu->memory[++cpu->pc]);
            break;
        case CP_P:
            cp_r(cpu, instr - REG_HL);

        case JP_NN:
            mem_jump(cpu, -1, btoword(cpu->memory[++cpu->pc], cpu->memory[++cpu->pc]));
            break;
        case JP_P:
            mem_jump(cpu, -1, btoword(cpu->registers[REG_H], cpu->registers[REG_L]));
            break;

        case JC_NN:
            mem_jump(cpu, JC, btoword(cpu->memory[++cpu->pc], cpu->memory[++cpu->pc]));
            break;
        case JC_P:
            mem_jump(cpu, JC, btoword(cpu->registers[REG_H], cpu->registers[REG_L]));
            break;

        case JZ_NN:
            mem_jump(cpu, JZ, btoword(cpu->memory[++cpu->pc], cpu->memory[++cpu->pc]));
            break;
        case JZ_P:
            mem_jump(cpu, JZ, btoword(cpu->registers[REG_H], cpu->registers[REG_L]));
            break;

        case JN_NN:
            mem_jump(cpu, JNZ, btoword(cpu->memory[++cpu->pc], cpu->memory[++cpu->pc]));
            break;
        case JN_P:
            mem_jump(cpu, JNZ, btoword(cpu->registers[REG_H], cpu->registers[REG_L]));
            break;
    
        case CALL_NN:
            mem_call(cpu, btoword(cpu->memory[++cpu->pc], cpu->memory[++cpu->pc]));
            break;
        case CALL_P:
            mem_call(cpu, btoword(cpu->registers[REG_H], cpu->registers[REG_L]));
            break;

        case RET:
            mem_ret(cpu);
            break;

        case DUMP_R:
            dump_registers(cpu);
            break;
        case DUMP_M:
            dump_memory(cpu, btoword(cpu->memory[++cpu->pc], cpu->memory[++cpu->pc]));
            break;

        default:
            printf("Halting at 0x%4x...\n", cpu->pc);
            halt(cpu, "Invalid opcode! Stopping...");
            break;
    }
}

// struct CPU -> None
// Run code from the Program Counter
void run(struct CPU *cpu)
{
    cpu->running = 1;

    plog("Beginning code execution.");
    while (cpu->running)
    {
        execute(cpu);
        ++(cpu->pc);
    }
    plog("Execution ended.");
}
