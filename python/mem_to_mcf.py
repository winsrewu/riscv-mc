from io import TextIOWrapper
from num_to_binary_array import *

import sys
import os

current_dir = os.path.dirname(os.path.abspath(__file__))
target_dir = os.path.join(current_dir, "..", "pyriscv", "src")
sys.path.append(os.path.abspath(target_dir))

from pymem import PyMEM
from pyriscv import PyRiscv
from pyriscv_riscv_def import *

memory_available = dict()

decode_map_len = {}
decode_map_len["CODECLASS"] = 2
decode_map_len["OPCODE"] = 5
decode_map_len["FUNCT3"] = 3
decode_map_len["FUNCT7"] = 7
decode_map_len["RD"] = 5
decode_map_len["RS1"] = 5
decode_map_len["RS2"] = 5
decode_map_len["IMMJ"] = 21
decode_map_len["IMMB"] = 13
decode_map_len["IMMI"] = 12
decode_map_len["IMMS"] = 12
decode_map_len["IMMU"] = 32


# read_mem.py
def parse_mem_file(f: TextIOWrapper):
    memory = {}  # dict：address (int) -> byte (int)
    current_address = None

    for line in f:
        line = line.strip()
        if not line or line.startswith("#"):
            continue

        if line.startswith("@"):
            addr_hex = line[1:].strip()
            current_address = int(addr_hex, 16)
        else:
            bytes_hex = line.split()
            for byte_hex in bytes_hex:
                if not byte_hex:
                    continue
                byte_val = int(byte_hex, 16)
                memory[current_address] = byte_val
                memory_available[current_address] = True
                current_address += 1

    return memory


inst_map = {}


def pre_generate_decode_map(emulator, f: TextIOWrapper):
    for k, v in memory_available.items():
        # check if all 32 bits are available
        if k % 4 != 0:
            continue
        if not (
            memory_available.get(k + 1, False)
            and memory_available.get(k + 2, False)
            and memory_available.get(k + 3, False)
        ):
            continue

        inst = emulator.fetch(k)

        key = num_to_binary_string(inst[31:0], 32)
        if key in inst_map:
            continue

        decode_map = emulator.decode(inst)

        if decode_map.CODECLASS != PYRSISCV_CODECLASS.BASE or decode_map.OPCODE == None:
            continue

        decode_map_output = {}
        decode_map_output["CODECLASS"] = True
        decode_map_output["OPCODE"] = decode_map.OPCODE.name
        if (
            decode_map.OPCODE == PYRSISCV_OPCODE.OP
            or decode_map.OPCODE == PYRSISCV_OPCODE.OP_IMM
        ):
            if decode_map.FUNCT3_OP_IMM_OP == None:
                continue
            decode_map_output["FUNCT3_OP_IMM_OP"] = decode_map.FUNCT3_OP_IMM_OP.name
        elif decode_map.OPCODE == PYRSISCV_OPCODE.BRANCH:
            if decode_map.FUNCT3_BRANCH == None:
                continue
            decode_map_output["FUNCT3_BRANCH"] = decode_map.FUNCT3_BRANCH.name
        elif (
            decode_map.OPCODE == PYRSISCV_OPCODE.LOAD
            or decode_map.OPCODE == PYRSISCV_OPCODE.STORE
        ):
            if decode_map.FUNCT3_LOAD_STORE == None:
                continue
            decode_map_output["FUNCT3_LOAD_STORE"] = decode_map.FUNCT3_LOAD_STORE.name

        if decode_map.EBREAK:
            decode_map_output["EBREAK"] = True
        if decode_map.ECALL:
            decode_map_output["ECALL"] = True

        w = inst

        decode_map_raw = {}
        decode_map_raw["CODECLASS"] = w[1:0]
        decode_map_raw["OPCODE"] = w[6:2]
        decode_map_raw["FUNCT3"] = w[14:12]
        decode_map_raw["FUNCT7"] = w[31:25]
        decode_map_raw["RD"] = w[11:7]
        decode_map_raw["RS1"] = w[19:15]
        decode_map_raw["RS2"] = w[24:20]
        decode_map_raw["IMMJ"] = (
            (w[30:21] << 1) | (w[20] << 11) | (w[19:12] << 12) | (w[31] << 20)
        )
        decode_map_raw["IMMB"] = (
            (w[11:8] << 1) | (w[30:25] << 5) | (w[7] << 11) | (w[31] << 12)
        )
        decode_map_raw["IMMI"] = w[31:20]
        decode_map_raw["IMMS"] = w[11:7] + (w[31:25] << 5)
        decode_map_raw["IMMU"] = w[31:12] << 12

        decode_map_raw_output = {}
        for k, v in decode_map_raw.items():
            decode_map_raw_output[k] = num_to_binary_array(v, decode_map_len[k])

        inst_map[key] = (
            str(decode_map_output)
            .replace("True", "1b")
            .replace("False", "0b")
            .replace("'", ""),
            str(decode_map_raw_output).replace("'", ""),
        )

    for k, v in inst_map.items():
        f.write(f"data modify storage riscvmc:decode_map ip.{k} set value {v[0]}\n")
        f.write(f"data modify storage riscvmc:decode_map i.{k} set value {v[1]}\n")
    f.write("say decode map loaded!")


def main():
    filename = "app.mem"
    memory = parse_mem_file(open(filename, "r"))

    emulator = PyRiscv(PyMEM(filename))
    pre_generate_decode_map(emulator, open("decode_map.mcfunction", "w"))

    with open("app.mcfunction", "w") as f:
        for addr in sorted(memory.keys()):
            byte_val = memory[addr]
            s = f"data modify storage riscvmc:memory m.{num_to_binary_string(addr, 32)} set value {num_to_binary_array(byte_val, 8)}"
            f.write(s + "\n")
        f.write("say loaded!")

    print(f"\nTotal {len(memory)} bytes loaded.")


if __name__ == "__main__":
    main()
