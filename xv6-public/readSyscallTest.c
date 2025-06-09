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
        assert((rc2 - rc1) == 10);  // 5 reads by parent + 5 by child

        // Now reset the counter
        int reset_rc = getreadcount(1);
        assert(reset_rc == 0);  // reset returns 0 on success
        printf(1, "Read count reset\n");

        // Check that the counter is now zero
        int rc3 = getreadcount(0);
        printf(1, "Read count after reset %d\n", rc3);
        assert(rc3 == 0);

        printf(1, "TEST PASSED\n");
    }
    exit();
}
