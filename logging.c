/*
 * VICERA by h34ting4ppliance
 *
 * logging.c
 *
 * This file contains all the logging functions to make log formatting easier.
 */
#include <stdio.h>
#include <stdlib.h>

// String, String -> None
// writes log
void logging_log(const char* comp, const char* msg)
{
    fprintf(stderr, "\033[1m[#] |%s|\033[0m %s\n", comp, msg);
}

// String, String -> None
// warning log
void logging_warn(const char* comp, const char* msg)
{
    fprintf(stderr, "\033[1;43m[!] |%s|\033[0;43m %s \033[0m\n", comp, msg);
}

// String, String -> None
// writes error
void logging_error(const char* comp, const char* msg)
{
    fprintf(stderr, "\033[1;41m[X] |%s|\033[0;41m %s \033[0m\n", comp, msg);
}
