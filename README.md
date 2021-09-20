# `crash`, the shell that hopefully doesn't _crash_

## compilation instructions

1. simply type `make`
2. run with `./308sh`

## todo

- [x] assign prompt if passed in
- [x] leave default if not
- [x] print error if flag is used, but with no prompt
- [ ] accept user input
- [ ] parse commands and flags
- [ ] program commands (using `execvp()`)
- [ ] notify user if command is not found
- [ ] print pid of command when spawned, including name of command
- [ ] wait for child to exit by default
- [ ] when a child process exits, print out the pid and exit status
- [ ] backgrounding tasks
- [x] `exit`
- [x] `pid`
- [x] `ppid`
- [x] `cd`
- [x] `cd <dir>`
- [x] `pwd

## extra credit

- [ ] jobs command