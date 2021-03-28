#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(2, "Usage: sleep n\n");
        exit(1);
    }

    int n = atoi(argv[1]);

    if (sleep(n) < 0) {
        fprintf(2, "Error while putting program to sleep\n");
        exit(1);
    }
    exit(0);
}
