English | [简体中文](README_zh_CN.md)  

# riscv-mc
This datapack provides a RISC-V emulator for Minecraft.
It supports RV32I instruction sets, except for csr instructions.  
This datapack is built using https://mcbuild.dev.  

Similar repo https://github.com/SuperTails/riscvcraft but with rv32ima. This author also have llvm / wasm support for mc datapack.

# Usage
### Compiling the datapack
1. Install mc-build, please follow the instructions on https://mcbuild.dev.
2. Clone this repository directly into your Minecraft world's datapacks folder.
3. Run ```mcb build``` in the repository directory
4. Now the repository directory becomes a datapack. You can now load it into your Minecraft world.

### Compiling the program
1. You may need RISC-V toolchain to compile the program. Good to know that I had create a fork at https://github.com/winsrewu/rv32i-gnu-toolchain, you can directly use that workflow to compile the toolchain.
2. Run c/build.sh to compile the program, this should generate a .mem file and copy it to python/ folder, and call a python script to generate a .mcfunction file, which will be copied to data/loader.


### Running the program
1. Load the datapack and run the loader:app.mcfunction.
2. If you had runned before, do
```
/function riscvmc:reset
# reset memory, pc, regs
/function <your app.mcfunction>
# load memory again, in case of it being modified by the program
/function riscvmc:set_running
# set running flag to true
```
3. You can now run the program by doing ```function riscvmcrunner:tick50``` multiple times.
This will let the emulator run 50 instructions at one tick. Please notice that there's a limit in Minecraft of commands per tick.
4. If you want to stop the program, do ```function riscvmc:stop_running```.


# Good to know
- Python emulator: https://github.com/winsrewu/pyriscv, i think they have the same behavior.
- The output is buffered until a \n is encountered.
- You can use scanf. But the read action is not blocking, so you must set the ascii tmp like this before running the program:
```
/data modify storage ascii:tmp input_buffer set value "Hello_from_input_buffer!"
```
- The entry point of the program is hardcoded to 0x80000000, but you can change it.
- I have provided a systemcalls.c in the python emulator.
- There do have some problems, like invaild file descriptor. I dont know why, i tried it on spike and the result is the same. But most of code works fine.
- I think I won't support privileged architecture.

# Troubleshooting
If your program breaks, you can use the riscvmctester, where you can import a correct list of program counter, and the emulator will check it for you when running. And you can also use the emulator based on python or spike to debug your program. The python emulator mentioned above do have some functionalities to help you debug your program, like printing the registers at each step, monitering memory access, etc.

# system calls table
| number | function | args | return |
|--------|----------|------|--------|
| 63     | read     | fd, ptr, len | number of bytes read |
| 64     | write    | fd, ptr, len | number of bytes written |
| 93     | exit     | error_code | - |
| 513    | run_command | ptr, len | - |
| 514    | run_if | ptr, len | the result of the if statement, 0 or 1 |
| 515    | place_block | x, y, z, block_type | - |

# block types
| id | name |
|----|------|
| 0  | air  |
| 1  | stone |
| 2  | dirt |
| 3  | birch_wood |

## explanation
- read: read the buffer from the file descriptor (only support 0, which is stdin) to the memory pointer, return the number of bytes read.
- write: write the buffer from the memory pointer to the file descriptor (only support 1, which is stdout), return the number of bytes written.
- exit: exit the emulator with the error code.
- run_command: run the command in the buffer immediately, ignoring all the control characters.
- run_if: run /execute if data storage riscvmc:if $(buffer) run ..., if test passed, return 1, else return 0.

# TODO
- [ ] Optimizing