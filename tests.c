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

void load_program_at(struct CPU *cpu, const int *program, WORD at)
{
    size_t i;
    for (i = 0; program[i] != EOF; ++i)
        cpu->memory[i + at] = program[i];
}

void load_program(struct CPU *cpu, const int *program)
{
    load_program_at(cpu, program, 0x0000);
}

/*
 * =================
 *    MOV OPCODES
 * =================
 */

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

        DUMP_M, 0xff, 0x00,
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

/*
 * ====================
 *  ARITHMEIC OPCODES
 * ====================
 */

void math_opcodes_add()
{
    /*
     * add A, r
     * add A, n
     * add A, (HL)
     */
    system.pc = 0x0000;

    // add A, r
    const int program_r[] = {
        MOV_AN, 0x00,
        MOV_BN, 0x01,
        MOV_CN, 0x02,
        MOV_DN, 0x04,
        MOV_EN, 0x08,
        MOV_HN, 0x10,
        MOV_LN, 0x20,

        ADD_B, ADD_C, ADD_D, ADD_E, ADD_H, ADD_L,
        HALT, -1
    };

    load_program(&system, program_r);
    run(&system);

    assert_int_equal(system.registers[REG_A], 0x3f);

    // add A, n
    system.pc = 0x0000;
    const int program_n[] = {
        MOV_AN, 0x10,
        ADD_N, 0x0a,

        HALT, -1
    };

    load_program(&system, program_n);
    run(&system);

    assert_int_equal(system.registers[REG_A], 0x1a);

    // add A, (HL)
    system.pc = 0x0000;
    system.memory[0x1234] = 0x0e;
    const int program_p[] = {
        MOV_AN, 0x20,
        MOV_HN, 0x12,
        MOV_LN, 0x34,
        ADD_P,

        HALT, -1
    };

    load_program(&system, program_p);
    run(&system);

    assert_int_equal(system.registers[REG_A], 0x2e);
}

void math_opcodes_inc_dec()
{
    int i;
    /*
     * inc r
     * dec r
     */
    
    init_cpu(&system);

    const int program[] = 
    {
        INC_A, DEC_A,
        INC_B, DEC_B,
        INC_C, DEC_C,
        INC_D, DEC_D,
        INC_E, DEC_E,
        INC_H, DEC_H,
        INC_L, DEC_L,

        HALT, -1
    };

    for (i = 0; i < REGSIZE; ++i)
        assert_int_equal(system.registers[i], 0x00);
}

/*
 * Stack operations
 */
void stack_opcodes()
{
    /*
     * Epic stack moment
     */

    system.sp = 0xffff;
    system.pc = 0x0000;
    const int program[] =
    {
        MOV_HN, 0xde,
        MOV_LN, 0xad,
        PUSH,
        MOV_HN, 0xbe,
        MOV_LN, 0xef,
        PUSH,
        
        DUMP_M, 0xff, 0xf0,
        POP, POP,
        HALT, -1
    };

    load_program(&system, program);
    run(&system);

    assert_int_equal(system.registers[REG_H], 0xde);
    assert_int_equal(system.registers[REG_L], 0xad);
}

/*
 * Jumping, Conditions and Subroutines
 */

void jumping_opcodes()
{
    int i;
    /*
     * jp nn
     * jp (HL)
     */

    const int program_1[] =
    {
        JP_NN, 0xde, 0xad,
        HALT, -1
    };
    const int program_2[] =
    {
        MOV_HN, 0xbe,
        MOV_LN, 0xef,
        JP_P,

        HALT, -1
    };
    const int *programs[] =
    {
        program_1, program_2
    };
    const WORD results[] =
    {
        0xdead, 0xbeef
    };
    
    system.memory[0xdead] = HALT;
    system.memory[0xbeef] = HALT;

    for (i = 0; i < 2; ++i)
    {
        system.pc = 0x0000;
        load_program(&system, programs[i]);
        run(&system);

        assert_int_equal(system.pc, results[i] + 1);
    }
}

void jumping_opcodes_cond()
{
    int i;
    /*
     * jc nn
     * jc (HL)
     * jz nn
     * jz (HL)
     * jnz nn
     * jnz (HL)
     */
    
    init_cpu(&system);

    const int program_1[] =
    {
        MOV_AN, 0x00,
        JZ_NN, 0xde, 0xad,
        
        MOV_AN, 0x01,
        HALT, -1
    };
    const int program_2[] =
    {
        MOV_AN, 0x00,
        MOV_HN, 0xbe,
        MOV_LN, 0xef,
        JZ_P,

        MOV_AN, 0x01,
        HALT, -1
    };
    
    system.flags = Z_FLAG;
    for (i = 0; i < 4; ++i)
    {
        system.pc = 0x0000;
        if (i < 2)
            load_program(&system, program_1);
        else
            load_program(&system, program_2);
        run(&system);

        switch (i)
        {
            case 0:
                assert_int_equal(system.pc, 0xdead + 1);
                system.flags = 0x00;
                break;
            case 1: 
                assert_int_equal(system.registers[REG_A], 0x01);
                system.flags = Z_FLAG;
                break;

            case 2:
                assert_int_equal(system.pc, 0xbeef + 1);
                system.flags = 0x00;
                break;
            case 3:
                assert_int_equal(system.registers[REG_A], 0x01);
                break;
        }
    }
}

void conditions()
{
    /*
     * cp A, r
     * cp A, (HL)
     * cp A, n
     */

    // TODO: Make tests
}

void subroutines()
{
    /*
     * call nn
     * call (HL)
     * ret
     */
    init_cpu(&system);

    // at 0x0000
    const int program_1[] = {
        // test 1
        CALL_NN, 0xde, 0xad,

        MOV_HN, 0xbe,
        MOV_LN, 0xef,
        CALL_P,

        HALT, -1
    };
    // at 0xdead
    const int program_2[] = {
        MOV_AN, 0x01,
        RET
    };
    // at 0xbeef
    const int program_3[] = {
        MOV_BN, 0x01,
        RET
    };

    load_program(&system, program_1);
    load_program_at(&system, program_2, 0xdead);
    load_program_at(&system, program_3, 0xbeef);
    run(&system);

    assert_int_equal(system.registers[REG_A], 0x01);
    assert_int_equal(system.registers[REG_B], 0x01);
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

        cmocka_unit_test(math_opcodes_add),
        cmocka_unit_test(math_opcodes_inc_dec),

        cmocka_unit_test(stack_opcodes),

        cmocka_unit_test(jumping_opcodes),
        cmocka_unit_test(jumping_opcodes_cond),

        cmocka_unit_test(conditions),

        cmocka_unit_test(subroutines)
    };
    
    return cmocka_run_group_tests(tests, NULL, NULL);
}
