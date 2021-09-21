#include <stdio.h>
#include <sys/wait.h>

#include "process.h"

void get_status(int pid, int status, char *name)
{
  if (WIFEXITED(status))
    fprintf(stderr, "[%d] %s exited (%d)\n", pid, name, status);
  if (WIFSIGNALED(status))
    fprintf(stderr, "[%d] %s killed (%d)\n", pid, name, status);
}