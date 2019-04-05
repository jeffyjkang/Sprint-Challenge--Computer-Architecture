#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cpu.h"

#define DATA_LEN 6

/**
 * Read / Write ram
 */

// function to read cpu ram, takes cpu struct and int address
unsigned char cpu_ram_read(struct cpu *cpu, int address)
{
  // return cpu ram value at address
  return cpu->ram[address];
}
// function to write cpu ram, takes cpu struct, int address, value, no return
void cpu_ram_write(struct cpu *cpu, int address, unsigned char value)
{
  // assign value to cpu ram at address
  cpu->ram[address] = value;
}

/**
 * push/ pop instructions
 */
// SP r7 stack pointer, register 7
// function to pop value of cpu registers at top
unsigned char cpu_pop(struct cpu *cpu)
{
  // copy value from address pointed to by sp to given register
  // invoke cpu ram read pass in register address
  unsigned char value = cpu_ram_read(cpu, cpu->registers[7]);
  // increment sp
  cpu->registers[7]++;
  // return value
  return value;
}
// function to push value to cpu registers at top
void cpu_push(struct cpu *cpu, unsigned char value)
{
  // decrement the sp
  cpu->registers[7]--;
  // copy value in the given register to address pointed to by sp
  // invoke cpu ram write func pass in register address 7  and value
  cpu_ram_write(cpu, cpu->registers[7], value);
}

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
// update cpu_load to accept second arg filename
void cpu_load(struct cpu *cpu, char *filename)
{
  // char data[DATA_LEN] = {
  //     // From print8.ls8
  //     0b10000010, // LDI R0,8
  //     0b00000000,
  //     0b00001000,
  //     0b01000111, // PRN R0
  //     0b00000000,
  //     0b00000001 // HLT
  // };

  // int address = 0;

  // for (int i = 0; i < DATA_LEN; i++)
  // {
  //   cpu->ram[address++] = data[i];
  // }

  // TODO: Replace this with something less hard-coded
  // open file, only need read, file pointer
  FILE *fp = fopen(filename, "r");
  // handle error when fp doesnt exist
  if (fp == NULL)
  {
    printf("error filename does not exist\n");
    exit(1);
  }
  // init line pointer
  char line[1024];
  // init register address to 0 to write in ram
  int address = 0;

  // read contents line by line, save appropriate data into RAM
  // fgets(char *str, int n, FILE *stream) while this holds true
  while (fgets(line, 1024, fp) != NULL)
  {
    // init end ptr
    // char *ptr;
    // ignore blank lines and everything after #, (comments)
    // if (ptr == buffer)
    // {
    // convert binary strings to integer values to store in RAM, strtoul()
    // strtoul(char *str, char **endptr, int base )
    // if (buffer[0] == '#')
    // {
    //   continue;
    // }
    // else
    // {
    char *endptr;
    unsigned char value = strtoul(line, &endptr, 2);
    // write address with value
    if (line == endptr)
    {
      continue;
    }
    cpu_ram_write(cpu, address++, value);
    // }
  }
  // }
  // close fp
  fclose(fp);
}

/**
 * ALU
 */
// alu arithmetic-logic unit
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op)
  {
  case ALU_MUL:
    // TODO
    cpu->registers[regA] *= cpu->registers[regB];
    break;
  case ALU_ADD:
    cpu->registers[regA] += cpu->registers[regB];
    break;
  // fl bits: 00000LGE, equal 00000001 =1, lessthan 00000100 =4, greaterthan 00000010 =2
  case ALU_CMP:
    // equal
    if (cpu->registers[regA] == cpu->registers[regB])
    {
      cpu->fl = 1;
    }
    //les than
    else if (cpu->registers[regA] < cpu->registers[regB])
    {
      cpu->fl = 4;
    }
    // greater than
    else if (cpu->registers[regA] > cpu->registers[regB])
    {
      cpu->fl = 2;
    }
    break;
    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction
  // init

  while (running)
  {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    // init instruction register to the return value of cpu ram read function
    // pass in cpu and cpu pc register memory address
    unsigned char IR = cpu_ram_read(cpu, cpu->pc);
    // read bytes at pc+1 pass to operandA
    unsigned char operandA = cpu_ram_read(cpu, cpu->pc + 1);
    // read bytes at pc+2 pass to operandB
    unsigned char operandB = cpu_ram_read(cpu, cpu->pc + 2);
    // 2. Figure out how many operands this next instruction requires
    // trace
    printf("TRACE: cpu: %02X, register: %02X, opA: %02X, opB: %02X\n", cpu->pc, IR, operandA, operandB);
    // switch statement for Instruction register
    switch (IR)
    {
      //for case halt, halt cpu and exit the emulator
    case HLT:
      // change value of running to stop the while loop
      running = 0;
      //
      cpu->pc += 1;
      // exit code
      exit(1);
      // break
      break;
      // case LDI, load immediate, stor value in register
    case LDI:
      // value in cpu registers operandA or pc ram address 1, assign to pc ram address 2
      cpu->registers[operandA] = operandB;
      // move to ram address 3
      cpu->pc += 3;
      // break
      break;
      // case print
    case PRN:
      // print value of cpu registers cpu->pc +1 address
      printf("%d\n", cpu->registers[operandA]);
      // move to address 3
      cpu->pc += 2;
      // break
      break;
      // case mul
    case MUL:
      // invoke algorithimic logic unit, pass in cpu, operator, values
      alu(cpu, ALU_MUL, operandA, operandB);
      // move address 3
      cpu->pc += 3;
      break;
      // case add
    case ADD:
      alu(cpu, ALU_ADD, operandA, operandB);
      cpu->pc += 3;
      break;
    // case pop
    case POP:
      // assign return of cpu_pop to cpu registers at operand a
      cpu->registers[operandA] = cpu_pop(cpu);
      // move address 2
      cpu->pc += 2;
      //break
      break;
    // case push
    case PUSH:
      // invoke cpu push pass in operand a
      cpu_push(cpu, cpu->registers[operandA]);
      // move address 2
      cpu->pc += 2;
      // break
      break;
    // case call
    case CALL:
      cpu_push(cpu, cpu->pc + 2);
      cpu->pc = cpu->registers[operandA];
      break;
    // case return
    case RET:
      cpu->pc = cpu_pop(cpu);
      break;
    // default case
    default:
      // error for unknown instructions
      printf("Unrecognized instruction\n");
      //exit code
      exit(1);
      // break
      break;
    }
    // 3. Get the appropriate value(s) of the operands following this instruction
    // 4. switch() over it to decide on a course of action.
    // 5. Do whatever the instruction should do according to the spec.
    // 6. Move the PC to the next instruction.
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  // memset syntax = ptr, value, number of bytes
  // init cpu pc to 0
  cpu->pc = 0;
  cpu->registers[7] = 0xF4;
  // init cpu registers to 0
  memset(cpu->registers, 0, 8);
  // init cpu ram to 0
  memset(cpu->ram, 0, 256);
}
