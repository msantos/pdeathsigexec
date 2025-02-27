# SYNOPSIS

pdeathsigexec *cmd* *...*

# DESCRIPTION

pdeathsigexec: signal process when parent exits

A subprocess whose parent exits can be re-parented to init (PID 1) and
continue to run. `pdeathsigexec` sets the process to have a signal sent
if the parent process terminates.

The "signal on parent termination" behaviour applies
to the executed process only and not descendents
([prctl(2)](https://man7.org/linux/man-pages/man2/prctl.2.html)):

```
The parent-death signal setting is cleared for the child of a fork(2).
It is also (since Linux 2.4.36 / 2.6.23) cleared when  executing  a
set-user-ID or set-group-ID binary, or a binary that has associated
capabilities (see capabilities(7)); otherwise, this value is preserved
across execve(2).
```

The behaviour can be set for related, dynamically linked processes in
a supervision tree by using a `LD_PRELOAD` library (see `EXAMPLES`).

# EXAMPLES

```
$ sh -c "sleep inf" &
[1] 25880
$ kill -9 25880
$ pgrep -fa sleep
25882 sleep inf
```

## pdeathsigexec

```
$ sh -c "pdeathsigexec sleep inf" &
[1] 25926
$ kill -9 25926
$ pgrep -fa sleep
<no output>
```

## libpdeathsigexec

```
$ LD_PRELOAD=libpdeathsigexec.so sh -c 'sh -c "sleep inf" & sleep inf' &
[1] 25987
$ kill -9 25987
$ pgrep -fa sleep
<no output>
```

# Build

```
make

#### static executable using musl
## sudo apt install musl-dev musl-tools

## linux seccomp process restrictions: requires kernel headers
export MUSL_INCLUDE=/tmp
git clone https://github.com/sabotage-linux/kernel-headers.git $MUSL_INCLUDE/kernel-headers
./musl-make
```

# OPTIONS

## pdeathsigexec

-s/--signal
: set the termination signal (default `9` (SIGKILL))

# ENVIRONMENT VARIABLES

PDEATHSIGEXEC_SIGNAL
: set the termination signal (default `9` (SIGKILL))
