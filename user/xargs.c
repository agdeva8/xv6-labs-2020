#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/fs.h"

void panic(char* s);
int fork1(void);
void printArgv(char *argv[]);

void xargs(int argc, char* argv[])
{
    char c[1];
    char buf[MAXARG][64];
    int currArgc = argc, j = 0;

    for (int i = 0; i < currArgc; i++)
        strcpy(buf[i], argv[i]);
    
    while (read(0, c, sizeof(c)) == sizeof(c)) {
        if (*c == ' ' || *c == '\n') {
            if (currArgc > MAXARG)
                panic("args: line is too big to process");

            buf[currArgc++][j] = 0; j = 0;
            if (*c == '\n')
            {
                if (fork1() == 0) {
                    char *line[MAXARG];
                    for (int i = 0; i<currArgc; i++)
                        line[i] = buf[i];
                    exec(line[1], &line[1]);
                    fprintf(2, "exec %s failed\n", line[1]);
                }
                wait(0);
                currArgc = argc;
            }
            continue;
        }
        buf[currArgc][j++] = *c;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        panic("Usage: xargs command args..");
    }

    xargs(argc, argv);
    exit(0);
}

void panic(char* s) 
{
    fprintf(2, "%s\n", s);
    exit(1);
}

int fork1()
{
    int n;
    n = fork();
    if (n < 0)
        panic("fork");
    return n;
}

void printArgv(char *argv[]) {
    printf("printing argv\n");
    int argc;
    for(argc = 0; argv[argc]; argc++) {
        printf("%s ", argv[argc]);
    }
    printf("\n");
}