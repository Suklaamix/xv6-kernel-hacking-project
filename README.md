# xv6-kernel-hacking-project
Implemented a global variable called readcount which is incremented every time the syscall read is called and a syscall getreadcount that returns the value of readcount.

These are all of the changes made to the file
**syscall.c**

```c
// earlier code
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_getreadcount(void); // Added this line
extern int readcount; // Added this line

// rest of the code

[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_getreadcount]   sys_getreadcount, // Added this line

void
syscall(void)
{
  int num;
  struct proc *curproc = myproc();
  num = curproc->tf->eax;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    curproc->tf->eax = syscalls[num]();
      if (num == SYS_read) readcount++; // Added this line
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}
```
Creating the **sys_getreadcount** call function and the global variable **readcount** that will store our count, and making the getreadcount system call which user programs can use to call it. Incrementing the readcount variable by 1 whenever the **read** systemcall is used

**syscall.h** changes

```h
// earlier code
#define SYS_getreadcount 22 // Added this line
```
adding an extra systemcall for the getreadcount after the default 21 from xv6

**sysfile.c** changes

```c
#include "file.h"
#include "fcntl.h"

int readcount; / Added everything after this line
int
sys_getreadcount(int reset) {
  if (argint(0, &reset) < 0)
    return -1;

  if (reset) {
    readcount = 0;
    return 0;
  } else {
    return readcount;
  }
} 
```
Adding the new system call getreadcount which depending on the argument 0 or 1 will either reset the counter or return the value

**user.h** changes

```
// Earlier code
int sleep(int);
int uptime(void);
int getreadcount(int reset); // Added this line
// Rest of the code
```
registering the syscall we made so it can be called and now the code functions

including a **readSyscallTest.c**

```c
#include "types.h"
#include "user.h"
#include "param.h"

#define assert(x)                                                      \
    if (x)                                                             \
    { /* pass */                                                       \
    }                                                                  \
    else                                                               \
    {                                                                  \
        printf(1, "assert failed %s %s %d\n", #x, __FILE__, __LINE__); \
        exit();                                                        \
    }

void readfile(char *file, int howmany)
{
    int i;
    int fd = open(file, 0);
    char buf;
    for (i = 0; i < howmany; i++)
        (void)read(fd, &buf, 1);
    close(fd);
}

int main(int argc, char *argv[])
{
    int rc1 = getreadcount(0);
    printf(1, "Initial read count %d\n", rc1);

    int rc = fork();
    if (rc < 0)
    {
        printf(1, "Fork failed!\n");
        exit();
    }
    readfile("README", 5);
    if (rc > 0)
    {
        wait();
        int rc2 = getreadcount(0);
        printf(1, "Read count after reads %d\n", rc2);
        assert((rc2 - rc1) == 10);

        int reset_rc = getreadcount(1);
        assert(reset_rc == 0);
        printf(1, "Read count reset\n");

        int rc3 = getreadcount(0);
        printf(1, "Read count after reset %d\n", rc3);
        assert(rc3 == 0);

        printf(1, "TEST PASSED\n");
    }
    exit();
}
```
This code was made by chatgpt and it will read the initial read count and then the read count after reading README and then read count after reseting it.

The output looks something like this
```
Initial read count 16
Read count after reads 26
Read count reset
Read count after reset 0
TEST PASSED
```  
**Makefile** changes
```
UPROGS=\
    _cat\
    _zombie\
    _readSyscallTest\ # Added this line

EXTRA=\
    mkfs.c ulib.c user.h cat.c echo.c forktest.c grep.c kill.c\
    ln.c ls.c mkdir.c rm.c stressfs.c usertests.c wc.c zombie.c\
    printf.c umalloc.c\
    readSyscallTest.c\ # Added this line
    README dot-bochsrc *.pl toc.* runoff runoff1 runoff.list\
    .gdbinit.tmpl gdbutil\
```
The following changes make sure that the readSyscallTest is included in the xv6 image when compiled  
You can now run  
**Make qemu** and
**readSyscallTest**  
to run the test and compile xv6
