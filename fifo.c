/*
 * VICERA by h34ting4ppliance
 *
 * fifo.c
 *
 * The FIFO extension is an extension that allow direct read/write access and
 * basic CPU control from 3rd party software using a FIFO special file.
 *
 * Since FIFOs are most likely a *nix thing, this is not compatible with
 * Microsoft Windows systems.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fifo.h"
#include "logging.h"
#include "cpu.h"
// From config.c
extern char* strip(char*);

// Filename
#define FNAME       "fifo.c"

// Command successfully executed
#define F_SUCCESS   0
// Command failed to execute
#define F_FAIL      1
// Command was unknown
#define F_UNKNOWN   2

///// BUFFERING //////

void init_buff(struct LineBuffer* buf)
{
    buf->size = 1;
    buf->ptr = 0;

    buf->buff = (char*)malloc(sizeof(char) * buf->size);
}

void free_buff(struct LineBuffer* buf)
{
    buf->size = -1;
    buf->ptr = -1;

    free(buf->buff);
}

void append_buff(struct LineBuffer* buf, const char c)
{
    if (++(buf->ptr) == buf->size)
        buf->size *= 2;
    buf->buff = realloc(buf->buff, sizeof(char) * buf->size);
    buf->buff[buf->ptr] = c;
}

//////////////////////
// Logging

static void warn(const char* msg)
{
    logging_warn(FNAME, msg);
}

static void die(const char* msg)
{
    logging_error(FNAME, msg);
    perror("Error");

    exit(1);
}

//////////

// Get command ID from string
static FIFOInstruction get_instr(const char* cmd)
{
    #define MATCH(x) (strcmp(cmd, x) == 0) return

    if      MATCH("write")  F_WRITE;
    else if MATCH("read")   F_READ;
    else if MATCH("jump")   F_JMP;
    else if MATCH("regdmp") F_REGDMP;
    else             return F_NONE;
}

// char* -> FIFOCommand
// Parses a line
static struct FIFOCommand parse_line(char* line)
{
    struct FIFOCommand cmd;
    char *token, *saveptr;

    token = strtok_r(line, " \t", &saveptr);
    cmd.inst = get_instr(token);
    if (cmd.inst == F_NONE ||
        cmd.inst == F_REGDMP)
        return cmd;
    
    cmd.args[0] = cmd.args[1] = 0;

    token = strtok_r(NULL, " \t", &saveptr);
    for (int i = 0; i < 2 && token; ++i)
    {
        token = strip(token);
        cmd.args[i] = strtol(token, NULL, 0);

        token = strtok_r(NULL, " \t", &saveptr);
    }

    return cmd;
}

// FIFOCommand* -> int
// Executes a FIFO Command
static int execute_command(struct CPU *cpu, 
                            struct FIFOCommand* cmd,
                            char** str)
{
    *str = NULL;
    int state = F_UNKNOWN;
    switch (cmd->inst)
    {
        case F_NONE:
            state = F_UNKNOWN;
            break;
        case F_WRITE:
            cpu->memory[cmd->args[0]] = cmd->args[1];
            
            state = F_SUCCESS;
            asprintf(str, "success\n");
            break;
        case F_READ:
            state = F_SUCCESS;
            asprintf(str, "success %d\n", cpu->memory[cmd->args[0]]);
            break;
        case F_JMP:
            cpu->pc = cmd->args[0];

            state = F_SUCCESS;
            asprintf(str, "success\n");
            break;
        case F_REGDMP:
            asprintf(str, "success %d %d %d %d %d %d %d %d %d\n",
                    cpu->registers[REG_A],
                    cpu->registers[REG_B],
                    cpu->registers[REG_C],
                    cpu->registers[REG_D],
                    cpu->registers[REG_E],
                    cpu->registers[REG_H],
                    cpu->registers[REG_L],

                    cpu->sp, cpu->pc);
            
            state = F_SUCCESS;
            break;
    }

    if (state != F_SUCCESS)
    {
        warn("A command has failed.");
        asprintf(str, "failed\n");
    }
    return state;
}

// const char* -> void
// Receive from FIFO
void receive_from_fifo(struct CPU* cpu, const char* filename)
{
    // Some useful variables
    struct LineBuffer linebuf;
    struct FIFOCommand fifo;

    // malloc
    char* tmp;
    char* result = (char*)malloc(1);
    if (!result)
        die("Unable to malloc");
    result[0] = '\0';

    // Open the FIFO to get the information
    int c;
    FILE* f = fopen(filename, "r");
    if (!f)
        die("Unable to open file");

    // Loop 'til there is no more data
    while ((c=fgetc(f)) != EOF)
    {
        init_buff(&linebuf);
        do
        {
            append_buff(&linebuf, c);
        } while ((c=fgetc(f)) != EOF && c != '\n');
        append_buff(&linebuf, 0);

        // That would be a waste of time if the string is empty
        if (strlen(linebuf.buff) > 1)
        {
            fifo = parse_line(linebuf.buff);
            
            execute_command(cpu, &fifo, &tmp);
            if (!tmp)
                die("Null pointer.");

            result = realloc(result, sizeof(char) *
                                    (strlen(result)+strlen(tmp)));
            strcat(result, tmp);
            free(tmp);
        }
        // Free to allocate it again
        free_buff(&linebuf);
    }

    fclose(f);
    // Put the content of the result string back into the FIFO
    
    // Open the file for writing this time.
    f = fopen(filename, "w");
    if (!f)
        die("Unable to open file for writing");
    
    puts(result);
    fputs(result, f);

    // Free and close
    fclose(f);
    free(result);
}
