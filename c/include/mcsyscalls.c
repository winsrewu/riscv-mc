#define SYS_run_command 513
#define SYS_run_if 514
#define SYS_place_block 515

void run_command(char *ptr, int len)
{
    register int a0 asm("a0") = (int)ptr;
    register int a1 asm("a1") = len;
    register int a7 asm("a7") = SYS_run_command;

    asm volatile("ecall"
                 :
                 : "r"(a0), "r"(a1), "r"(a7)
                 : "memory");
}

int run_if(char *ptr, int len)
{
    register int a0 asm("a0") = (int)ptr;
    register int a1 asm("a1") = len;
    register int a7 asm("a7") = SYS_run_if;

    asm volatile("ecall"
                 : "+r"(a0)
                 : "r"(a1), "r"(a7)
                 : "memory");

    return a0;
}

void place_block(int x, int y, int z, int block)
{
    register int a0 asm("a0") = x;
    register int a1 asm("a1") = y;
    register int a2 asm("a2") = z;
    register int a3 asm("a3") = block;
    register int a7 asm("a7") = SYS_place_block;

    asm volatile("ecall"
                 :
                 : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a7)
                 : "memory");
}