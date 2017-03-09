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

`tmuxtry` does the same thing, unless `$TMUX` is not set, in which case `CMD [ARGS ...]` is run normally.

## urxvto

```
urxvtdo [-ioe] [-g GEOMETRY] [--] CMD [ARGS ...]
```

Effectively runs the given `CMD` with `ARGS` in a new urxvt window by changing the given file descriptors to the pty newly created by urxvt.
`-g` causes the new window to be transient with the given geometry.

## withtty [TODO]

```
Usage withtty [-twb] [-mcx] [--] CMD [ARGS ...]
```

Effectively runs the given `CMD` with `ARGS` in some tty somewhere, based on the environment and some given preferences.

`-m`, `-c`, and `-x` indicate preference order of a tmux pane, the current tty, or a urxvt window.
Those not provided as arguments will be filled in afterwards in the order `mxc`.

`-t` is for a tall tty, `-w` for wide, and `-b` for both ("big").
