#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

void panic(char*);
int fork1(void);

void pingpong(void)
{
    int p2cpipe[2];
    int c2ppipe[2];

    int buff[1];

    if (pipe(p2cpipe) < 0 || pipe(c2ppipe) < 0)
        panic("pipe");

    int n;
    int pid;

    // this is child
    if (fork1() == 0) {
        close(0);
        dup(p2cpipe[0]);
        close(p2cpipe[0]);
        close(p2cpipe[1]);

        while((n = read(0, buff, sizeof(buff))) == 0);
        if (n < 0)
            panic("Can't read in child\n");

        pid = getpid();
        printf("%d: received ping\n", pid);

        close(1);
        dup(c2ppipe[1]);
        close(c2ppipe[0]);
        close(c2ppipe[1]);

        if((n = write(1, buff, sizeof(buff))) <= 0)
            panic("Can't write from child");

        exit(0);
    }

    close(0);
    dup(c2ppipe[0]);
    close(c2ppipe[0]);
    close(c2ppipe[1]);

    buff[0] = 1;
    if ((n = write(p2cpipe[1], buff, sizeof(buff))) <= 0)
        panic("Can't write from parent");

    close(p2cpipe[1]);
    close(p2cpipe[0]);

    while ((n = read(0, buff, sizeof(buff))) == 0);
    if (n < 0)
        panic("Can't read in parent");

    pid = getpid();
    fprintf(1, "%d: received pong\n", pid);
    wait(0);
    exit(0);
}

void panic(char* s) {
    fprintf(2, "%s\n", s);
    exit(1);
}

int fork1(void) {
    int pid = fork();
    if (pid == -1)
        panic("fork");
    return pid;
}

void main(int argc, int* argv[])
{
    pingpong();
    exit(0);
}