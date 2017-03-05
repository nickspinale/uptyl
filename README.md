# uttyl

Some tty/pty-related hacks that I find useful.

## parpty

```
Usage: partpy [-ioe] DRIVER_SCRIPT SLAVE [SLAVE_ARGS ...]
```

Creates a new pty pair and forks:
*   `sh -c DRIVER_SCRIPT FD` where `FD` is the master end's file descriptor.
*   `SLAVE [SLAVE_ARGS ...]` with the slave end as the controlling terminal.
    Sets the file descriptors given by `[-ioe]` (in, out, err) to the slave end.

See `./examples/popup-fzf` for an example usage.

## tmuxdo

```
Usage: tmuxdo [-ioe] [-u|d|l|r] [-s SIZE[%]] [--] CMD [ARGS ...]
```

Runs the given `CMD` with `ARGS` in a new tmux pane (whose location/size is optionally given).
The file descriptors given by `[-ioe]` are set to the pane's pty,
and the rest are (in effect) not changed from those of the calling process.

This program depends on [reptyr]
If it doesn't work, there may be a `ptrace` problem.
Try `sudo sh -c 'echo 0 > /proc/sys/kernel/yama/ptrace_scope'`.

## MORE DOCUMENTATION COMING SOON
