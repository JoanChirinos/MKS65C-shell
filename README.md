# Joan's Wonderful-est Shell

## Features
- Prompt contains username and cwd, in flashy colors
- Runs simple bash commands
  - plain commands, such as ```ls```, ```ls -al```
  - ```cd``` and ```exit```
  - commands including stdin redirection, such as ```wc < input.txt``` and ``` wc < nothing.txt < input.txt```
  - commands including stdout redirection, such as ```ls > output.txt``` and ```ls > nothing.txt > output.txt```
  - commands including a single pipe, such as ```ls | wc```
  - chains of commands separated by semicolons, such as ```ls; ls | wc; ls > output.txt```
  - ```cd``` and ```exit```
- Ignores extra whitespace between tokens

## Failes/In Progress Features
- Multiple pipes in the same command
- Using ```>``` and ```<``` in the same command
- Tab completion
- History

## Bugs and Stuff
- User cannot use ```>``` and ```<``` in the same command
- Piping only works with simple commands

## Functions
- **trim** - ```char* trim(char* to_trim)```
  - description:
    - Trims whitespace from start end end of char* to_trim
  - params:
    - char* to_trim : the char* being trimmed
  - returns:
    - char* : the trimmed char*
    
- **parse_args** - ```char** parse_args(char* line, char* parse_at)```
  - description:
    - Splits a line by a char* delimiter, removing extra copies of the delimeter
  - params:
    - char* line      : the line to be split
    - char* parse_at  : the delimiter
  - returns:
    - char**  : array of strings after splitting
    
- **print_prompt** - ```void print_prompt()```
  - description:
    - Prints the joan-shell prompt
  - params:
    - None
  - returns:
    - None
    
- **get_args** - ```char* get_args()```
  - description:
    - Takes user input using fgets and replaces \n with NULL
  - params:
    - None
  - returns:
    - char* : the input
    
- **run_line** - ```void run_line(char* arg_line)```
  - description:
     - Takes a line of arguments, splits them by semicolon, then runs each line
  - params:
    - char* arg_line  : the line of arguments
  - returns:
    - None
    
- **run_single_command** - ```void run_single_command(char* arg_line)```
  - description:
    - Runs simple commands (sans semicolons). Has different cases for commands with <, >, |, and commands without those
  - params:
    - char* arg_line  : the simple command to be run

  - returns:
    - None
    
- **pipe_execution** - ```char pipe_execution(char* line)```
  - description:
    - Attempts to execute a command with a pipe
  - params:
    - char* line  : the command to be ran
  - returns:
    - char  : 1 if successful, 0 if not
    
- **output_execution** - ```char output_execution(char* line)```
  - description:
    - Attempts to execute a command that redirects output into a file (>)
  - params:
    - char* line  : the command to be ran
  - returns:
    - char  : 1 if successful, 0 if not
    
- **input_execution** - ```char input_execution(char* line)```
  - description:
    - Attempts to execute a command that redirects input from a file (<)
  - params:
    - char* line  : the command to be ran
  - returns:
    - char  : 1 if successful, 0 if not
    
- **regular_execution** - ```void regular_execution(char* arg_line)```
  - description:
    - Attempts to execute a simple command
  - params:
    - char* line  : the command to be ran
  - returns:
    - char  : 1 if successful, 0 if not
    
- **cd_exit** - ```char cd_exit(char* arg_line)```
  - description:
    - Attemps to cd to a different directory, or exit the shell
  - params:
    - char* line  : the command to be ran
  - returns:
    - char  : 1 if successful, 0 if not
