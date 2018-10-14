#include "parser.h"
#include <stdio.h>

int main(int count, char* args[]) {
  if (count < 2) {
    printf("\nError: Must provide path to the folder with ascii art to be processed\n");
    printf("  view PATH_FOLDER [ OUT_FILENAME ]\n\n");
    printf("Examples:\n  view ./mickey bigmickey.txt\n  view ./wolf\n\n");
    return -1;
  }
  printf("\n view %s %s\n\n", args[1], args[2]);
  return p_parse("./wolf", "out.txt");
}
