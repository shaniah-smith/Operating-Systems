# Shaniah Smith @03110157
# Lab 2 (Part 1): Intro to Processes

This project demonstrates basic Unix process creation and management using `fork()`, `sleep()`, and `wait()` in C.

---

## 📂 Files
- **main.c** → Program 1 (`prog1`)
  - Demonstrates process creation with `fork()`.
  - Both parent and child print 20 lines, showing interleaved output.

- **main2.c** → Program 2 (`prog2`)
  - Similar to `prog1` but separates parent and child into their own functions (`ParentProcess()` and `ChildProcess()`).
  - Each process prints up to 200 values, then terminates.

- **main3.c** → Program 3 (`my3proc`)
  - The parent process forks **two children**.
  - Each child prints a message when going to sleep and waking up (random intervals).
  - The parent waits until both children finish, then prints completion messages.

- **Makefile**
  - Simplifies compilation with the following targets:
    - `make prog1` → builds `main.c`
    - `make prog2` → builds `main2.c`
    - `make my3proc` → builds `main3.c`
    - `make all` → builds all programs
    - `make clean` → removes compiled executables

---

## ⚙️ Build Instructions

make prog1     # main.c   -> prog1
make prog2     # main2.c  -> prog2
make my3proc   # main3.c  -> my3proc
# Run

./prog1      # Program 1: interleaved parent/child prints (1..20)
./prog2      # Program 2: parent & child print (1..200), then both finish
./my3proc    # Program 3: two children sleep/wake randomly; parent waits

From the project directory, run:

```bash
make all
