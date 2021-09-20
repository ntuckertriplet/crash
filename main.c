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

    char input[256]; // initialize a buffer for user input

    usleep(50);
    fprintf(stdout, "%s", prompt);

    int i;
    for (i = 0; i < 256; i++)
    {
      int c = fgetc(stdin);
      if (c == EOF || c == '\n')
      {
        input[i] = '\0';
        break;
      }

      input[i] = c;
    }

    /* Builtin commands to support */
    if (strncmp(input, "exit", 4) == 0) run = 0;
    else if (strncmp(input, "quit", 4) == 0) fprintf(stderr, "did you mean 'exit'?\nx ");
    else if (strncmp(input, "pid", 3) == 0) fprintf(stdout, "%d\n", getpid());
    else if (strncmp(input, "ppid", 4) == 0) fprintf(stdout, "%d\n", getppid());
    else if (strncmp(input, "pwd", 3) == 0)
    {
      char cwd[256];
      if (getcwd(cwd, sizeof(cwd)) != NULL)
      {
        fprintf(stdout, "%s\n", cwd);
      }
      else
      {
        fprintf(stderr, "error fetching current working directory\nx ");
      }
    }
    else if (strncmp(input, "cd", 2) == 0) // if "cd" appears _at all_
    {
      if (strcmp(input, "cd") == 0) // if no arg passed in, just go $HOME
      {
        char* home = getenv("HOME");
        if (home == NULL)
        {
          fprintf(stderr, "error fetching $HOME. has it been set?\nx ");
        }
        else
        {
          chdir(home);
        }
      }
      else // we need to chdir() to the arg passed in
      {
        char* token = strtok(input, " "); // this will grab "cd"
        if (token == NULL)
        {
          fprintf(stderr, "error in parsing, maybe try again?\nx ");
        }

        token = strtok(NULL, " "); // this should grab the relative or absolute directory
        int changed = chdir(token);
        if (changed == -1) // no path exists for this, sorry!
        {
          fprintf(stderr, "no such file or directory: %s\nx ", token);
        }
      }
    }
    /* End builtins */
  }

  return 0;
}