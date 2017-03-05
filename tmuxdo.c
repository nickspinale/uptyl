#define _GNU_SOURCE

#include <fcntl.h>
#include <malloc.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>


extern char *program_invocation_short_name;

static void usage(int code) {
    fprintf(stderr, "Usage: %s [-z] [-s SIZE[%]] CMD [ARGS ...]\n", program_invocation_short_name);
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


char **tmux_argv(struct args *args, int child) {

    int i = 0;
    char *cmd = malloc(256);
    char **argv = malloc(256);

    sprintf(cmd, "reptyr %d && kill -CONT %d && sleep infinity", child, child);

    argv[i++] = "tmux";
    argv[i++] = "set-window-option";
    argv[i++] = "-q";
    argv[i++] = "synchronize-panes";
    argv[i++] = "off";
    argv[i++] = ";";
    argv[i++] = "set-window-option";
    argv[i++] = "-q";
    argv[i++] = "remain-on-exit";
    argv[i++] = "off";
    argv[i++] = ";";
    argv[i++] = "split-window";
    argv[i++] = "-P";
    argv[i++] = "-F";
    argv[i++] = "#{pane_pid}";

    if (args->dir == HORIZONTAL) {
        argv[i++] = "-h";
    }
    if (args->st != NONE) {
        argv[i++] = args->st == PERCENT ? "-p" : "-l";
        argv[i++] = args->size;
    }

    argv[i++] = "sh";
    argv[i++] = "-c";
    argv[i++] = cmd;
    argv[i++] = NULL;

    return argv;
}


int do_tmux(struct args *args, int child) {

    int pid;
    int fd[2];
    char line[256];
    char **targv;
    FILE *out;

    targv = tmux_argv(args, child);

    pipe(fd);

    if (fork()) {
        out = fdopen(fd[0], "r");
        fgets(line, sizeof(line), out);
        sscanf(line, "%d\n", &pid);
        return pid;
    } else {
        dup2(fd[1], 1);
        execvp(targv[0], targv);
    }

}


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
