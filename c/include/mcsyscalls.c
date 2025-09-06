#define SYS_run_command 513
#define SYS_run_if 514

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