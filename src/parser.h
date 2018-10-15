#ifndef ASCI_GRID_PARSER
#define ASCI_GRID_PARSER

#include <dirent.h>
#include <stdio.h>

int p_parse(char* path, char* out);
int p_scan(char* path, int* x, int* y, char** name);
int p_open_output(FILE** fd, char** path, char* out, char* name);
int p_max(int a, int b);
int p_num_digits(int value);

#endif
