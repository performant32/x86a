# Architecture

The assembler so far has the following steps
1. Tokenizing. We convert the source files into a list of tokens.  
e.g. Imm16, Imm8, Instruction, Symbol, etc  
1. Section Generation  
For each source file we create a list of sections such as test, code, rodata, rel.data, etc  
These sections contain information such as what instruction and operands were passed to it.  
The instructions here don't represent machine code just an intermediary
