#ifndef CONFIG_H
#define CONFIG_H

char* load_file_into_string(const char* filename);
char* strip(char* a_string);
void parse_config(const char* filename,
                  void (*handler)(const char*, const char*));
#endif
