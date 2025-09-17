[English](README.md) | 简体中文 

# riscv-mc
这是一个为 Minecraft 设计的 RISC-V 指令集模拟器数据包（Datapack）。  
它支持完整的 RV32I 指令集，**但不包括 CSR 指令**。  
本数据包使用 [MC-Build](https://mcbuild.dev) 构建。  

类似的仓库 https://github.com/SuperTails/riscvcraft 但是支持 rv32ima. 该作者也有对 llvm / wasm 的mc数据包支持。

# Performance
当解码表缓存命中时平均 **1.1k** 命令 (Command) 每指令 (Instruction)。
其他情况下平均 5k 命令每指令。

# 使用方法
### 编译数据包
1. 安装 mc-build，请参考 [https://mcbuild.dev](https://mcbuild.dev) 的安装指南。
2. 将本仓库克隆到你的 Minecraft 世界目录下的 `datapacks` 文件夹中。
3. 在仓库目录下运行 `mcb build` 命令。
4. 此时该目录已生成为一个可加载的数据包，可直接放入 Minecraft 世界中使用。

### 编译程序
1. 你可能需要 RISC-V 工具链来编译程序。我创建了一个分支：https://github.com/winsrewu/rv32i-gnu-toolchain ，你可以直接使用其中的工作流来编译工具链。
2. 运行 c/build.sh 来编译程序，这将生成一个 .mem 文件并复制到 python/ 文件夹，然后调用一个 Python 脚本生成 .mcfunction 文件，该文件将被复制到 data/loader 目录。

### 运行程序
1. 加载数据包后，运行你的 `loader:app.mcfunction` 来加载程序。
2. 如果你之前运行过其他程序，请先执行以下命令重置状态：
```
/function riscvmc:reset
# reset memory, pc, regs
/function <your app.mcfunction>
# load memory again, in case of it being modified by the program
/function riscvmc:set_running
# set running flag to true
```
3. 通过多次执行 `/function riscvmcrunner:tick50` 来运行程序。  
每次执行将模拟 50 条指令。请注意，Minecraft 每 tick 有命令数限制，因此不宜一次运行过多。
4. 如需停止程序，执行 `/function riscvmc:stop_running`。

# 注意事项
- Python 模拟器：https://github.com/winsrewu/pyriscv ，我认为它们行为一致。
- 输出内容会被缓冲，直到遇到换行符 `\n` 才会显示。
- 你可以使用 scanf。但读取操作是非阻塞的，因此在运行程序前，你必须像这样设置 ascii tmp：
```
/data modify storage ascii:tmp input_buffer set value "Hello_from_input_buffer!"
```
- 程序入口点在 0x80000000, 但是你可以更改
- 我在python模拟器内提供了 systemcalls.c
- 存在一些问题，例如无效的文件描述符。我不清楚原因，我在 spike 上尝试过，结果也是一样的。但大部分代码都能正常工作。
- 我想我不会支持特权架构。
- 你需要一个很高的命令限制来运行程序，本数据包不会修改这个限制。所以你需要自己 ```/gamerule maxCommandChainLength 10000000```。
- 内存加载（app.mcfunction）和解码表加载（decode_map.mcfunction）完成后会打印一条消息。

# 故障排查
如果程序运行出错，你可以使用 `riscvmctester` 模块：导入正确的程序计数器（PC）序列，模拟器会在运行时自动比对。  
此外，你也可以使用 Python 版模拟器或 Spike 进行调试。上述 Python 模拟器支持打印每步的寄存器状态、监控内存访问等调试功能，便于定位问题。

# 系统调用表
| 编号 | 函数名       | 参数          | 返回值         |
|------|--------------|---------------|----------------|
| 63   | read         | fd, ptr, len  | 读取的字节数   |
| 64   | write        | fd, ptr, len  | 写入的字节数   |
| 93   | exit         | error_code    | 无             |
| 513  | run_command  | ptr, len      | 无             |
| 514  | run_if       | ptr, len      | if 语句结果，0 或 1 |
| 515    | place_block | x, y, z, block_type | - |

# block types
| id | name |
|----|------|
| 0  | air  |
| 1  | stone |
| 2  | dirt |
| 3  | birch_wood |

## 说明
- read: 从文件描述符（仅支持 0，即标准输入）读取数据到内存指针处，返回读取的字节数。
- write: 从内存指针处写入数据到文件描述符（仅支持 1，即标准输出），返回写入的字节数。
- exit: 以错误码退出模拟器。
- run_command: 立即执行缓冲区中的命令，忽略所有控制字符。
- run_if: 执行 /execute if data storage riscvmc:if $(buffer) run ...，如果测试通过返回 1，否则返回 0。

# TODO
懒得翻译了自己去看英文版本