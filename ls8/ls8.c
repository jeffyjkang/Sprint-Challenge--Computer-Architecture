#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

/**
 * Main
 */
// argc is argument count, argv is array of strings that hold individual arguments
int main(int argc, char *argv[])
{
  struct cpu cpu;

  // if arggument count is less than 2
  if (argc < 2)
  {
    // print error
    printf("Error, no argument given for filename\n");
    exit(1);
  }

  cpu_init(&cpu);
  // change cpu_load, to have second param, pass in argument string at index 1
  cpu_load(&cpu, argv[1]);
  cpu_run(&cpu);

  return 0;
}