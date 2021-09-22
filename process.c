#include <stdio.h>
#include <sys/wait.h>

#include "process.h"

/**
 * Format and print the exit status, pid, and name of an ended process
 * 
 * @param pid the process id of the exited process
 * @param status the exit status that will give the final status
 * @param name the name of the process, mostly for formatting purposes
 */
void get_status(pid_t pid, int status, char *name)
{
  if (WIFEXITED(status))
    fprintf(stderr, "[%d] %s exited %d\n", pid, name, status);
  if (WIFSIGNALED(status))
    fprintf(stderr, "[%d] %s killed %d\n", pid, name, status);
}