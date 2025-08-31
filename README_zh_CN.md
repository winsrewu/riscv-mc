[English](README.md) | 简体中文 

# riscv-mc
这是一个为 Minecraft 设计的 RISC-V 指令集模拟器数据包（Datapack）。  
它支持完整的 RV32I 指令集，**但不包括 CSR 指令**。  
本数据包使用 [MC-Build](https://mcbuild.dev) 构建。  

类似的仓库 https://github.com/SuperTails/riscvcraft 但是支持 rv32ima. 该作者也有对 llvm / wasm 的mc数据包支持。

# 使用方法
### 编译数据包
1. 安装 mc-build，请参考 [https://mcbuild.dev](https://mcbuild.dev) 的安装指南。
2. 将本仓库克隆到你的 Minecraft 世界目录下的 `datapacks` 文件夹中。
3. 在仓库目录下运行 `mcb build` 命令。
4. 此时该目录已生成为一个可加载的数据包，可直接放入 Minecraft 世界中使用。

### 编译程序
1. 参考 [https://github.com/winsrewu/pyriscv](https://github.com/winsrewu/pyriscv) 的说明，你需要 RISC-V 工具链来编译程序。  
   我已为此创建了一个工具链的 fork：[https://github.com/winsrewu/rv32i-gnu-toolchain](https://github.com/winsrewu/rv32i-gnu-toolchain)，可直接使用其构建流程。
2. 执行其中的 `build.sh` 脚本，会生成 `app.mem` 文件，即你希望运行的程序。  
   将此文件复制到数据包的 `riscv-mc/python` 目录下，然后运行 `python mem_to_mcf.py`，  
   该脚本将生成 `app.mcfunction` —— 这是模拟器所需的内存初始化函数文件。

### 运行程序
1. 加载数据包后，运行你的 `app.mcfunction` 来加载程序。
2. 如果你之前运行过其他程序，请先执行以下命令重置状态：
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
3. 通过多次执行 `/function riscvmcrunner:tick50` 来运行程序。  
每次执行将模拟 50 条指令。请注意，Minecraft 每 tick 有命令数限制，因此不宜一次运行过多。
4. 如需停止程序，执行 `/function riscvmc:stop_running`。

# 注意事项
- 输出内容会被缓冲，直到遇到换行符 `\n` 才会显示。
- 我只实现了 exit、read 和 write 系统调用。你可以使用 scanf，但 read 操作是非阻塞的，因此在运行程序之前，必须像这样设置 ascii tmp：
```
/data modify storage ascii:tmp input_buffer set value "Hello_from_input_buffer!"
```
- 程序入口点在 0x80000000, 但是你可以更改
- 我在python模拟器内提供了 systemcalls.c
- 存在一些问题，例如无效的文件描述符。我不清楚原因，我在 spike 上尝试过，结果也是一样的。但大部分代码都能正常工作。
- 我想我不会支持特权架构。

# 故障排查
如果程序运行出错，你可以使用 `riscvmctester` 模块：导入正确的程序计数器（PC）序列，模拟器会在运行时自动比对。  
此外，你也可以使用 Python 版模拟器或 Spike 进行调试。上述 Python 模拟器支持打印每步的寄存器状态、监控内存访问等调试功能，便于定位问题。

# TODO
懒得翻译了自己去看英文版本