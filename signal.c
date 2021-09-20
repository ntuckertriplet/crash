#include <stdio.h>

#include "signal.h"

void signal_handler(int signal)
{
  fprintf(stderr, "\n");
}