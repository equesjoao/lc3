#include <stdint.h>

#define MEMORY_MAX (1 << 16)

void main() {
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

}
