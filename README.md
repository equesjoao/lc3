## What is LC-3?

The **LC-3** (Little Computer 3) is a 16-bit virtual machine designed as an educational tool for teaching computer architecture and low-level programming. It was introduced in the book *"Introduction to Computing Systems: From Bits and Gates to C and Beyond"* by Yale N. Patt and Sanjay J. Patel. The LC-3's architecture is simple enough for beginners to understand but powerful enough to teach important computer science concepts.

---

### Key Features

- **Word Size**: 16 bits
- **Address Space**: 65,536 locations (16-bit addressing)
- **Instruction Set**:
  - 15 total instructions
  - Operations include logic, arithmetic, control flow, and memory access
- **Registers**:
  - 8 general-purpose registers (R0-R7)
  - Condition Register (NZP) to store status information
- **Input and Output**:
  - Support for keyboard and monitor via memory-mapped I/O
- **Memory**:
  - Divided into RAM (data and instructions) and ROM (preloaded programs, such as the bootloader)

---

### Educational Purpose

The LC-3 is designed to help students:

1. **Understand Computer Architecture**: From logic gates and circuits to registers and buses.
2. **Learn Assembly Language**: Write and debug programs directly in the machine's instruction set.
3. **Explore Operating Systems**: Understand basic OS functionalities, including interrupts and memory management.

---

### Example LC-3 Assembly Instructions

```asm
ADD R0, R1, R2  ; Add the values of R1 and R2, storing the result in R0
LD R3, LABEL     ; Load the value stored at address LABEL into R3
BRz END          ; Branch to END if the last result was zero
```

---

