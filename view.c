#include "parser.h"
#include <stdio.h>
// #include <string.h>
// #include <math.h>
// #include <inttypes.h>
// #include <stdlib.h>


int main(int count, char* args[]) {
  printf("args: %d\n", count);
  for (int i = 0; i < count; i++) {
    printf("%d: %s\n", i, args[i]);
  }

  if (count < 2) {
    printf("> Error: Must provide folder name of ascii art to be processed");
    printf("> view [ mickey | wolf ]\n");
    return -1;
  }

  return p_parse("./mickey", NULL);

  // if (test("./wolf/part_0_0_wolf.txt") != 0) return -1;
  // if (test("./wolf/part_0_1_wolf.txt") != 0) return -1;

  /*
  int result;
  struct Parser p = {};
  if ((result = p_initialize(&p, "./mickey")) == 0) {
    result = p_write(&p, NULL);
  }
  p_free(&p);
  return result;
  */

  /*
  char dirLength = strlen(args[1]);
  char dirName[dirLength+ 3];
  dirName[0] = '.';
  dirName[1] = '/';
  memcpy(&dirName[2], args[1], dirLength);
  dirName[dirLength + 2] = '\0';
  printf("'%s'\n", dirName);
  */
}
