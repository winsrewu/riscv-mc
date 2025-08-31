English | [简体中文](README_zh_CN.md)  

# riscv-mc
This datapack provides a RISC-V emulator for Minecraft.
It supports RV32I instruction sets, except for csr instructions.  
This datapack is built using https://mcbuild.dev.

# Usage
### Compiling the datapack
1. Install mc-build, please follow the instructions on https://mcbuild.dev.
2. Clone this repository directly into your Minecraft world's datapacks folder.
3. Run ```mcb build``` in the repository directory
4. Now the repository directory becomes a datapack. You can now load it into your Minecraft world.

### Compiling the program
1. Follow the instructions on https://github.com/winsrewu/pyriscv, you may need RISC-V toolchain to compile the program. Good to know that I had create a fork at https://github.com/winsrewu/rv32i-gnu-toolchain, you can directly use that workflow to compile the toolchain.
2. The build.sh script there should generate an app.mem file, which is the program you want to run. Copy this file to the datapack's riscv-mc/python folder and run ```python mem_to_mcf.py```, this will generate app.mcfunction, which is the memory initialization file for the emulator.


### Running the program
1. Load the datapack and run the app.mcfunction.
2. If you had runned before, do
```
/function riscvmc:reset
# reset memory, pc, regs
/function <your app.mcfunction>
# load memory again, in case of it being modified by the program
/function riscvmcrunner:reset
# reset the runner
/function riscvmc:set_running
# set running flag to true
```
3. You can now run the program by doing ```function riscvmcrunner:tick50``` multiple times.
This will let the emulator run 50 instructions at one tick. Please notice that there's a limit in Minecraft of commands per tick.
4. If you want to stop the program, do ```function riscvmc:stop_running```.


# Good to know
- The output is buffered until a \n is encountered.
- I only implemented exit read and write system calls. You can use scanf. But the read action is not blocking, so you must set the ascii tmp like this before running the program:
```
/data modify storage ascii:tmp input_buffer set value "Hello_from_input_buffer!"
```
- The entry point of the program is hardcoded to 0x80000000, but you can change it.
- I have provided a systemcalls.c in the python emulator.
- There do have some problems, like invaild file descriptor. I dont know why, i tried it on spike and the result is the same. But most of code works fine.

# Troubleshooting
If your program breaks, you can use the riscvmctester, where you can import a correct list of program counter, and the emulator will check it for you when running. And you can also use the emulator based on python or spike to debug your program. The python emulator mentioned above do have some functionalities to help you debug your program, like printing the registers at each step, monitering memory access, etc.

# TODO
- Support the privileged architecture
- Optimize the SLOW bit operations