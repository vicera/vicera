/*
 *  _____         _       
 * |_   _|__  ___| |_ ___ 
 *   | |/ _ \/ __| __/ __|
 *   | |  __/\__ \ |_\__ \
 *   |_|\___||___/\__|___/
 * 
 * This C file will contain a test framework
 * and tests to test the CPU opcodes.
 *
 * This file contains multiple tests to see
 * if the CPU works as attended. The following tests
 * will be done :
 * 
 *  - All the MOV opcodes
 *  - Arithmetic opcodes
 *  - Stack operations
 *  - Jumps and conditional jumps
 *  - Subroutines
 *
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "cpu.h"

struct CPU system;

void load_program(struct CPU *cpu, const int *program)
{
    size_t i;

    for (i = 0; program[i] != EOF; ++i)
        cpu->memory[i] = program[i];
}

void mov_opcodes_rn()
{
    /*
     * MOV r. n
     */
    const int program[] =
    {
        MOV_AN, 0x01,
        MOV_BN, 0x20,
        MOV_CN, 0xf3,
        MOV_DN, 0x13,
        MOV_EN, 0xff,
        MOV_HN, 0x12,
        MOV_LN, 0x31,
        HALT,
        -1
    };

    load_program(&system, program);
    run(&system);
    
    assert_int_equal(system.registers[REG_A], 0x01);
    assert_int_equal(system.registers[REG_B], 0x20);
    assert_int_equal(system.registers[REG_C], 0xf3);
    assert_int_equal(system.registers[REG_D], 0x13);
    assert_int_equal(system.registers[REG_E], 0xff);
    assert_int_equal(system.registers[REG_H], 0x12);
    assert_int_equal(system.registers[REG_L], 0x31);
}

void mov_opcodes_rr()
{
    int i, j;
    /*
     * mov r, r
     */
    const int program_b1[] =
    {
        MOV_AN, 0xde,
        MOV_BA, MOV_CA, MOV_DA, MOV_EA, MOV_HA, MOV_LA,
        HALT, -1
    };
    const int program_b2[] =
    {
        MOV_BN, 0xad,
        MOV_AB, MOV_CB, MOV_DB, MOV_EB, MOV_HB, MOV_LB,
        HALT, -1
    };
    const int program_b3[] =
    {
        MOV_CN, 0xbe,
        MOV_AC, MOV_BC, MOV_DC, MOV_EC, MOV_HC, MOV_LC,
        HALT, -1
    };
    const int program_b4[] =
    {
        MOV_DN, 0xef,
        MOV_AD, MOV_BD, MOV_CD, MOV_ED, MOV_HD, MOV_LD,
        HALT, -1
    };
    const int program_b5[] =
    {
        MOV_EN, 0x42,
        MOV_AE, MOV_BE, MOV_CE, MOV_DE, MOV_HE, MOV_LE,
        HALT, -1
    };
    const int program_b6[] =
    {
        MOV_HN, 0x11,
        MOV_AH, MOV_BH, MOV_CH, MOV_DH, MOV_EH, MOV_LH,
        HALT, -1
    };
    const int program_b7[] =
    {
        MOV_LN, 0x22,
        MOV_AL, MOV_BL, MOV_CL, MOV_DL, MOV_EL, MOV_HL,
        HALT, -1
    };
    const int *programs_b[] = {
        program_b1, program_b2, program_b3, program_b4, program_b5, program_b6, program_b7 };
    const BYTE results_b[] = {
        0xde, 0xad, 0xbe, 0xef, 0x42, 0x11, 0x22
    };
    
    for (i = 0; i < REGSIZE; ++i)
    {
        system.pc = 0x0000;
        load_program(&system, programs_b[i]);
        run(&system);

        for (j = 0; j < REGSIZE; ++j)
            assert_int_equal(system.registers[j], results_b[i]);
    }
}

void mov_opcodes_rp()
{
    int i;
    /*
     * mov r, (HL)
     */
    
    system.pc = 0x0000;
    system.memory[0x1234] = 0xc0;
    system.memory[0xffff] = 0xb0;

    const int program[] =
    {
        MOV_HN, 0x12,
        MOV_LN, 0x34,
        MOV_AP, MOV_BP, MOV_CP,

        MOV_HN, 0xff,
        MOV_LH,
        MOV_DP, MOV_EP,
        HALT, -1
    };

    load_program(&system, program);
    run(&system);

    for (i = 0; i <= REG_C; ++i)
        assert_int_equal(system.registers[i], 0xc0);
    for (i = REG_D; i <= REG_E; ++i)
        assert_int_equal(system.registers[i], 0xb0);
}

void mov_opcodes_pr()
{
    int i;
    /*
     * mov (HL), r
     */
    system.pc = 0x0000;

    const int program[] =
    {
        MOV_AN, 0xde,
        MOV_BN, 0xad,
        MOV_CN, 0xbe,
        MOV_DN, 0xef,
        MOV_EN, 0xff,
        
        MOV_HN, 0xff,
        MOV_LN, 0x00,

        MOV_PA, INC_L,
        MOV_PB, INC_L,
        MOV_PC, INC_L,
        MOV_PD, INC_L,
        MOV_PE, INC_L,

        DUMP_M, 0x00, 0xff,
        HALT,
        -1
    };
    const BYTE results[] = {
        0xde, 0xad, 0xbe, 0xef, 0xff
    };

    load_program(&system, program);
    run(&system);

    for (i = 0; i <= REG_E; ++i)
        assert_int_equal(system.memory[0xff00 + i], results[i]);
}

void math_opcodes(void **state)
{
    assert_true(1);
}

int main()
{
    init_cpu(&system);

    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(mov_opcodes_rn),
        cmocka_unit_test(mov_opcodes_rr),
        cmocka_unit_test(mov_opcodes_rp),
        cmocka_unit_test(mov_opcodes_pr),
        cmocka_unit_test(math_opcodes),
    };
    
    return cmocka_run_group_tests(tests, NULL, NULL);
}
