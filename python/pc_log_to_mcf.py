pc = []
with open('res.txt', 'r') as f:
    for line in f:
        if line.startswith('0x'):
            pc.append(line.strip()[2:].upper())

with open('pc.mcfunction', 'w') as f:
    f.write('data modify storage riscvmc:pc_log pc set value []\n')
    for p in pc:
        f.write('data modify storage riscvmc:pc_log pc append value "{0}"\n'.format(p))