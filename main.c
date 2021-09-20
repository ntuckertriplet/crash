#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "signal.h"

int main(int argc, char **argv)
{
  int opt = -1;
  char* prompt = "308sh> ";

  while ((opt = getopt(argc, argv, "hp:")) != -1) {
    switch (opt) {
      case 'p':
        prompt = optarg;
        break;
      case 'h':
        fprintf(stdout, "usage: shell [-p 'prompt']\n");
        exit(-1);
      default:
        exit(-1);
      break;
    }
  }

  int run = 1;
  while(run)
  {
    signal(SIGINT, signal_handler); // we want to catch ^C

    // starting malloc size for command line args, to be realloc'ed later if needed
    int malloc_size = 2;
    char** commands = malloc(sizeof(char*) * malloc_size);

    // don't interrupt the input/output buffer
    usleep(50);
    fprintf(stdout, "%s", prompt);

    char* input = NULL;
    size_t len = 0;
    getline(&input, &len, stdin);
    
    // snag the first item in the line (possible it's the first)
    char* input_token = strtok(input, " ");
    int num_inputs = 0; // num inputs will always be +1 because 0 index
    while(input_token != NULL)
    {
      commands[num_inputs] = strdup(input_token);
      commands[num_inputs][strcspn(commands[num_inputs], "\n")] = 0; // replace newline with '\0'
      num_inputs += 1;
      if (num_inputs == malloc_size) // if we reached the max supported size
      {
        void* _ = realloc(commands, malloc_size * 2);
        if (_ == NULL)
        {
          fprintf(stderr, "memory error in command parsing, exiting\n");
          exit(-1);
        }
        malloc_size *= 2;
      }

      input_token = strtok(NULL, " ");
    }

    /* Builtin commands to support */
    if (strncmp(commands[0], "exit", 4) == 0) run = 0;
    else if (strncmp(commands[0], "quit", 4) == 0) fprintf(stderr, "did you mean 'exit'?\nx ");
    else if (strncmp(commands[0], "pid", 3) == 0) fprintf(stdout, "%d\n", getpid());
    else if (strncmp(commands[0], "ppid", 4) == 0) fprintf(stdout, "%d\n", getppid());
    else if (strncmp(commands[0], "pwd", 3) == 0)
    {
      char cwd[256];
      if (getcwd(cwd, sizeof(cwd)) != NULL) fprintf(stdout, "%s\n", cwd);
      else fprintf(stderr, "error fetching current working directory\nx ");
    }
    else if (strncmp(commands[0], "cd", 2) == 0) // if "cd" appears _at all_ at the beginning
    {
      if (num_inputs == 1) // if no arg passed in, just cd, just go $HOME
      {
        char* home = getenv("HOME");
        if (home == NULL) fprintf(stderr, "error fetching $HOME. has it been set?\nx ");
        else chdir(home);
      }
      else // we need to chdir() to the arg passed in
      {
        if (chdir(commands[1]) == -1) fprintf(stderr, "no such file or directory:%s\nx ", commands[1]);
      }
    }
    /* End builtins */

    /* cleanup */
    int i;
    for (i = 0; i < num_inputs; i++) free(commands[i]);
  }

  return 0;
}