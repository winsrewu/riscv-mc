set -e

riscv32-unknown-elf-gcc -g -O3 -static -specs=nosys.specs -march=rv32i -mabi=ilp32 -T include/link.ld -o app.elf include/vectors.S include/syscalls.c main.c -lc -lgcc
riscv32-unknown-elf-objcopy -F verilog app.elf app.mem

mv app.mem ../python
cd ../python
python3 mem_to_mcf.py app.mem
mv app.mcfunction ../data/loader/function