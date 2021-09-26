#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

#include "list.h"
#include "process.h"
#include "signalhandler.h"

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
    signal(SIGINT, signal_handler);  // we want to catch ^C
    signal(SIGTSTP, signal_handler); // we want to catch ^Z
    signal(SIGQUIT, signal_handler); // we want to catch SIGQUIT (i cant type backslash)

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
        if (cur_node->p->pid == bg_pid)
        {
          bg_name = strdup(cur_node->p->name);
          break;
        }

        cur_node = cur_node->next;
      }

      fprintf(stderr, "background child ended: ");
      get_status(bg_pid, bg_status, bg_name);
      list_delete(jobs, cur_node->p);
    }
    /* End background checking */
    /* Start taking user input */

    // starting malloc size for command line args, to be realloc'ed later if needed
    int malloc_size = 2;
    char **commands = malloc(sizeof(char *) * malloc_size);

    fprintf(stdout, "%s", prompt);

    char *input = NULL;
    size_t len = 0;
    getline(&input, &len, stdin);

    // snag the first item in the line
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

    // check if we need to background it
    int background_command = (strcmp(commands[num_inputs - 1], "&") == 0);
    if (background_command)
      num_inputs -= 1; // this will be important when we go to fork

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
          fprintf(stdout, "pid: %d, name: %s\n", cur_node->p->pid, cur_node->p->name);
          cur_node = cur_node->next;
        }
      }
    }
    else if (strcmp(commands[0], "") == 0) // ignore empty input
    {
      continue;
    }
    else if (strncmp(commands[0], "get", 3) == 0)
    {
      if (num_inputs < 2)
        fprintf(stderr, "you need to pass the name of the var you want to get");

      char *var = getenv(commands[1]);
      if (var == NULL)
        fprintf(stderr, "that variable does not exist, set it first!");

      fprintf(stdout, "%s\n", var);
    }
    /* End builtins */
    else
    { /* start sending things to the shell */
      // duplicate for NULL terminated command list
      char **args = malloc(sizeof(char *) * num_inputs + 1);
      int i;
      for (i = 0; i < num_inputs; i++)
        args[i] = strdup(commands[i]);
      args[num_inputs] = NULL;

      pid_t pid;
      int status;
      pid = fork();
      if (pid < 0)
        fprintf(stderr, "whoa there, something went awry with that fork\n");
      else if (pid == 0) // child process
      {
        int error = execvp(commands[0], args);
        if (error < 0)
          fprintf(stderr, "command not found: \"%s\"\n", commands[0]);
        exit(-1);
      }
      else // parent now
      {
        fprintf(stdout, "[%d] %s\n", pid, commands[0]);
        if (!background_command)
        {
          waitpid(pid, &status, 0);
          get_status(pid, status, commands[0]);
        }
        else
        {
          process *to_add = malloc(sizeof(process));
          to_add->pid = pid;
          to_add->name = strdup(commands[0]);
          list_add(jobs, to_add);

          // created process is no longer needed
          free(to_add->name);
          free(to_add);
        }
      }

      // some seriously needed memory cleanup
      for (i = 0; i < num_inputs; i++)
      {
        free(args[i]);
        free(commands[i]);
      }

      free(args[num_inputs]);
    }
  }

  return 0;
}