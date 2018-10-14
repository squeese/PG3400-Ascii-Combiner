#include "parser.h"
#include <stddef.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

int p_parse(const char* in_arg, const char* out_arg) {
  int max_x = 0;
  int max_y = 0;
  char* name = NULL;

  // First we open the directory and scan the filenames for: max_x, max_y
  // If needed we also get the name
  if (p_scan(in_arg, &max_x, &max_y, &name) != 0) {
    free(name);
    return -1;
  }

  // File descriptors for the output file
  FILE* out_fd;
  char* out_path;
  if (p_open_output(&out_fd, &out_path, out_arg, name) != 0) {
    free(name);
    free(out_path);
    return -1;
  }

  printf(" x:%d y:%d name:%s out_path:'%s' \n", max_x, max_y, name, out_path);

  // String buffer for input file pathnames
  int in_path_len = strlen(in_arg) + log10(max_x) + log10(max_y) + strlen(name) + 16;
  char* in_path_buffer = (char*) malloc(in_path_len * sizeof(char));

  // String buffer for output buffer when writing to files
  int out_write_len = 30 * (max_x + 1);
  char* out_write_buffer = malloc(out_write_len * sizeof(char));

  // File descriptors for the input files
  // We will open one for each 'column',
  // as we need to alternate the read from the columns for the correct order
  FILE** in_fds = calloc(max_x + 1, sizeof(FILE*));

  // Do the actually knitting of ascii textiskskcs
  int x, y, s;
  for (y = 0; y <= max_y; y++) {
    // open files for current column
    for (x = 0; x <= max_x; x++) {
      snprintf(in_path_buffer, in_path_len, "%s/part_%d_%d_%s.txt", in_arg, x, y, name);
      if ((in_fds[x] = fopen(in_path_buffer, "r")) == NULL) {
        printf("Unable to open input file (col:%d) '%s' \n", max_x, in_path_buffer);
        perror("Error: ");
        fclose(out_fd);
        for (int i = 0; i < x; i++) {
          fclose(in_fds[i]);
        }
        free(name);
        free(out_path);
        free(in_fds);
        free(in_path_buffer);
        free(out_write_buffer);
        return -1;
      }
    }

    for (s = 0; s < 30; s++) {
      // Read from the input files
      for (x = 0; x <= max_x; x++) {
        // for all 30 lines, fill one buffer with 30 characters from each file
        fread(out_write_buffer + (x * 30), sizeof(char), 30, in_fds[x]);
      }
      // write that buffer out to the output file
      fwrite(out_write_buffer, sizeof(char), out_write_len, out_fd);
      fputc('\r', out_fd);
      fputc('\n', out_fd);
    }

    // close column pointers
    for (x = 0; x <= max_x; x++) {
      fclose(in_fds[x]);
    }
  }

  // cleanup
  fclose(out_fd);
  free(name);
  free(out_path);
  free(in_fds);
  free(in_path_buffer);
  free(out_write_buffer);

  return 0;
}

int p_scan(const char* path, int* const x, int* const y, char** name) {
  DIR *dir = opendir(path);
  if (dir == NULL) {
    printf("Unable to open directory '%s'\n", path);
    return -1;
  }
  char* x_ptr = NULL;
  char* y_ptr;
  struct dirent *entry;
  int length;
  while ((entry = readdir(dir)) != NULL) {
    length = strlen(entry->d_name);
    // skipping . and .. files
    // the minimun filename given the file naming pattern: N_X_Y.txt is 10
    if (length < 10) continue;
    // set the position of the X value in the filename
    x_ptr = entry->d_name + 5;
    // converting the X and Y values to int
    // storing the max value of each of them, since we only need to know the depth
    // ** could check to integrity of the grid, I guess, but we assume its correct
    *x = (int)fmaxl((long)*x, strtol(x_ptr, &y_ptr, 10));
    // x_ptr points the the position in the filename where the X value starts
    // when reading the X value with strtol, we update the y_ptr to the position after X ends
    // we also move the y_ptr to the end of the y value, to grabe the name (which comes after the Y value)
    *y = (int)fmaxl((long)*y, strtol(y_ptr + 1, &y_ptr, 10));
    // scan for the name on the file
    if (*name == NULL) {
      // figuring out the length of the name portion of filename;
      // given the total length of the filename, and the y_ptr position
      int size = length - (int)(y_ptr - entry->d_name) - 5;
      *name = (char*) malloc(sizeof(char) * size);
      memcpy(*name, y_ptr + 1, size);
    }
  }
  closedir(dir);
  return 0;
}

int p_open_output(FILE** fd, char** path, const char* out, const char* name) {
  if (out != NULL) {
    int len = strlen(out);
    *path = calloc(len, sizeof(char));
    memcpy(*path, out, len);
  } else {
    // use the output filename parsed from the filenames
    int len = strlen(name) + 5;
    *path = malloc(sizeof(char) * len);
    snprintf(*path, len, "%s.txt", name);
  }
  if ((*fd = fopen(*path, "w+")) == NULL) {
    printf("Unable to open output file '%s' \n", *path);
    perror("Error: ");
    return -1;
  }
  return 0;
}
