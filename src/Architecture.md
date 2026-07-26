# Architecture
x86a is designed specifically for the intel x86 architecture  
The goal is to support basic x86 assembly that is recognized globally such as ```mov eax, rdi``` and translate to the correct corresponding machine code  
Designed for portability and extensibility   

## Pipeline
The assembler so far has the following steps
1. Tokenizing. We convert the source files into a list of tokens.  
e.g. Imm16, Imm8, Instruction, Symbol, etc  
1. Section Generation  
For each source file we create a list of sections such as test, code, rodata, rel.data, etc  
These sections contain information such as what instruction and operands were passed to it.  
The instructions here don't represent machine code just an intermediary
1. Code Generation   
After every section is generated for a source file we create a linkable file with a `.o` extension  
The internal format is Elf32 and designed to work with a linker such as ld  

## Tokenizing
Each source file gets grouped into tokens  
These tokens consist of:  
*   Symbol  
*   Instruction  
*   Register(Reg8,Reg16,Reg32)  
*   MemoryAddress(Mem8,Mem16,Mem32) 
    *   These specify the size of the data they point to, not the size of the pointer
    *   The actual size of these pointers are the cpus address bus size
*   Immediates(Imm8, Imm16, Imm32)

## Section Generation
This stage takes these tokens we just generated and converts them into their associated machine code/binary equivalent  
If we come across a section directive we change what section to write to
by defualt we write to the `.text` section

## Code Generation
Fo
