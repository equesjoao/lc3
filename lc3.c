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

#define MEMORY_MAX (1 << 16)

  uint16_t memory[MEMORY_MAX]; // 2 ^ 16 memory alocations (65536)

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

  // COUNT storage
  uint16_t reg[COUNT];

  // Instruction: command which tells the CPU to do, such as add numbers
  
  // Instructions have opcode which indicate kind of task and a set of paramters which provide inputs to the task
  
  enum
  {
    OP_BR = 0,  // branch
    OP_ADD,     // add
    OP_LD,      // load
    OP_STI,     // store
    OP_JSR,     // jump register 
    OP_AND,     // and operator
    OP_LDR,     // load register
    OP_STR,     // store register
    OP_UNU,     // unused
    OP_NOT,     // not operator
    OP_LDI,     // load indirect 
    OP_STRI,    // store indirect
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

uint16_t sign_extend(uint16_t x, int bitc)
{
  if ((x >> (bitc - 1)) & 1) {
    x |= (0xFFFF << bitc); 
  }  
  return x;
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
        break;
    }
  }
}



