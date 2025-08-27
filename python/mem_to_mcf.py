from num_to_binary_array import *

# read_mem.py
def parse_mem_file(filename):
    memory = {}  # dict：address (int) -> byte (int)
    current_address = None

    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue

            if line.startswith('@'):
                addr_hex = line[1:].strip()
                current_address = int(addr_hex, 16)
            else:
                bytes_hex = line.split()
                for byte_hex in bytes_hex:
                    if not byte_hex:
                        continue
                    byte_val = int(byte_hex, 16)
                    memory[current_address] = byte_val
                    current_address += 1

    return memory


def main():
    filename = 'app.mem'
    memory = parse_mem_file(filename)

    with open('app.mcfunction', 'w') as f:
        for addr in sorted(memory.keys()):
            byte_val = memory[addr]
            s = f"data modify storage riscvmc:memory m.{num_to_binary_string(addr, 32)} set value {num_to_binary_array(byte_val, 8)}"
            f.write(s + '\n')
        f.write('say loaded!')

    print(f"\nTotal {len(memory)} bytes loaded.")


if __name__ == '__main__':
    main()