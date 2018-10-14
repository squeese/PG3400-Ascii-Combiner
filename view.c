#include "parser.h"
#include <stdio.h>

int main(int count, char* args[]) {
  if (count < 2) {
    printf("\nError: Must provide path to the folder with ascii art to be processed\n");
    printf("  view PATH_FOLDER [ OUT_FILENAME ]\n\n");
    printf("Examples:\n  view mickey bigmickey.txt\n  view ./wolf\n\n");
    return -1;
  }
  return p_parse(args[1], args[2]);
}
