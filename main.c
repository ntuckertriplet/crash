#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "comparator.h"
#include "debug.h"
#include "list.h"
#include "process.h"
#include "signal.h"

int main(int argc, char **argv)
{
  int opt = -1;
  char *prompt = "308sh> ";

  while ((opt = getopt(argc, argv, "hp:")) != -1)
  {
    switch (opt)
    {
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

  linked_list *jobs = malloc(sizeof(linked_list));

  while (1)
  {
    // signal(SIGINT, signal_handler); // we want to catch ^C

    int bg_status = -1;
    int bg_pid = -1;

    /* Check for background processes */
    bg_pid = waitpid(-1, &bg_status, WNOHANG);
    if (bg_pid > 0)
    {
      char *bg_name = NULL;
      node *cur_node = jobs->head;
      while (cur_node != NULL)
      {
        process *p = (process *)cur_node->data;
        if (p->pid == bg_pid)
        {
          bg_name = strdup(p->name);
          break;
        }

        cur_node = cur_node->next;
      }

      fprintf(stderr, "background child ended: ");
      get_status(bg_pid, bg_status, bg_name);
      list_delete(jobs, cur_node->data, proc_comp);
    }
    /* End background checking */
    /* Start taking user input */

    // starting malloc size for command line args, to be realloc'ed later if needed
    int malloc_size = 2;
    char **commands = malloc(sizeof(char *) * malloc_size);

    // don't interrupt the input/output buffer
    usleep(50);
    fprintf(stdout, "%s", prompt);

    char *input = NULL;
    size_t len = 0;
    getline(&input, &len, stdin);

    // snag the first item in the line (possible it's the first)
    char *input_token = strtok(input, " ");
    int num_inputs = 0; // num inputs will always be +1 because 0 index
    while (input_token != NULL)
    {
      commands[num_inputs] = strdup(input_token);
      commands[num_inputs][strcspn(commands[num_inputs], "\n")] = 0; // replace newline with '\0'
      num_inputs += 1;
      if (num_inputs == malloc_size) // if we reached the max supported size
      {
        void *_ = realloc(commands, malloc_size * 2);
        if (_ == NULL)
        {
          fprintf(stderr, "memory error in command parsing, exiting\n");
          exit(-1);
        }
        malloc_size *= 2;
      }

      input_token = strtok(NULL, " ");
    }

#ifdef DEBUG
    fprintf(stdout, "WE GOT %d INPUTS\n", num_inputs);
    for (int j = 0; j < num_inputs; j++)
      fprintf(stdout, "COMMAND AT INDEX %d IS %s\n", j, commands[j]);
#endif

    // check if we need to background it
    int do_background = (strcmp(commands[num_inputs - 1], "&") == 0);
#ifdef DEBUG
    fprintf(stdout, "DO_BACKGROUND STATUS: %d\n", do_background);
#endif
    if (do_background)
      num_inputs -= 1; // this will be important when we go to fork
#ifdef DEBUG
    fprintf(stdout, "NEW NUM INPUTS IS %d\n", num_inputs);
#endif

    /* Builtin commands to support */
    if (strncmp(commands[0], "exit", 4) == 0)
      return 0;
    else if (strncmp(commands[0], "quit", 4) == 0)
      fprintf(stderr, "did you mean 'exit'?\nx ");
    else if (strncmp(commands[0], "pid", 3) == 0)
      fprintf(stdout, "%d\n", getpid());
    else if (strncmp(commands[0], "ppid", 4) == 0)
      fprintf(stdout, "%d\n", getppid());
    else if (strncmp(commands[0], "pwd", 3) == 0)
    {
      char cwd[256];
      if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "%s\n", cwd);
      else
        fprintf(stderr, "error fetching current working directory\nx ");
    }
    else if (strncmp(commands[0], "cd", 2) == 0) // if "cd" appears _at all_ at the beginning
    {
      if (num_inputs == 1) // if no arg passed in, just cd, just go $HOME
      {
        char *home = getenv("HOME");
        if (home == NULL)
          fprintf(stderr, "error fetching $HOME. has it been set?\nx ");
        else
          chdir(home);
      }
      else // we need to chdir() to the arg passed in
      {
        if (chdir(commands[1]) == -1)
          fprintf(stderr, "no such file or directory:%s\nx ", commands[1]);
      }
    }
    else if (strncmp(commands[0], "jobs", 4) == 0)
    {
      if (jobs->size == 0)
        fprintf(stderr, "no jobs currently running\n");
      else
      {
        node *cur_node = jobs->head;
        while (cur_node != NULL)
        {
          process *p = (process *)cur_node->data;
          fprintf(stdout, "pid: %d, name: %s\n", p->pid, p->name);
          cur_node = cur_node->next;
        }
      }
    }
    else if (strcmp(commands[0], "") == 0) // ignore empty input
      continue;
    /* End builtins */
    else
    { /* start sending things to the shell */
      // duplicate for NULL terminated command list
      char **args = malloc(sizeof(char *) * num_inputs + 1);
      int i;
      for (i = 0; i < num_inputs; i++)
        args[i] = strdup(commands[i]);
      args[num_inputs] = NULL;

#ifdef DEBUG
      for (i = 0; i < num_inputs + 1; i++)
        fprintf(stdout, "ARG AT %d IS %s\n", i, args[i]);
#endif

      int pid, status;
      pid = fork();
      if (pid < 0)
        fprintf(stderr, "whoa there, something went awry with that fork\n");
      else if (pid == 0) // child process
      {
        fprintf(stdout, "[%d] %s\n", getpid(), commands[0]);
        int error = execvp(commands[0], commands);
#ifdef DEBUG
        fprintf(stdout, "EXECVP OUTPUT: %d\n", error);
#endif
        if (error < 0)
          fprintf(stderr, "command not found: %s\n", commands[0]);
        exit(-1);
      }
      else // parent now
      {
        if (!do_background)
        {
          waitpid(pid, &status, 0);
          get_status(pid, status, commands[0]);
        }
        else
        {
          process *to_add = malloc(sizeof(process));
          to_add->pid = pid;
          to_add->name = strdup(commands[0]);
          list_add(jobs, (void *)to_add, sizeof(process));
        }
      }
    }

    /* cleanup */
    int i;
    for (i = 0; i < num_inputs; i++)
      free(commands[i]);
  }

  return 0;
}