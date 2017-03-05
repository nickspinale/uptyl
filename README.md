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
Usage: tmuxdo [-z] [-s SIZE[%]] [--] CMD [ARGS ...]
```

Effectively runs the given `CMD` with `ARGS` in a new tmux pane by changing all file descriptors that point to the current controlling terminal to the pty newly created by tmux.
`-z` splits horizontally instead of vertically.

This program depends on [reptyr](https://github.com/nelhage/reptyr).
If it doesn't work, there may be a `ptrace` problem.
Try `sudo sh -c 'echo 0 > /proc/sys/kernel/yama/ptrace_scope'`.

## MORE DOCUMENTATION COMING SOON
