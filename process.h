#ifndef PROCESS_H_
#define PROCESS_H_

typedef struct process
{
  int pid;
  char *name;
} process;

void get_status(int pid, int status, char *name);

#endif