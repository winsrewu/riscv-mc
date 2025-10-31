pc = []
regs = []
with open("res.txt", "r") as f:
    for line in f:
        if line.startswith("PC="):
            pc.append(line.strip()[5:].upper())  # PC=0x...
        if line.startswith("Registers="):
            regs.append(line.strip()[10:].upper())  # Registers=...

with open("pc.mcfunction", "w") as f:
    f.write("data modify storage riscvmc:pc_log pc set value []\n")
    f.write("data modify storage riscvmc:pc_log regs set value []\n")
    for p in pc:
        f.write('data modify storage riscvmc:pc_log pc append value "{0}"\n'.format(p))
    for r in regs:
        f.write(
            'data modify storage riscvmc:pc_log regs append value "{0}"\n'.format(
                r.replace('"', '\\"')
            )
        )
