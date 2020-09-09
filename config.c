/*
 * VICERA by h34ting4ppliance
 *
 * config.c
 *
 * Config parser for the vicera
 * ----
 * Config example :
 * 
 * foo=bar
 * poggers = hi
 * controller.left = 137
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#define FNAME "config.c"

static void die(const char* message)
{
    logging_error(FNAME, message);
    perror(message);

    exit(1);
}

// const char* -> char*
// Loads a file into a string
//
// Example: IN: "file.txt"
//         OUT: "Hello, World!"
char* load_file_into_string(const char* filename)
{
    int err, c;
    // Load file into read mode.
    FILE* f = fopen(filename, "r");
    // Check if it successfully loaded.
    if (!f)
        die("Unable to open file.");

    // Get length
    err = fseek(f, 0, SEEK_END);
    if (err)
        die("Unable to seek file end.");

    long sz = ftell(f);
    if (sz == -1)
        die("Unable to tell file size.");

    // Malloc depending of the file size.
    char* result = (char *)malloc(sz * sizeof(char));
    char* tmp = result;

    // Copy file content
    rewind(f);
    while ((c=fgetc(f)) != EOF)
        *(tmp++) = c;

    return result;
}

// char* -> char*
// Strips whitespace from left
char* lstrip(char* a_string)
{
    while (*a_string == ' ' ||
           *a_string == '\t' ||
           *a_string == '\n')
        *(a_string++) = '\0';

    return a_string;
}

// char* -> char*
// Strips whitespace from right
char* rstrip(char* a_string)
{
    char *tmp = a_string + strlen(a_string)-1;
    while (*tmp == ' ' ||
           *tmp == '\t' ||
           *tmp == '\n')
        *(tmp--) = '\0';
    
    return a_string;
}

// char* -> char*
// Just strips
char* strip(char* a_string)
{
    a_string = lstrip(a_string);
    a_string = rstrip(a_string);

    return a_string;
}

// Parser
void parse_config(const char* filename, 
                  void (*handler)(const char*, const char*))
{
    // Load the file
    char* cfg = load_file_into_string(filename);
    // Some useful functions
    char *token, *next_token, *name, *value;

    // Parsing loop
    token = strtok(cfg, "\n");
    while (token != NULL)
    {
        next_token = strtok(NULL, "\0");
        
        name = strtok(token, "=");
        if (name) 
            name = strip(name);
        value = strtok(NULL, "\n");
        // Ignore if value is NULL (meaning there is no = sign)
        // or if there is # (comment)
        if (name && name[0] != '#' && value)
        {
            value = strip(value);
            handler(name, value);
        }
        
        token = strtok(next_token, "\n");
    }

    free(cfg);
}
