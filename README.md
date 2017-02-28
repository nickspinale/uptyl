# uptyl

Some pty-related hacks that I find useful.

## parpty.c

```
usage: partpy [-ioe] DRIVER_SCRIPT SLAVE [SLAVE_ARGS ...]
```

Creates a new pty pair and forks:
*   `sh -c DRIVER_SCRIPT FD` where `FD` is the master end's file descriptor.
*   `SLAVE [SLAVE_ARGS ...]` with the slave end as the controlling terminal.
    Sets the file descriptors given by `[-ioe]` (in, out, err) to the slave end.

See `./examples/popup-fzf` for an example usage.

## tmuxdo

```
usage: tmuxdo [-ioe] [-u|d|l|r] [-s SIZE[%]] [--] CMD [ARGS ...]
```

Runs the given `CMD` with `ARGS` in a new tmux pane (whose location/size is optionally given).
The file descriptors given by `[-ioe]` are set to the pane's pty,
and the rest are (in effect) not changed from those of the calling process.

## MORE DOCUMENTATION COMING SOON
