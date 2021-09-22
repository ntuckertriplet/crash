#ifndef PROCESS_H_
#define PROCESS_H_

typedef struct process
{
  pid_t pid;
  char *name;
} process;

void get_status(pid_t pid, int status, char *name);

#endif