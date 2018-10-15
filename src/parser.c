#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

int p_parse(char* in_arg, char* out_arg) {
  int max_x = 0;
  int max_y = 0;
  char* name = NULL;

  // First we open and scan the files in the given directory
  // user passed via the first arguments.
  // We scan the filenames for the largest X and Y value, and the name.
  // The format of the files follow this format: part_X_Y_NAME.txt
  if (p_scan(in_arg, &max_x, &max_y, &name) != 0) {
    free(name);
    return EXIT_FAILURE;
  }
  // printf("\n> name:%s x:%d y:%d", name, max_x, max_y);

  // Create a file descriptor for the output buffer (file) were going
  // to store the resulting patched ASCII art in.
  // The path to the file can be passed from the user as the secord argument
  // in the command line, but of none is passed, we default to 'NAME.txt',
  // name parsed from the filenames earlier.
  FILE* out_fd;
  char* out_path;
  if (p_open_output(&out_fd, &out_path, out_arg, name) != 0) {
    free(name);
    free(out_path);
    return EXIT_FAILURE;
  }
  // printf("\n> out_path:%s", out_path);

  // String buffer for input file pathnames
  // When opening file descriptors for each input file, we need to pass the path,
  // we can reuse the same string buffer for the pathname, we just have to know the max
  // length the filename can be, given the NAME, X and Y values.
  // Note: not strictly neccesary, given the example input, but a fun excercise, it was.
  int in_path_len = strlen(in_arg) + p_num_digits(max_x) + p_num_digits(max_y) + strlen(name) + 16;
  char* in_path_buffer = (char*) malloc(in_path_len * sizeof(char));

  // String buffer for we store chunks for data in when reading from files,
  // which is then used to write to the output file.
  // Since we write an entire 'line' of the ascii image at each write, we can use
  // the X value to correctly determine the needed size of the buffer:
  //  30 characters * amount of columns;
  int out_write_len = 30 * (max_x + 1);
  char* out_write_buffer = malloc(out_write_len * sizeof(char));

  // File descriptors for the input files
  // We need to have an open input file descriptor for each 'column' (X value) in the
  // ascii grid.
  FILE** in_fds = malloc((max_x + 1) * sizeof(FILE*));

  // Here we do the actuall weaving of the ASCII image.
  // We loop over the grid in the order of row, column
  int x, y, s;
  for (y = 0; y <= max_y; y++) {
    // We create a input file descriptor for each file in current cell (column)
    for (x = 0; x <= max_x; x++) {
      snprintf(in_path_buffer, in_path_len, "%s/part_%d_%d_%s.txt", in_arg, x, y, name);
      if ((in_fds[x] = fopen(in_path_buffer, "r")) == NULL) {
        printf("Unable to open input file (col:%d) '%s' \n", max_x, in_path_buffer);
        perror("Error: ");
        // Stuff went wrong when opening one of the input files, make sure we clean stuff up
        // before exiting.
        fclose(out_fd);
        free(name);
        free(out_path);
        free(in_fds);
        free(in_path_buffer);
        free(out_write_buffer);
        for (int i = 0; i < x; i++)
          fclose(in_fds[i]);
        return EXIT_FAILURE;
      }
    }

    // Now that we got input files for each cell, we loop 30 times (the line/height count)
    // and take 30 characters from each file into the output buffer, arranged in the order
    // after each other
    for (s = 0; s < 30; s++) {
      // Read 30 characters from each of the input files
      for (x = 0; x <= max_x; x++) 
        fread(out_write_buffer + (x * 30), sizeof(char), 30, in_fds[x]);
      // Then write the completed row to the output file, with newlines
      fwrite(out_write_buffer, sizeof(char), out_write_len, out_fd);
      fputc('\r', out_fd);
      fputc('\n', out_fd);
    }

    // We are finnished with all the files in the current row, so we have to close all
    // the files, and make ready for next iteration to open new files. Rince repeat.
    for (x = 0; x <= max_x; x++) {
      fclose(in_fds[x]);
    }
  }

	// Success
  printf("\n DONE -> %s\n\n", out_path);

  // Cleanup.
  fclose(out_fd);
  free(name);
  free(out_path);
  free(in_fds);
  free(in_path_buffer);
  free(out_write_buffer);
  return EXIT_SUCCESS;
}

int p_scan(char* path, int* x, int* y, char** name) {
  DIR *dir = opendir(path);
  if (dir == NULL) {
    printf("Unable to open directory '%s'\n", path);
    return EXIT_FAILURE;
  }
  // These pointers are used to point to the X and Y positions in the filename string:
  char* x_ptr = NULL;
  char* y_ptr;
  struct dirent *entry;
  int length;
  while ((entry = readdir(dir)) != NULL) {
    length = strlen(entry->d_name);
    // Skipping the . and .. files
    // The minimun filename given the file naming pattern: part_X_Y_N.txt is 15
    if (length < 15) continue;
    // Set the position of the X value in the filename,
    // we know its always 5 characters from the beginning
    x_ptr = entry->d_name + 5;
    // We then read and convert the value from string to int,
    // but we will only store the largest value we can find, since its the only one that matters
    // Note: could check integrity of the grid, I guess, but we assume its correct, the examples are.
    *x = p_max(*x, (int)strtol(x_ptr, &y_ptr, 10));
    // When reading the X value with strtol, we can pass along a ptr that will point to the position after
    // the value read was converted to int (the x value), we now know where the Y value starts.
    *y = p_max(*y, (int)strtol(y_ptr + 1, &y_ptr, 10));
    // We use the same fact, to move the y_ptr to the end of the Y value, so we know the position of the NAME
    // value aswell.
    if (*name == NULL) {
      // In order to read the name value, we need to know the size of it, we can deduce that with
      // the given: (1: total length of the filename) and (2: the y_ptr position)
      // (length of filename) - (y_ptr offset from beginning of filename) - (length of trailing ".txt" text)
      int size = length - (int)(y_ptr - entry->d_name) - 5;
      *name = (char*) malloc(sizeof(char) * size);
      memcpy(*name, y_ptr + 1, size);
    }
  }
  closedir(dir);
  return EXIT_SUCCESS;
}

int p_open_output(FILE** fd, char** path, char* out, char* name) {
  // When creating a output file descriptor, we need to figure out the pathname of the file.
  if (out != NULL) {
    // If the user passes in a value as the second argument in command line, we use that one.
    int len = strlen(out);
    *path = malloc(len * sizeof(char));
    memcpy(*path, out, len);
    // Need to trim the whitespace off the end of the string, commandline args can tag on the \n it seems
    // https://gist.github.com/kenkam/790090
    // Create a char pointer to the end of the string
    char *end = *path + len - 1;
    // Move it backwards while it finds a whitespace
    while (end > *path && isspace(*end)) end--;
    // When it moved past all whitespace, add null terminator after current character
    *(end+1) = '\0';
  } else {
    // Or use the name parsed from the filenames as the base for output filename.
    // We need to add .txt to the extension aswell.
    int len = strlen(name) + 5;
    *path = malloc(sizeof(char) * len);
    snprintf(*path, len, "%s.txt", name);
  }
  // Lets try create a file at the deduced path, fingers crossed
  if ((*fd = fopen(*path, "w+")) == NULL) {
    printf("Unable to open output file '%s' \n", *path);
    perror("Error: ");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int p_max(int a, int b) {
  return a > b ? a : b;
}

int p_num_digits(int value) {
  return (int)log10((double) value);
}
