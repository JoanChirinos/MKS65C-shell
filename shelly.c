/*
  Joan Chirinos
  Shell

  Huge thanks to Camilla Cheng for help and emotional support cuz this project
  made me wanna cry
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include "shelly.h"

/*========================================

  trim

  Trims whitespace from start end end of char*

  params:
    char* to_trim : the char* being trimmed

  returns:
    char* : the trimmed char*

========================================*/
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

/*========================================

  parse_args

  Splits a line by a char* delimiter, removing extra copies of the delimeter
  Ex. parse_args("hello   I am   david", " ") --> ["hello", "I", "am", "david"]

  params:
    char* line      : the line to be split
    char* parse_at  : the delimiter

  returns:
    char**  : array of strings after splitting

========================================*/

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

/*========================================

  print_prompt

  Prints the joan-shell prompt

  params: None

  returns: None

========================================*/

void print_prompt() {
  char* cwd = getcwd(NULL, 1024);
  // char* current_dir;
  // while (cwd != NULL) {
  //   current_dir = strsep(&cwd, "/");
  // }
  printf("\e[1;5;93;105m%s %s$\e[0m ", cwd, getlogin());
  free(cwd);
}

/*========================================

  get_args

  Takes user input using fgets and replaces \n with NULL

  params: None

  returns:
    char* : the input

========================================*/

char* get_args() {
  char* args = calloc(sizeof(char), 1024);
  fgets(args, 1023, stdin);
  char* p = strrchr(args, '\n');
  if (p) *p = 0;
  return args;
}

/*========================================

  run_line

  Takes a line of arguments, splits them by semicolon, then runs each line
  Ex. ls -al; ls | wc ;  ls > contents.txt

  params:
    char* arg_line  : the line of arguments

  returns: None

========================================*/

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

/*========================================

  run_single_command

  Runs simple commands (sans semicolons). Has different cases for commands with
  <, >, |, and commands without those
  Ex. ls | wc

  params:
    char* arg_line  : the simple command to be run

  returns: None

========================================*/

void run_single_command(char* arg_line) {
  char* p = strchr(arg_line, '\n');
  if (p) *p = 0;
  // if the arg is not cd or exit
  char* cd_line = calloc(sizeof(char), 1024);
  strcpy(cd_line, arg_line);
  if (cd_exit(cd_line)) { return; }
  // if there is |
  char* pipe_line = calloc(sizeof(char), 1024);
  strcpy(pipe_line, arg_line);
  if (pipe_execution(pipe_line)) { return; }
  // if there is <
  char* ie_line = calloc(sizeof(char), 1024);
  strcpy(ie_line, arg_line);
  if (input_execution(ie_line)) { return; }
  // if there is >
  char* oe_line = calloc(sizeof(char), 1024);
  strcpy(oe_line, arg_line);
  if (output_execution(oe_line)) { return; }

  // else
  regular_execution(arg_line);

}

/*========================================

  pipe_execution

  Attempts to execute a command with a pipe
  Ex. ls | wc

  params:
    char* line  : the command to be ran

  returns:
    char  : 1 if successful, 0 if not

========================================*/

char pipe_execution(char* line) {
  if (line[0] == '|') {
    printf("| : That looks a lot like not a command xD\n");
    return 1;
  }
  if (!strchr(line, '|')) { return 0; }
  if (!fork()) {
    int fds[2];
    pipe(fds);

    char** args = parse_args(line, "|");

    char** left = parse_args(args[0], " ");
    char** right = parse_args(args[1], " ");

    if (fork()) {
      close(fds[0]);
      dup2(fds[1], STDOUT_FILENO);
      execvp(left[0], left);
      return 1;
    }
    else {
      wait(NULL);
      close(fds[1]);
      dup2(fds[0], STDIN_FILENO);
      execvp(right[0], right);
      return 1;
    }
  }
  else {
    wait(NULL);
    return 1;
  }
  return 1;
}

/*========================================

  output_execution

  Attempts to execute a command that redirects output into a file (>)
  Ex. ls > output.txt

  params:
    char* line  : the command to be ran

  returns:
    char  : 1 if successful, 0 if not

========================================*/

char output_execution(char* line) {
  if (line[0] == '>') {
    printf("> : That looks a lot like not a command xD\n");
    return 1;
  }
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

    int error = execvp(commands[0], commands);
    if (error == -1) {
      printf("%s : That looks a lot like not a command xD\n", args[0]);
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

/*========================================

  input_execution

  Attempts to execute a command that redirects input from a file (<)
  Ex. wc < input.txt

  params:
    char* line  : the command to be ran

  returns:
    char  : 1 if successful, 0 if not

========================================*/

char input_execution(char* line) {
  if (line[0] == '<') {
    printf("< : That looks a lot like not a command xD\n");
    return 1;
  }
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
    char** stuff = parse_args(args[0], " ");

    int error = execvp(stuff[0], stuff);
    if (error == -1) {
      printf("%s : That looks a lot like not a command xD\n", args[0]);
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

/*========================================

  regular_execution

  Attempts to execute a simple command
  Ex. ls -al

  params:
    char* line  : the command to be ran

  returns:
    char  : 1 if successful, 0 if not

========================================*/

void regular_execution(char* arg_line) {
  char** arg_array = parse_args(arg_line, " ");
  int index = 0;
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

/*========================================

  cd_exit

  Attemps to cd to a different directory, or exit the shell

  params:
    char* line  : the command to be ran

  returns:
    char  : 1 if successful, 0 if not

========================================*/

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

int main() {
  while (1) {
    print_prompt();

    char* line = get_args();
    run_line(line);

  }
}
