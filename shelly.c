/*
  Joan Chirinos
  Shell
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include "shelly.h"

/*
parse_args

Takes char* args line and parse_at and splits line by parse_at into char**

params:
  char* line
  char* parse_at
returns:
  char**
*/

void print_charss(char** lines) {
  while (*lines){
    printf("|%s|\n", *lines);
    lines++;
  }
}

char* trim(char* to_trim) {
  char* start = to_trim;
  while (*start == ' ') {
    start++;
  }
  char* end = to_trim;
  while (*end) {
    end++;
  }
  end--;
  while (*end == ' ') {
    *end = 0;
    end--;
  }
  return start;
}

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

/*
parse_arg_array

Takes

params:
  char** arg_array
  char*  parse_at

returns:
  char**
*/
char** parse_arg_array(char** arg_array, char* parse_at) {
  char** out = calloc(sizeof(char*), 10);
  int index = 0;
  while (arg_array[index]) {
    printf("%d: ", index);
    printf("%s\n", arg_array[index]);
    int comp = strcmp(arg_array[index], parse_at);
    printf("comp: %d\n", comp);
    if (!comp) {
      char** out = &arg_array[index + 1];
      arg_array[index] = 0;
      return out;
    }
    index++;
  }
  return 0;
}

void print_prompt() {
  char* cwd = getcwd(NULL, 1024);
  // char* current_dir;
  // while (cwd != NULL) {
  //   current_dir = strsep(&cwd, "/");
  // }
  printf("\e[1;5;93;105m%s %s$\e[0m ", cwd, getlogin());
  free(cwd);
}

char* get_args() {
  char* args = calloc(sizeof(char), 1024);
  fgets(args, 1023, stdin);
  char* p = strrchr(args, '\n');
  if (p) *p = 0;
  return args;
}

void run_line(char* arg_line) {
  char* p = strchr(arg_line, '\n');
  if (p) *p = 0;
  char** arg_array = parse_args(arg_line, ";");

  int index = 0;
  while (arg_array[index]) {
    run_single_command(arg_array[index]);
    index++;
  }
}

void run_single_command(char* arg_line) {
  printf("line: %s\n", arg_line);
  char* p = strchr(arg_line, '\n');
  if (p) *p = 0;
  else {
    printf("no new line char\n");
  }
  // if the arg is not cd or exit
  char* cd_line = calloc(sizeof(char), 1024);
  printf("about to copy\n");
  strcpy(cd_line, arg_line);
  printf("done\n");
  if (cd_exit(cd_line)) { printf("not cd or exit\n");return; }
  // if there is |
  char* pipe_line = calloc(sizeof(char), 1024);
  strcpy(pipe_line, arg_line);
  if (pipe_execution(pipe_line)) { printf("not pipe\n");return; }
  // if there is <
  char* ie_line = calloc(sizeof(char), 1024);
  strcpy(ie_line, arg_line);
  if (input_execution(ie_line)) { printf("not <\n");return; }
  // if there is >
  char* oe_line = calloc(sizeof(char), 1024);
  strcpy(oe_line, arg_line);
  if (output_execution(oe_line)) { printf("not >\n");return; }

  // else
  regular_execution(arg_line);

}

char pipe_execution(char* line) {
  int pipefd[2];
  int pid;

  char* left = calloc(sizeof(char), 1000);
  char* right = calloc(sizeof(char), 1000);

  strcpy(left, trim(strsep(&line, "|")));
  strcpy(right, trim(line));

  printf("left: %s\n", left);
  printf("right: %s\n", right);

  // char** ls_args = calloc(sizeof(char*), 2);
  // char** wc_args = calloc(sizeof(char*), 2);
  //
  // ls_args[0] = "ls";
  // wc_args[0] = "wc";

  pipe(pipefd);

  pid = fork();

  if (pid == 0) {
    dup2(pipefd[0], 0);
    close(pipefd[1]);
    run_single_command(right);
  }
  else {
    dup2(pipefd[1], 1);
    close(pipefd[0]);
    run_single_command(left);
  }
}

char output_execution(char* line) {
  if (!strchr(line, '>')) { return 0; }
  char** args = parse_args(line, ">");
  if (!fork()) {
    int index = 1;
    int fd;
    char* filename;
    while (args[index] && index < 101) {
      filename = trim(args[index]);
      fd = open(filename, O_WRONLY | O_CREAT, 0644);
      if (fd == -1) {
        printf("%s\n", strerror(errno));
        return 1;
      }
      index++;
    }
    dup2(fd, STDOUT_FILENO);
    char** commands = parse_args(args[0], " ");
    //works up to here ^

    int error = execvp(commands[0], commands);
    if (error == -1) {
      printf("|%s| : That looks a lot like not a command xD\n", args[0]);
      printf("%s\n", strerror(errno));
    }
    return 1;
  }
  else {
    int status;
    wait(&status);
    return 1;
  }
}

char input_execution(char* line) {
  if (!strchr(line, '<')) { return 0; }
  char** args = parse_args(line, "<");
  if (!fork()) {
    int index = 1;
    int fd;
    char* filename;
    while (args[index] && index < 101) {
      filename = trim(args[index]);
      fd = open(filename, O_RDONLY);
      if (fd == -1) {
        printf("%s\n", strerror(errno));
        return 1;
      }
      index++;
    }
    dup2(fd, STDIN_FILENO);
    char* command = trim(args[0]);
    char** rest = calloc(sizeof(char*), 2);
    rest[0] = command;
    rest[1] = NULL;

    int error = execvp(command, rest);
    if (error == -1) {
      printf("|%s| : That looks a lot like not a command xD\n", args[0]);
      printf("%s\n", strerror(errno));
    }
    return 1;
  }
  else {
    int status;
    wait(&status);
    return 1;
  }
}

void regular_execution(char* arg_line) {
  printf("got to regular exec\n");
  char** arg_array = parse_args(arg_line, " ");
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

char cd_exit(char* arg_line) {
  char** arg_array = parse_args(arg_line, " ");
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
