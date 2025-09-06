#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define SYS_read 63
#define SYS_write 64
#define SYS_exit 93

#ifdef __cplusplus
#define EXTERN_C_BEGIN \
    extern "C"         \
    {
#define EXTERN_C_END }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif

extern char _heap_start;
extern char _heap_end;

static char *heap_end = &_heap_start;

EXTERN_C_BEGIN

caddr_t _sbrk(int incr)
{
    char *prev;
    if (heap_end + incr > &_heap_end)
    {
        errno = ENOMEM;
        return (caddr_t)-1;
    }
    prev = heap_end;
    heap_end += incr;
    return prev;
}

int _write(int fd, char *ptr, int len)
{
    register int a0 asm("a0") = fd;
    register int a1 asm("a1") = (int)ptr;
    register int a2 asm("a2") = len;
    register int a7 asm("a7") = SYS_write;

    asm volatile("ecall"
                 : "+r"(a0)
                 : "r"(a1), "r"(a2), "r"(a7)
                 : "memory");

    return a0;
}

int _fstat(int fd, struct stat *sb)
{
    if (fd >= 0 && fd <= 1)
    {
        // stdin, stdout, stderr(not supported)
        sb->st_mode = S_IFCHR;
        return 0;
    }
    errno = EBADF;
    return -1;
}

int _isatty(int fd)
{
    return 1; // always true for a character device
}

int _lseek(int fd, off_t offset, int whence)
{
    return 0; // not supported
}

int _close(int fd)
{
    return -1; // not supported
}

int _read(int fd, char *ptr, int len)
{
    register int a0 asm("a0") = fd;
    register int a1 asm("a1") = (int)ptr;
    register int a2 asm("a2") = len;
    register int a7 asm("a7") = SYS_read;

    asm volatile("ecall"
                 : "+r"(a0)
                 : "r"(a1), "r"(a2), "r"(a7)
                 : "memory");

    return a0;
}

void _exit(int status)
{
    register int a0 asm("a0") = status;
    register int a7 asm("a7") = SYS_exit;

    asm volatile("ecall"
                 :
                 : "r"(a0), "r"(a7)
                 : "memory");

    while (1)
        ; // halt the CPU
}

int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}

int _getpid(void)
{
    return 1;
}

int _open(const char *name, int flags, int mode)
{
    errno = ENOENT; // do not support any files, so it does not exist
    return -1;
}

EXTERN_C_END