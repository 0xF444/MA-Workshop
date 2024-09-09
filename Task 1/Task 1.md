# The Compilation Process of a C Program
1. The preprocessing stage which basically "processes" any `#` directives, macros, pragmas, etc..
2. The compilation stage which basically transforms our high level code into [object files](https://en.wikipedia.org/wiki/Object_file).
3. The assembler stage which actually generates the assembly instructions needed for execution.
4. The linking stage which links and external libraries (may it be OS libraries or user defined) to the executable to ensure proper execution.
![[iFtdu.webp]]
All of these stages produce an executable code in a format for the targeted OS: PE format or an ELF format.
# Anatomy of an Assembly Instruction
Any assembly instruction normally contain a **mnemonic** (as predefined by the flavor of the OS/Architecture manufacturers) and some operands (which are optional, depending on the mnemonic) 
For example:
```asm
MOV RAX, RBX == movq %rbx,%rax
```
These instructions do the same exact functionality, it's only a matter of a flavor difference: Intel vs AT&T.
Eventually, these assembly instructions are represented in [opcodes](https://en.wikipedia.org/wiki/Opcode) which are the binary combinations that the processor can only recognize to understand the instruction.
All of this information can be found within the processor manufacturer's manual.
![[Pasted image 20240508030128.png]]
# What are registers?
- Registers are basically very small (and very fast) storage elements that are inside the CPU, some may even consider them as very volatile elements.
- They basically serve as the place where all processing occur, this is because there is little to no latency when moving data from registers.
- We only tend to use the cache or main memory (This is all dependent on the [memory hierarchy](https://en.wikipedia.org/wiki/Memory_hierarchy)) as needed.
- RAM is where programs are loaded and the CPU has to *fetch* the instructions, *decode* them in order to understand what they are and then *execute*.
- Cache is used by extension to RAM in order to reduce the number of RAM reads because RAM is very slow compared to the blazingly fast CPU &rarr; It works by reading RAM lines (64 bytes) and storing them inside ever the CPU the data again.
![[Pasted image 20240508030851.png]]
These are the registers that are normally found within a 64 bit Intel Processor, there are many more but they are not discussed here.
# Flag Register
- This is basically a registers that stores the status of operations that are undergoing in the CPU.
- For example: `xor eax,eax` always gives 0. So in that case the `Zero Flag bit` in the RFLAGS registers will be set.
- There are many flags in the register but I'll keep it simple for this summary.
---
