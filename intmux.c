#define _GNU_SOURCE

#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

extern char *program_invocation_short_name;

static void usage(int code) {
    FILE *out = code ? stderr : stdout;
    fprintf(out, "USAGE: %s CMD [ARGS ...]\n", program_invocation_short_name);
    exit(code);
}

int main(int argc, char **argv) {

    int child;
    int p0[2];
    int p1[2];
    char dummy;
    int stat;

    int tmux;
    FILE *tmux_out;
    char line[256];
    char cmd[1024];

    int i;

    if (argc < 2) {
        usage(1);
    }

    pipe(p0);
    pipe(p1);

    if ((child = fork())) {

        read(p0[0], &dummy, 1);
        kill(child, SIGSTOP);
        write(p1[1], &dummy, 1);

        sprintf(cmd,
                "tmux splitw -PF '#{pane_pid}' "
                "sh -c \"reptyr %d && kill -CONT %d && sleep infinity\"",
                child, child);

        tmux_out = popen(cmd, "r");
        fgets(line, sizeof(line), tmux_out);
        sscanf(line, "%d\n", &tmux);

        waitpid(child, &stat, 0);
        kill(tmux, SIGTERM);
        return 0;

    } else {

        setpgid(0, 0);
        write(p0[1], &dummy, 1);
        read(p1[0], &dummy, 1);

        for (i = 0; i < argc - 1; i++) {
            argv[i] = argv[i + 1];
        }
        argv[argc - 1] = NULL;

        execvp(argv[0], argv);
        return 1;
    }
}
