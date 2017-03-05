#define _GNU_SOURCE

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>


extern char *program_invocation_short_name;

static void usage(int code) {
    fprintf(stderr, "Usage: %s [-z] [-s SIZE[%%]] CMD [ARGS ...]\n", program_invocation_short_name);
    exit(code);
}


enum direction { HORIZONTAL, VERTICAL };
enum size_type { NONE, ABSOLUTE, PERCENT };

struct args {
    enum direction dir;
    enum size_type st;
    char *size;
    char **cmdv;
};


void parse_args(int argc, char **argv, struct args *args);
int do_tmux(struct args *args, int child);


int main(int argc, char **argv) {

    struct args args;

    int child;
    int p0[2];
    int p1[2];
    char dummy;
    int stat;

    int tmux;

    parse_args(argc, argv, &args);

    pipe(p0);
    pipe(p1);

    if ((child = fork())) {

        read(p0[0], &dummy, 1);
        kill(child, SIGSTOP);
        write(p1[1], &dummy, 1);

        tmux = do_tmux(&args, child);
        waitpid(child, &stat, 0);
        kill(tmux, SIGTERM);

        return 0;

    } else {

        setpgid(0, 0);
        write(p0[1], &dummy, 1);
        read(p1[0], &dummy, 1);
        execvp(args.cmdv[0], args.cmdv);
        return 1;

    }

}


void parse_args(int argc, char **argv, struct args *args) {

    int i;
    char c;
    char *pt;

    args->dir = VERTICAL;
    args->st = NONE;

    while ((c = getopt(argc, argv, "hzs:")) != -1) {
        switch (c) {
            case 'z':
                args->dir = HORIZONTAL;
                break;
            case 's':
                args->size = optarg;
                while (1) {
                    if ('0' <= *optarg && *optarg <= '9') {
                        optarg++;
                    } else if (*optarg == '%' && *(optarg + 1) == 0) {
                        args->st = PERCENT;
                        *optarg = 0;
                        break;
                    } else if (*optarg == 0) {
                        args->st = ABSOLUTE;
                        break;
                    } else {
                        usage(1);
                    }
                }
                break;
            case 'h':
                usage(0);
            default:
                usage(1);
        }
    }

    if (argc - optind < 1) {
        usage(1);
    }

    args->cmdv = argv + optind - 1;
    for (i = optind - 1; i < argc - 1; i++) {
        argv[i] = argv[i + 1];
    }
    argv[argc - 1] = NULL;

}


int do_tmux(struct args *args, int child) {

    int pid;
    FILE *out;
    char line[256];
    char cmd[1024];

    sprintf(cmd, "tmux"
        " set-window-option -q synchronize-panes off ';'"
        " set-window-option -q remain-on-exit off ';'"
        " split-window -P -F '#{pane_pid}' %s %s %s"
        " sh -c 'reptyr %d && kill -CONT %d && sleep infinity'",
        args->dir == HORIZONTAL ? "-h" : "",
        args->st == ABSOLUTE ? "-l" : args->st == PERCENT ? "-p" : "",
        args->st == NONE ? "" : args->size,
        child, child);

    out = popen(cmd, "r");
    fgets(line, sizeof(line), out);
    sscanf(line, "%d\n", &pid);
    return pid;
}
