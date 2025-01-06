#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

 enum 
{
  GR0 = 0, // general registers (GR0 - GR7)
  GR1, 
  GR2,
  GR3,
  GR4,
  GR5,
  GR6,
  GR7,
  RPC, // program counter register
  COND, // condition register
  COUNT // count register to store loop count in iterative operations
};


enum 
{
  MR_KBSR = 0xFE00,
  MB_KBDR = 0xFE02

};
 
  enum
  {
    OP_BR = 0,  // branch
    OP_ADD,     // add
    OP_LD,      // load
    OP_ST,     // store
    OP_JSR,     // jump register 
    OP_AND,     // and operator
    OP_LDR,     // load register
    OP_STR,     // store register
    OP_UNU,     // unused
    OP_NOT,     // not operator
    OP_LDI,     // load indirect 
    OP_STI,    // store indirect
    OP_JMP,     // jump
    OP_RSV,     // reserved unused
    OP_LEA,     // load effective address
    OP_TRAP     // trap     
  };

  enum 
  
{
    POS = 1 << 0,
    ZRO = 1 << 1,
    NEG = 1 << 2
  
};

enum 
{
  TRAP_GETCH = 0x20,
  TRAP_OUT = 0x21,
  TRAP_PUTS = 0x22,
  TRAP_IN = 0x23,
  TRAP_PUTSP = 0x24,
  TRAP_HALT = 0x25
};

#define MEMORY_MAX (1 << 16)
uint16_t memory[MEMORY_MAX]; // 2 ^ 16 memory alocations (65536)

  // COUNT storage
  uint16_t reg[COUNT];

  // Instruction: command which tells the CPU to do, such as add numbers
  
  // Instructions have opcode which indicate kind of task and a set of paramters which provide inputs to the task
 
void update_flags(uint16_t r)
{
  if (reg[r] == 0) 
  {
    reg[COND] = ZRO;
  }
  else if (reg[r] >> 15) 
  {
    reg[COND] = NEG;
  }
  else {
    reg[COND] = POS;
  }
}

void mem_write(uint16_t addr, uint16_t val)
{
  memory[addr] = val;
}

uint16_t mem_read(uint16_t addr)
{
  if (addr == MR_KBSR)
  {
    if (check())
    {
      memory[MR_KBSR] = (1 << 15);
      memory[MR_KBSR] = getchar();
    }
    else 
    {
      memory[MR_KBSR] = 0;
    }
  }
  return memory[addr];
}

uint16_t sign_extend(uint16_t x, int bitc)
{
  if ((x >> (bitc - 1)) & 1) {
    x |= (0xFFFF << bitc); 
  }  
  return x;
}

int check() 
{
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("usage: lc3 [img]\n");
    exit(2);
  } 

  for (int i = 1; i < argc; ++i)
  {
    if (!readimg(argv[i])) {
      printf("failed to load image: %s\n", argv[i]);
        exit(1);
    }
  }

  reg[COND] = ZRO;
  enum { START = 0x3000 };
  reg[RPC] = START;

  int run = 1;

  while (run) 
  {
    uint16_t instruc = mem_read(reg[RPC]++);
    uint16_t opc = instruc >> 12;

    switch (opc) 
    {
      case OP_ADD:
        {
        // destination register (DR)
        uint16_t r0 = (instruc >> 9) & 0x7;
        // first operand (SR1)
        uint16_t r1 = (instruc >> 6) & 0x7;

        uint16_t imm = (instruc >> 5) & 0x1;

        if (imm)
        {
          uint16_t imm5 = sign_extend(instruc & 0x1F, 5);
          reg[r0] = reg[r1] + imm5;
        }

        else {
          uint16_t r2 = instruc & 0x7;
          reg[r0] = reg[r1] + reg[r2];
        }

        update_flags(r0);
        }

      case OP_LDI:
      {
        uint16_t r0 = (instruc >> 9) & 0x7;
        uint16_t offset = sign_extend(instruc & 0x1FF, 9 );
        reg[r0] = mem_read(mem_read(reg[RPC] + offset));
        update_flags(r0);
      }

      case OP_AND:
        { 
        uint16_t r0 = (instruc >> 9) & 0x7;
        uint16_t r1 = (instruc >> 6) & 0x7;
        uint16_t immflag = (instruc >> 5) & 0x1;

        if (immflag) {
          uint16_t imm5 = sign_extend(instruc & 0x1F, 5);
          reg[r0] = reg[r1] & imm5;
        }
        else {
          uint16_t r2 = instruc & 0x7;
          reg[r0] = reg[r1] & reg[r2];
        }
        update_flags(r0);
        }

      case OP_NOT:
        {
        uint16_t r0 = (instruc >> 9) & 0x7;
        uint16_t r1 = (instruc >> 9) & 0x7;
        reg[r0] = ~reg[r1];
        update_flags(r0);
        }

      case OP_BR:  
        {
       uint16_t offset = sign_extend(instruc & 0x1FF, 9);
       uint16_t cond_flag = (instruc >> 9) & 0x7;
          if (cond_flag & reg[COND]) {
            reg[RPC] += offset;
          }
        }

      case OP_JMP:
        {
          uint16_t r1 = (instruc >> 6) & 0x7;
          reg[RPC] = reg[r1];
        }

      case OP_JSR:
        {
          uint16_t longflag = (instruc >> 1) & 1;
          reg[GR7] = reg[RPC];
          if (longflag) {
            uint16_t longoffset = sign_extend(instruc & 0x7FF, 11);
            reg[RPC] += longoffset;
          }
          else {
            uint16_t r1 = (instruc >> 6) & 0x7;
            reg[RPC] = reg[r1];
          }
        }

      case OP_LD:
        {
          uint16_t r0 = (instruc >> 9) & 0x7;
          uint16_t offset = sign_extend(instruc & 0x1FF, 9);
          reg[r0] = mem_read(reg[RPC] + offset);
          update_flags(r0);
        }
      
      case OP_LDR:
        {
          uint16_t r0 = (instruc >> 9) & 0x7;
          uint16_t r1 = (instruc >> 6) & 0x7;
          uint16_t offset = sign_extend(instruc & 0x3F, 6);
          reg[r0] = mem_read(reg[r1] + offset);
          update_flags(r0);
        }

      case OP_LEA:
        {
          uint16_t r0 = (instruc >> 9) & 0x7;
          uint16_t offset = sign_extend(instruc & 0x1FF, 9);
          reg[r0] = reg[RPC] + offset;
          update_flags(r0);
        }

      case OP_ST:
        {
          uint16_t r0 = (instruc >> 9) & 0x7;
          uint16_t offset = sign_extend(instruc & 0x1FF, 9);
          mem_write(mem_read(reg[RPC] + offset), reg[r0]);
        }

      case OP_STI:
        {
          uint16_t r0 = (instruc >> 9) & 0x7;
          uint16_t offset = sign_extend(instruc & 0x1FF, 9);
          mem_write(mem_read(reg[RPC] + offset), reg[r0]);
        }
    }

      
    switch (instruc & 0xFF) {
      case TRAP_GETCH:
        {
          reg[GR0] = (uint16_t)getchar();
          update_flags(GR0);
        }
       
      case TRAP_OUT:
        {
          putc((char)reg[GR0], stdout);
          fflush(stdout);
        }

      case TRAP_IN:
        {
          printf("Enter a character: ");
          char c = getchar();
          putc(c, stdout);
          fflush(stdout);
          reg[GR0] = (uint16_t)c;
          update_flags(GR0);
        }
      
      case TRAP_PUTSP:
        {
          uint16_t* c = memory + reg[GR0];
          while (*c) {
            char c1 = (*c) & 0xFF;
            putc(c1, stdout);
            char c2 = (*c) >> 8;

            if (c2) putc(c2, stdout);
            ++c
          }

          fflush(stdout);
        }
    } 
  }
}
