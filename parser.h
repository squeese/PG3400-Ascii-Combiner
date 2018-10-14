#ifndef ASCI_GRID_PARSER
#define ASCI_GRID_PARSER

#include <dirent.h>
#include <stdio.h>    // perror

int p_parse(const char* path, const char* out);
int p_scan(const char* path, int* const x, int* const y, char** name);
int p_open_output(FILE** fd, char** path, const char* out, const char* name);

#endif
