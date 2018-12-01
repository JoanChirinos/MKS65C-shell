/*
  Joan Chirinos
  Shell
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "shelly.h"

char** parse_args(char* line, char* parse_at) {
  char** args = calloc(sizeof(char*), 101);
  int index = 0;
  char* token;
  while (line != NULL && index < 100) {
    token = strsep(&line, parse_at);
    // ignores extra spaces
    if (strcmp(token, "")) {
      args[index] = token;
      index++;
    }
  }
  return args;
}

char** parse_arg_array(char** arg_array, char* parse_at) {
  int index = 0;
  while (arg_array[index]) {
    if (!strcmp(arg_array[index], parse_at)) {
      char** out = &arg_array[index + 1];
      arg_array[index] = 0;
      return out;
    }
  }
  return 0;
}

void print_prompt() {
  char* cwd = getcwd(NULL, 1024);
  char* current_dir;
  while (cwd != NULL) {
    current_dir = strsep(&cwd, "/");
  }
  printf("%s %s$ ", current_dir, getlogin());
  free(cwd);
}

char* get_args() {
  char* args = calloc(sizeof(char), 1024);
  fgets(args, 1023, stdin);
  char* p = strrchr(args, '\n');
  if (p) *p = 0;
  return args;
}

// stdin fd = STDIN_FILENO

void run_line(char* arg_line) {
  char* p = strchr(arg_line, '\n');
  if (p) *p = 0;
  char** arg_array = parse_args(arg_line, " ");

  // if the arg is not cd or exit
  if (!cd_exit(arg_array)) {
    // if there is |

    // if there is <
    char** input = parse_arg_array(arg_array, "<");
    if (!input) {
      input_execution(arg_array, input);
    }
    // if there is >

    // else
    regular_execution(arg_array);
  }
}

void input_execution(char** commands, char** in) {
  if (!fork()) {

  }
}

void regular_execution(char** arg_array) {
  if (!fork()) {
    int error = execvp(arg_array[0], arg_array);
    if (error == -1) {
      printf("%s : That looks a lot like not a command xD\n", arg_array[0]);
    }
    return;
  }
  else {
    int status;
    wait(&status);
  }
}

char cd_exit(char** arg_array) {
  if (!strcmp(arg_array[0], "exit")) {
    free(arg_array);
    exit(0);
  }
  else if (!strcmp(arg_array[0], "cd")) {
    int error = chdir(arg_array[1]);
    if (error == -1) {
      printf("%s\n", strerror(errno));
    }
    return 1;
  }
  return 0;
}

/*
  NOTE:
  FOR PIPING, FORK, CHILD_PIPE, CHILD_FORK. WHEN CHILD DIES, MAIN FILE TABLE DOESNT MESS UP
*/

int main() {
  while (1) {
    print_prompt();

    char* line = get_args();
    run_line(line);

  }
}
