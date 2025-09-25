# Part 2: Signals Lab  
**Author:** Shaniah Smith  
**Course:** Operating Systems  

---

## 📌 Overview
This lab demonstrates how to use **UNIX signals** in C. You will learn to set up signal handlers, use `alarm()` for timers, and handle interrupts like `Ctrl-C` (`SIGINT`).  

There are **three programs** in this part:  

1. **signal1.c**  
   - After 5 seconds, prints:  
     ```
     Hello World!
     Turing was right!
     ```  
   - Then exits.  

2. **signal2.c**  
   - Every 5 seconds, repeatedly prints:  
     ```
     Hello World!
     Turing was right!
     ```  
   - Runs forever until stopped with **Ctrl-C**.  

3. **timer.c**  
   - Prints `Hello World!` once per second.  
   - When stopped with **Ctrl-C**, it prints the total number of ticks (seconds elapsed) before exiting.  

---

## 🗂️ Files
- `signal1.c` — one-shot signal handling.  
- `signal2.c` — periodic signal handling.  
- `timer.c` — timer with SIGALRM every second and SIGINT handling.  
- `Makefile` — builds all executables.  

---

## ⚙️ Build Instructions
Run the following commands in your terminal:  

```bash
make        # build all programs
./signal1
./signal2
./timer
Stop with Ctrl-C
