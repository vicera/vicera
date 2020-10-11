/*
 * VICERA by h34ting4ppliance
 *
 * debug.c
 *
 * Debugging features for the VICERA.
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "logging.h"
#include "cpu.h"

#define FNAME "debug.c"

const char*  reg8s[] = {"A", "B", "C", "D", "E", "H", "L"};
const char* reg16s[] = {"HL", "BC", "DE", "SP"};

// Formatted log.
static void flog(const char* msg, ...)
{
    fprintf(stderr, "\033[1m[#] |%s|\033[0m ", FNAME);
    va_list args;
    va_start(args, msg);

    vfprintf(stderr, msg, args);
    fprintf(stderr, "\n");

    va_end(args);
}

static WORD memword(BYTE **ptr)
{
    ++(*ptr);
    return (**ptr * 0x100) + *(++(*ptr));
}

// Prints and disassemble an instruction.
WORD disasm_instruction(struct CPU* cpu, WORD addr)
{
    #define MEMPOS          ptr - cpu->memory
    #define PRIR(msg, r)    flog("#%04x\t%s\t%s", MEMPOS, msg, reg8s[r])
    #define PRIN(msg)       flog("#%04x\t%s\t#%02x", MEMPOS, msg, *(++ptr))
    #define PRINN(msg)      flog("#%04x\t%s\t#%04x", MEMPOS, msg, memword(&ptr))
    #define PRIP(msg, r)    flog("#%04x\t%s\t(%s)", MEMPOS, msg, reg16s[r])
    #define PRIRR(msg, r)   flog("#%04x\t%s\t%s", MEMPOS, msg, reg16s[r])
    #define PRIRR_N(msg, r) \
                            flog("#%04x\t%s\t%s, \t#%04x", MEMPOS, msg, \
                                    reg16s[r], memword(&ptr))
    #define PRIRR_RR(msg, r1, r2) \
                            flog("#%04x\t%s\t%s, \t%s", MEMPOS, msg, \
                                    reg16s[r1], reg16s[r2])
    #define PRIR_RR(msg, r1, r2) \
                            flog("#%04x\t%s\t%s, \t%s", MEMPOS, msg, \
                                    reg8s[r1], reg16s[r2])
    #define PRIRR_R(msg, r1, r2) \
                            flog("#%04x\t%s\t%s, \t%s", MEMPOS, msg, \
                                    reg16s[r1], reg8s[r2])
    #define PRIR_R(msg, r1, r2) \
                            flog("#%04x\t%s\t%s, \t%s",  MEMPOS, msg, \
                                    reg8s[r1], reg8s[r2])
    #define PRIP_R(msg, r1, r2)  \
                            flog("#%04x\t%s\t(%s), \t%s",MEMPOS, msg, \
                                    reg16s[r1], reg8s[r2])
    #define PRIR_P(msg, r1, r2)  \
                            flog("#%04x\t%s\t%s, \t(%s)",MEMPOS, msg, \
                                    reg8s[r1], reg16s[r2])
    #define PRIANN(msg) \
                            flog("#%04x\t%s\tA, \t(#%04x)", MEMPOS, msg, \
                                    memword(&ptr));
    #define PRINNA(msg) \
                            flog("#%04x\t%s\t(#%04x),A", MEMPOS, msg, \
                                    memword(&ptr));
    #define PRIP_N(msg, r)  \
                            flog("#%04x\t%s\t(%s), \t#%02x",MEMPOS, msg, \
                                    reg16s[r], *(++ptr))
    #define PRIR_N(msg, r) \
                            flog("#%04x\t%s\t%s, \t#%02x",MEMPOS, msg, \
                                    reg8s[r], *(++ptr))
    #define PRINST(msg)     flog("#%04x\t%s\t ", MEMPOS, msg)
    
    BYTE *ptr = cpu->memory + addr;
    BYTE instr = *ptr;

    switch (instr)
    {
        case HALT:
            PRINST("HALT");
            break;
        case NOP:
            PRINST("NOP");
            break;
        
        // Stack commands
        case PUSH_HL:
        case PUSH_BC:
        case PUSH_DE:
            PRIRR("PUSH", instr - PUSH_HL);
            break;
        case POP_HL:
        case POP_BC:
        case POP_DE:
            PRIRR("POP", instr - POP_HL);
            break;

        case PUSHA:
            PRINST("PUSHA");
            break;
        case POPA:
            PRINST("POPA");
            break;

        // MOV commands
        case MOV_AA:
        case MOV_AB:
        case MOV_AC:
        case MOV_AD:
        case MOV_AE:
        case MOV_AH:
        case MOV_AL:
            PRIR_R("MOV", 0, instr - MOV_AA);
            break;

        case MOV_BA:
        case MOV_BB:
        case MOV_BC:
        case MOV_BD:
        case MOV_BE:
        case MOV_BH:
        case MOV_BL:
            PRIR_R("MOV", 1, instr - MOV_BA);
            break;

        case MOV_CA:
        case MOV_CB:
        case MOV_CC:
        case MOV_CD:
        case MOV_CE:
        case MOV_CH:
        case MOV_CL:
            PRIR_R("MOV", 2, instr - MOV_CA);
            break;

        case MOV_DA:
        case MOV_DB:
        case MOV_DC:
        case MOV_DD:
        case MOV_DE:
        case MOV_DH:
        case MOV_DL:
            PRIR_R("MOV", 3, instr - MOV_DA);
            break;

        case MOV_EA:
        case MOV_EB:
        case MOV_EC:
        case MOV_ED:
        case MOV_EE:
        case MOV_EH:
        case MOV_EL:
            PRIR_R("MOV", 4, instr - MOV_EA);
            break;

        case MOV_HA:
        case MOV_HB:
        case MOV_HC:
        case MOV_HD:
        case MOV_HE:
        case MOV_HH:
        case MOV_HL:
            PRIR_R("MOV", 5, instr - MOV_HA);
            break;
    
        case MOV_LA:
        case MOV_LB:
        case MOV_LC:
        case MOV_LD:
        case MOV_LE:
        case MOV_LH:
        case MOV_LL:
            PRIR_R("MOV", 6, instr - MOV_LA);
            break;

        case MOV_AN:
        case MOV_BN:
        case MOV_CN:
        case MOV_DN:
        case MOV_EN:
        case MOV_HN:
        case MOV_LN:
            PRIR_N("MOV", instr - MOV_AN);
            break;

        case MOV_AP:
        case MOV_BP:
        case MOV_CP:
        case MOV_DP:
        case MOV_EP:
        case MOV_HP:
        case MOV_LP:
            PRIR_P("MOV", instr - MOV_AP, 0);
            break;

        case MOV_PA:
        case MOV_PB:
        case MOV_PC:
        case MOV_PD:
        case MOV_PE:
        case MOV_PH:
        case MOV_PL:
            PRIP_R("MOV", 0, instr - MOV_PA);
            break;
        case MOV_PN:
            PRIP_N("MOV", 0);
            break;

        case MOV_SPNN:
            PRIRR_N("MOV", 3);
            break;
        case MOV_SPHL:
            PRIRR_RR("MOV", 3, 0);
            break;
        case MOV_HLSP:
            PRIRR_RR("MOV", 0, 3);
            break;

        case MOV_ABC:
        case MOV_ADE:
            PRIR_P("MOV", 0, instr + 1 - MOV_ABC);
            break;
        case MOV_ANN:
            PRIANN("MOV");
            break;

        case MOV_BCA:
        case MOV_DEA:
            PRIP_R("MOV", instr + 1 - MOV_BCA, 0);
            break;
        case MOV_NNA:
            PRINNA("MOV");
            break;
        
        case MOV_HLNN:
        case MOV_BCNN:
        case MOV_DENN:
            PRIRR_N("MOV", instr - MOV_HLNN);
            break;

        case ADD_A:
        case ADD_B:
        case ADD_C:
        case ADD_D:
        case ADD_E:
        case ADD_H:
        case ADD_L:
            PRIR_R("ADD", 0, instr - ADD_A);
            break;
        case ADD_N:
            PRIR_N("ADD", 0);
            break;
        case ADD_P:
            PRIR_P("ADD", 0, 0);
            break;
        case ADD_HL:
        case ADD_BC:
        case ADD_DE:
            PRIRR_RR("ADD", 0, instr - ADD_HL);
            break;
        case ADD_NN:
            PRIRR_N("ADD", 0);
            break;

        case SUB_A:
        case SUB_B:
        case SUB_C:
        case SUB_D:
        case SUB_E:
        case SUB_H:
        case SUB_L:
            PRIR_R("SUB", 0, instr - SUB_A);
            break;
        case SUB_N:
            PRIR_N("SUB", 0);
            break;
        case SUB_P:
            PRIR_P("SUB", 0, 0);
            break;
        case SUB_HL:
        case SUB_BC:
        case SUB_DE:
            PRIRR_RR("SUB", 0, instr - SUB_HL);
            break;
        case SUB_NN:
            PRIRR_N("SUB", 0);
            break;
        
        case AND_A:
        case AND_B:
        case AND_C:
        case AND_D:
        case AND_E:
        case AND_H:
        case AND_L:
            PRIR_R("AND", 0, instr - AND_A);
            break;
        case AND_N:
            PRIR_N("AND", 0);
            break;
        case AND_P:
            PRIR_P("AND", 0, 0);
            break;
        
        case OR_A:
        case OR_B:
        case OR_C:
        case OR_D:
        case OR_E:
        case OR_H:
        case OR_L:
            PRIR_R("OR", 0, instr - OR_A);
            break;
        case OR_N:
            PRIR_N("OR", 0);
            break;
        case OR_P:
            PRIR_P("OR", 0, 0);
            break;

        case XOR_A:
        case XOR_B:
        case XOR_C:
        case XOR_D:
        case XOR_E:
        case XOR_H:
        case XOR_L:
            PRIR_R("XOR", 0, instr - XOR_A);
            break;
        case XOR_N:
            PRIR_N("XOR", 0);
            break;
        case XOR_P:
            PRIR_P("XOR", 0, 0);
            break;

        case INC_A:
        case INC_B:
        case INC_C:
        case INC_D:
        case INC_E:
        case INC_H:
        case INC_L:
            PRIR("INC", instr - INC_A);
            break;
        case INC_HL:
        case INC_BC:
        case INC_DE:
            PRIRR("INC", instr - INC_HL);
            break;

        case DEC_A:
        case DEC_B:
        case DEC_C:
        case DEC_D:
        case DEC_E:
        case DEC_H:
        case DEC_L:
            PRIR("DEC", instr - DEC_A);
            break;
        case DEC_HL:
        case DEC_BC:
        case DEC_DE:
            PRIRR("DEC", instr - DEC_HL);
            break;

        case SL_A:
        case SL_B:
        case SL_C:
        case SL_D:
        case SL_E:
        case SL_H:
        case SL_L:
            PRIR("SL", instr - SL_A);
            break;

        case SR_A:
        case SR_B:
        case SR_C:
        case SR_D:
        case SR_E:
        case SR_H:
        case SR_L:
            PRIR("SR", instr - SR_A);
            break;

        // Jump and Conditions
        case CP_A:
        case CP_B:
        case CP_C:
        case CP_D:
        case CP_E:
        case CP_H:
        case CP_L:
            PRIR("CP", instr - CP_A);
            break;
        case CP_N:
            PRIN("CP");
            break;
        case CP_P:
            PRIP("CP", 0);
            break;

        case JP_NN:
            PRINN("JP");
            break;
        case JP_P:
            PRIRR("JP", 0);
            break;

        case JC_NN:
            PRINN("JC");
            break;
        case JC_P:
            PRIRR("JC", 0);
            break;

        case JZ_NN:
            PRINN("JZ");
            break;
        case JZ_P:
            PRIRR("JZ", 0);
            break;

        case JN_NN:
            PRINN("JN");
            break;
        case JN_P:
            PRIRR("JN", 0);
            break;
    
        case CALL_NN:
            PRINN("CALL");
            break;
        case CALL_P:
            PRIRR("CALL", 0);
            break;

        case RET:
            PRINST("RET");
            break;

        case DUMP_R:
            PRINST("DUMPR");
            break;
        case DUMP_M:
            PRINN("DUMPM");
            break;
        
        case SLP:
            PRINST("SLP");
            break;
        
        case SWAP_A:
        case SWAP_B:
        case SWAP_C:
        case SWAP_D:
        case SWAP_E:
        case SWAP_H:
        case SWAP_L:
            PRIR("SWAP", instr - SWAP_A);
            break;
        case SWAP_P:
            PRIP("SWAP", 0);
            break;
        
        case SWAP_HL:
        case SWAP_BC:
        case SWAP_DE:
            PRIRR("SWAP", instr - SWAP_HL);
            break;
    }
    
    return ptr - cpu->memory;
}

// "Disassemble" multiple instructions
void disasm_line(struct CPU* cpu, WORD addr, WORD n)
{
    logging_warn(FNAME, " --- 5X DISASM --- ");
    // TODO: Fix this and finish the snake.
    WORD p = addr;
    for (WORD i = 0; i < n; ++i)
        p = disasm_instruction(cpu, p) + 1;
        
}
