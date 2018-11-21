/*
=========
= NOTES =
=========

- Clear arg_array every time rather than freeing it maybe?
  - Only free it on exit

*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

char** parse_args(char* line) {
  char** args = calloc(sizeof(char*), 101);
  int index = 0;
  while (line != NULL && index < 100) {
    args[index] = strsep(&line, " ");
    index++;
  }
  return args;
}

int main() {
  int shell_pid = getpid();
  while (1) {
    // printf("ARGS: ");

    char* cwd = getcwd(NULL, 1024);
    printf("%s: \n", cwd);
    free(cwd);

    // stores input into string @ args
    char* args;
    fgets(args, 1023, stdin);
    char* p = strchr(args, '\n');
    if (p) *p = 0;

    // splits input into char**
    char** arg_array = parse_args(args);

    printf("what I got: %s\n", args);

    // if the input is "exit", then quit shell
    if (!strcmp(arg_array[0], "exit")) {
      free(arg_array);
      exit(1);
    }

    else if (!strcmp(arg_array[0], "cd")) {
      int error = chdir(arg_array[1]);
      if (error == -1) {
        printf("%s\n", strerror(errno));
      }
    }

    // otherwise, spawn a child process and make it run
    if (!fork()) {
      execvp(arg_array[0], arg_array);
      printf("%s\n", strerror(errno));
      return 0;
    }
    else {
      int status;
      wait(&status);
    }

    // free memory
    free(arg_array);
  }
}
