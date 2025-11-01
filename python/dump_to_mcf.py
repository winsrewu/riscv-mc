from mem_to_mcf import parse_mem_file, pre_generate_decode_map
from num_to_binary_array import num_to_binary_array, num_to_binary_string

import sys
import os

current_dir = os.path.dirname(os.path.abspath(__file__))
target_dir = os.path.join(current_dir, "..", "pyriscv", "src")
sys.path.append(os.path.abspath(target_dir))

from pymem import PyMEM
from pyriscv import PyRiscv
from pyriscv_operator import PyRiscvOperator
from pyriscv_riscv_def import *


def main():
    pc = 0
    regs = []
    memory = {}

    with open("dump.txt", "r") as f:
        pc = PyRiscvOperator(32).unsigned(int(f.readline()[2:], 16))
        for i in range(32):
            line = f.readline()
            regs.append(PyRiscvOperator(32).unsigned(int(line[2:], 16)))

        memory = parse_mem_file(f)

    emulator = PyRiscv(PyMEM("app.mem"))
    pre_generate_decode_map(emulator, open("decode_map.mcfunction", "w"))

    with open("app.mcfunction", "w") as f:
        f.write("function riscvmc:reset\n")
        f.write(
            f"data modify storage riscvmc:memory pc set value {num_to_binary_array(pc, 32)}\n"
        )

        for i in range(32):
            s = f"data modify storage riscvmc:memory reg.{num_to_binary_string(i, 5)} set value {num_to_binary_array(regs[i], 32)}"
            f.write(s + "\n")

        for addr in sorted(memory.keys()):
            byte_val = memory[addr]
            s = f"data modify storage riscvmc:memory m.{num_to_binary_string(addr, 32)} set value {num_to_binary_array(byte_val, 8)}"
            f.write(s + "\n")
        f.write("say loaded!")

    print(f"\nTotal {len(memory)} bytes loaded.")


if __name__ == "__main__":
    main()
