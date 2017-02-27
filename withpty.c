#define _GNU_SOURCE

#include <pty.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <termios.h>

extern char *program_invocation_short_name;

static void usage(int code) {
    FILE *out = code ? stderr : stdout;
    fprintf(out, "USAGE: %s [-ioe] TTY_PATH SLAVE [SLAVE_ARGS ...]\n", program_invocation_short_name);
    exit(code ? 1 : 0);
}

int main(int argc, char **argv) {

    int slave;
    char master_s[16];

    int fds = 0;
    char *pty_path, **slave_argv;
    int c, i;

    while ((c = getopt(argc, argv, "hioe")) != -1) {
        switch (c) {
            case 'i':
                fds |= 1;
                break;
            case 'o':
                fds |= 2;
                break;
            case 'e':
                fds |= 4;
                break;
            case 'h':
                usage(0);
            default:
                usage(1);
        }
    }

    if (argc - optind < 2) {
        usage(1);
    }

    pty_path = argv[optind];
    slave_argv = argv + optind;
    for (i = optind; i < argc - 1; i++) {
        argv[i] = argv[i + 1];
    }
    argv[argc - 1] = NULL;

    if ((slave = open(pty_path, O_RDWR)) == -1) {
        perror("");
        return -1;
    }
 
    if (setsid() == -1) {
        perror("");
        return -1;
    }

    if (ioctl(slave, TIOCSCTTY, (char *)NULL)) {
        perror("");
        return -1;
    }

    for (i = 0; i < 3; i++) {
        if (fds & (1 << i)) {
            dup2(slave, i);
        }
    }
    execvp(slave_argv[0], slave_argv);

    return -1;
}
