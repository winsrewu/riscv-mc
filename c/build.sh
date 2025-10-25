set -e

filename=$1
extension="${filename##*.}"

if [ "$extension" = "c" ]; then
    compiler="riscv32-unknown-elf-gcc"
elif [ "$extension" = "cpp" ] || [ "$extension" = "cxx" ] || [ "$extension" = "cc" ]; then
    compiler="riscv32-unknown-elf-g++"
else
    echo "Error: Don't support file with extension '$extension'. Only support C/C++ source code."
    exit 1
fi

if [ "$filename" = "snake.c" ]; then
    extra_flags="-Wl,--no-relax-gp"
    link_script="../pyriscv/app/c-common/gptype_a.ld"
else
    extra_flags=""
    link_script="../pyriscv/app/c-common/gptype_b.ld"
fi

$compiler -g -O3 -static $extra_flags -specs=nosys.specs -march=rv32i -mabi=ilp32 -T $link_script -L ../pyriscv/app/c-common/ -o app.elf ../pyriscv/app/c-common/vectors.S ../pyriscv/app/c-common/syscalls.c $1 -lc -lgcc
riscv32-unknown-elf-objcopy -F verilog app.elf app.mem

mv app.mem ../python
cd ../python
python3 mem_to_mcf.py app.mem
mv app.mcfunction ../data/loader/function
mv decode_map.mcfunction ../data/loader/function