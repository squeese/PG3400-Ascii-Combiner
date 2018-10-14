#ifndef ASCI_GRID_PARSER
#define ASCI_GRID_PARSER

#include <dirent.h>

struct Parser {
  int x;         // maximum X value from the filenames
  int y;         // maximum Y value --//--
  char* name;    // name of the file from the filenames (used, if no name is specified from user)
  char* buffer;  // buffer used when reading from files, before writing to final file
};

int p_parse(char* path, void* out);
int p_scan_filenames(struct Parser* p, DIR* d_pointer);

// int p_write(struct Parser* p, void* name);
// void p_free(struct Parser* p);
// int test(char* path);

#endif
