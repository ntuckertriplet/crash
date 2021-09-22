# `crash`, the shell that hopefully doesn't _crash_

## compilation instructions

1. simply type `make`
2. run with `./308sh`

## todo

- [x] assign prompt if passed in
- [x] leave default if not
- [x] print error if flag is used, but with no prompt
- [x] accept user input
- [x] parse commands and flags
- [x] program commands (using `execvp()`)
- [x] notify user if command is not found
- [x] print pid of command when spawned, including name of command
- [x] wait for child to exit by default
- [x] when a child process exits, print out the pid and exit status // possible segfault
- [x] backgrounding tasks
- [x] `exit`
- [x] `pid`
- [x] `ppid`
- [x] `cd`
- [x] `cd <dir>`
- [x] `pwd

## extra credit

- [ ] jobs command // segfault

## things i added (or want to add) not in the spec

- [ ] get env vars
- [ ] set env vars
- [ ] file redirects
- [ ] pipes
