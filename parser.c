#include "parser.h"
#include <stddef.h>   // C90 NULL macro
#include <dirent.h>   // open files and stuff
#include <stdio.h>    // perror
#include <stdlib.h>   // malloc, free
#include <string.h>   // strlen
#include <math.h>     // strtol
#include <inttypes.h> // long type?

int p_parse(char* path, void* out) {
  struct Parser p = { 0, 0, NULL, NULL };

  // Directory pointer, so we can scan the files for x/y/name data
  DIR *d_pointer = opendir(path);
  if (d_pointer == NULL) {
    printf("Unable to open directory '%s'\n", path);
    perror("Error: ");
    return -1;
  }

  if (out != NULL) {
    // assign the user specified name, so p_scan_filenames wont parse out
    // the name from the filenames
    p.name = (char*)out;
  }

  int r_signal = 0;
  if ((r_signal = p_scan_filenames(&p, d_pointer)) == 0) {
    printf("> X:%d Y:%d name:%s\n", p.x, p.y, p.name);

    FILE* f_out = fopen(p.name, "w+");
    if (f_out == NULL) {
      printf("Unable to open file '%s'\n", p.name);
      perror("Error: ");
      return -1;
    }

    fclose(f_out);
  }

  closedir(d_pointer);

  // cleanup anything stored in the heap
  if (out == NULL) {
    free(p.name);
  }

  return r_signal;
}

int p_scan_filenames(struct Parser* p, DIR* d_pointer) {
  int f_length;
  char* x_ptr = NULL;
  char* y_ptr;

  struct dirent *f_entry;
  // scan each file in the gived directory
  while ((f_entry = readdir(d_pointer)) != NULL) {
    printf("> file: %s\n", f_entry->d_name);
    f_length = strlen(f_entry->d_name);

    // skipping . and .. files
    // the minimun filename given the file naming pattern: N_X_Y.txt is 10
    if (f_length < 10) continue;

    // find the position of the X value in the filename
    // only need to do this once, since we assume all files are formatted equally
    if (x_ptr == NULL) {
      for (int i = 1;; i++) {
        if (i >= f_length) {
          // we didnt find any underscores, this is bad
          printf("Unable to find X coordinate in filename: %s\n", f_entry->d_name);
          return -1;
        } else if (f_entry->d_name[i] == '_') {
          x_ptr = &(f_entry->d_name[i]) + 1;
          break;
        }
      }
    }

    // converting the X and Y values to int
    // storing the max value of each of them, since we only need to know the depth
    // ** could check to integrity of the grid, I guess, but we assume its correct
    p->x = (int)fmaxl((long)p->x, strtol(x_ptr, &y_ptr, 10));
    // x_ptr points the the position in the filename where the X value starts
    // when reading the X value with strtol, we update the y_ptr to the position after X ends
    // we also move the y_ptr to the end of the y value, to grabe the name (which comes after the Y value)
    p->y = (int)fmaxl((long)p->y, strtol(y_ptr + 1, &y_ptr, 10));

    // if the user didnt pass along a name for the resulting output file
    // we scan and use the name on the file
    if (p->name == NULL) {
      // figuring out the length of the name portion of filename;
      // given the total length of the filename, and the y_ptr position
      int n_length = f_length - (int)(y_ptr - f_entry->d_name);
      p->name = malloc(sizeof(char) * n_length);
      memcpy(p->name, y_ptr + 1, n_length);
    }

    printf("> %p %c (%d, %d) %s\n", x_ptr, *x_ptr, p->x, p->y, f_entry->d_name);
  }
  return 0;
}



/*
int p_write(struct Parser* p, void* name) {
  // create a output buffer for combining the data
  // each file has 30 rows and 30 characters per row
  int size = (p->x + 1) * (p->y + 1) * 30 * 30;
  p->buffer = malloc(sizeof(char) * size);
  printf("name: %s %s\n", p->name, name);
  printf("buffer: %d\n", size);

  // create a filename buffer for creating pathnames to each file
  //          PATH       /part_   X                   _   Y                   _   NAME              .txt\0
  int fsize = p->psize + 6      + (log10(p->x) + 1) + 1 + (log10(p->y) + 1) + 1 + strlen(p->name) + 5;
  char filename[fsize];
  for (int i = 0; i < fsize; i++) {
    filename[i] = 'X';
  }
  printf("filename: '%s'\n", filename);

  // char cursor;
  // FILE *fp;
  // fp = fopen(filename, "r");
  // if (fp != NULL) {

  return 0;
}
*/

/*
void p_free(struct Parser* p) {
  if (p->buffer != NULL) {
    printf("free buffer\n");
    free(p->buffer);
    p->buffer = NULL;
  }
  if (p->name != NULL) {
    printf("free name\n");
    free(p->name);
    p->name = NULL;
  }
}
*/

/*
int test(char* filename) {
  char ch;
  FILE *fp;
  fp = fopen(filename, "r");
  if (fp != NULL) {
    int x = 0;
    int y = 0;
    while((ch = fgetc(fp)) != EOF) {
      if (x == 0) {
        if (y < 10) {
          printf(" %d: ", y);
        } else {
          printf("%d: ", y);
        }
        y++;
      }
      printf("%c", ch);
      x++;
      if (x >= 30) {
        printf("\n");
        x = 0;
      }
    }
    fclose(fp);
  } else {
    perror("Error opening file");
    return -1;
  }
  return 0;
}
*/
