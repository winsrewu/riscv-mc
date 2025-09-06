.section .vectors, "ax"
.globl _vectors
_vectors:

    # load _stack_top to sp
    lui sp, %hi(_stack_top)        # load high 20 bits
    addi sp, sp, %lo(_stack_top)   # add low 12 bits

    j _start