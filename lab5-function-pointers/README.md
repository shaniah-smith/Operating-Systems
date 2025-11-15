# Function Pointer Lab — Task 1 & Task 2  
## Exact Instructions to Compile and Run

This README contains the precise commands required to compile and run Task 1 and Task 2 in the Codio environment.

---

# TASK 1 — Process Sorting with qsort

Task 1 is located in:

/home/codio/workspace/task1

The input file is:

/home/codio/workspace/task1/test-data/test1.csv

To compile and run Task 1, use the following exact commands:

------------------------------------------------------------
cd /home/codio/workspace/task1

gcc func-ptr.c util.c process.c -o func-ptr

./func-ptr test-data/test1.csv
------------------------------------------------------------

If successful, the program will print the sorted processes in the format:

pid (priority, arrival_time)

---

# TASK 2 — Function Pointer Calculator (No Conditionals)

Task 2 is located in:

/home/codio/workspace/task2

To compile and run Task 2, use:

------------------------------------------------------------
cd /home/codio/workspace/task2

gcc task2.c -o calc

./calc
------------------------------------------------------------

The program will prompt for:
- two hardcoded operands (6 and 3)
- an operation value:
  0 = add  
  1 = subtract  
  2 = multiply  
  3 = divide  
  4 = exit  

Example interaction:

Operand 'a' : 6 | Operand 'b' : 3  
Specify the operation to perform (0 : add | 1 : subtract | 2 : multiply | 3 : divide | 4 : exit): 2  
Multiplying 'a' and 'b'  
x = 18  

---

# End of README

