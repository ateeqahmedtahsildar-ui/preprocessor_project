#ifndef FILE_INCLUSION_H
#define FILE_INCLUSION_H
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ctype.h>
#define MAX_MACROS 256
#define MAX_PARAMS 16
#define MAX_LINE 4096
int process_macros(const char *input_file, const char *output_file);
int process_includes(const char *input_file, const char *output_file);
int remove_comments(const char *input_file, const char *output_file);
#endif
