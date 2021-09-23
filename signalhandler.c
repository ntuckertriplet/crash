#include <stdio.h>

#include "signalhandler.h"

/**
 * This is a function meant to catch SIGKILL, SIGINT, and others.
 * It simply prints a newline to the screen, but does not allow a person
 * to kill the shell via user input signals
 * 
 * @param signal the int signal
 */
void signal_handler(int signal)
{
  fprintf(stderr, "\n");
}