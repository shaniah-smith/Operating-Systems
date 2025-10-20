# Project 1 – Create a Simple Shell (Quash)

**Author:** Shaniah Smith
**Course:** Operating Systems
**Instructor:** Dr. Legand Burge
**Date:** October 2025

---

## **1. Objective**

The goal of this project was to implement a simplified command-line shell (Quash) in C that mimics the basic functionality of Unix shells such as `bash` or `sh`. Through this project, I learned how to:

* Create and manage child processes using `fork()` and `execvp()`
* Wait for and handle process termination with `waitpid()`
* Manage background and foreground processes
* Handle I/O redirection using `dup2()` and file descriptors
* Use environment variables and signals
* Implement simple parsing and tokenization of command-line input

---

## **2. Design Overview**

The shell is composed of a single main loop that:

1. Displays a prompt showing the current working directory.
2. Reads a command line from the user.
3. Tokenizes the input into arguments.
4. Handles built-in commands directly.
5. Spawns child processes for external commands.
6. Waits for foreground jobs or manages background jobs.
7. Handles I/O redirection and signals safely.

---

## **3. Implementation Details**

### **3.1 Command Parsing**

The shell uses `fgets()` to read input and `strtok()` to tokenize it based on whitespace (`' \t\r\n'`). Tokens are stored in an array of character pointers (`arguments[]`). This array is passed directly to `execvp()` when executing external commands.

### **3.2 Built-in Commands**

Several commands are implemented directly in the shell (without forking):

| Command                | Description                                                                  |
| ---------------------- | ---------------------------------------------------------------------------- |
| `cd [dir]`             | Changes current directory using `chdir()`.                                   |
| `pwd`                  | Prints the current working directory using `getcwd()`.                       |
| `echo [args]`          | Prints arguments to the terminal; supports `$VAR` for environment variables. |
| `setenv [VAR] [VALUE]` | Adds or updates environment variables using `setenv()`.                      |
| `env`                  | Displays all environment variables from `environ`.                           |
| `exit`                 | Exits the shell cleanly.                                                     |

### **3.3 External Commands**

For non-built-in commands, the shell:

1. Forks a child process.
2. Uses `execvp()` to execute the command.
3. Waits for the child to complete using `waitpid()` (unless the command is run in the background with `&`).

If a command ends with `&`, the shell does **not wait** for it, but instead prints a background process ID message.

### **3.4 I/O Redirection**

Implemented using `dup2()`:

* **Output redirection (`>`):**

  ```bash
  ls > out.txt
  ```

  Opens `out.txt` with `O_WRONLY | O_CREAT | O_TRUNC` and redirects `STDOUT_FILENO`.
* **Input redirection (`<`):**

  ```bash
  more < file.txt
  ```

  Opens `file.txt` and redirects `STDIN_FILENO`.
* Both can be combined for full credit; pipes (`|`) can be implemented using `pipe()` and two forks (extra credit).

### **3.5 Signal Handling**

`SIGINT` (Ctrl-C) and `SIGALRM` (for timeouts) are handled using `sigaction`:

* Ctrl-C kills only the foreground process, not the shell.
* An alarm is set (`alarm(10)`) for long-running processes, automatically terminating any process exceeding 10 seconds.

### **3.6 Environment Variables**

The shell uses `extern char **environ;` to access system variables.
It supports:

* Expanding `$VAR` in commands (`echo $HOME`)
* Setting custom variables with `setenv`
* Printing with `env`

---

## **4. Testing and Validation**

### **Test 1 – Built-in Commands**

```
pwd
cd testDir1
pwd
```

**Result:** Displays correct working directory before and after `cd`.

### **Test 2 – Echo and Environment**

```
echo hello $HOME
setenv greeting hello
env greeting
```

**Result:** Prints the correct path for `$HOME` and shows `greeting=hello`.

### **Test 3 – Process Creation**

```
ls
cat shell.c | grep main
```

**Result:** Lists files and filters output correctly (if pipe implemented).

### **Test 4 – Background and Foreground**

```
sleep 20 &
[bg] pid 1853
sleep 20
```

**Result:** Background job returns immediately; foreground job terminates after ~10 seconds due to timeout.

### **Test 5 – I/O Redirection**

```
ls > out.txt
cat out.txt
```

**Result:** `out.txt` contains directory listing.

### **Test 6 – Signal Handling**

```
sleep 100
```

(Press **Ctrl-C**)
**Result:** Sleep is terminated, shell prompt returns without exiting.

### **Test 7 – Input Redirection**

```
cat < testData
```

**Result:** Displays contents of the file correctly.

---

## **5. Challenges and Lessons Learned**

* Managing **signal handling** without accidentally killing the shell required care with process IDs.
* Token parsing for environment variable expansion was tricky due to `$VAR` syntax handling.
* Redirection required understanding of **file descriptors** and how `dup2()` reassigns them.
* Ensuring background and foreground synchronization with `waitpid()` and signal-safe prints.

---

## **6. How to Compile and Run**

### **Compilation**

Using the provided Makefile:

```bash
make
```

This compiles `shell.c` into an executable named `ref_shell` (or `shell2` if modified in the Makefile).

To clean up compiled binaries:

```bash
make clean
```

### **Execution**

Run the shell using:

```bash
./ref_shell
```

You should see the prompt displaying the current working directory, for example:

```
/home/codio/workspace>
```

### **Example Commands to Test**

```
pwd
cd testDir1
echo hello $HOME
setenv greeting hi
env greeting
ls > output.txt
cat output.txt
sleep 20 &
sleep 100  # Then press Ctrl-C to test signal handling
exit
```

---

## **7. Conclusion**

This project deepened my understanding of Linux process control, signal management, and input/output redirection. By implementing a fully functional mini-shell, I practiced critical systems programming concepts such as forking, execing, and managing concurrent processes.

The resulting shell successfully implements:

* All tasks 1–6 (prompt, built-ins, fork/exec, signals, background jobs, I/O redirection)
* Proper environment management and process cleanup
* Extensible structure for extra credit features like pipes and job control
