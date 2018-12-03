#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

int main() {
  int fds[2];
  pipe(fds);
  if (!fork()) {
    close(fds[0]);
    dup2(fds[1], STDOUT_FILENO);
    char** args = calloc(sizeof(char*), 2);
    args[0] = "ls";
    execvp(args[0], args);
    exit(0);
  }
  else if (!fork()) {
    close(fds[1]);
    dup2(fds[0], STDIN_FILENO);
    char** args = calloc(sizeof(char*), 2);
    args[0] = "wc";
    execvp(args[0], args);
    // char* args = calloc(sizeof(char), 1024);
    // fgets(args, 1023, stdin);
    // char* p = strrchr(args, '\n');
    // if (p) *p = 0;
    // printf("this is what i got: %s\n", args);
    exit(0);
  }
  else {
    int status;
    wait(&status);
    printf("main done\n");
  }
}
