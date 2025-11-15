#include <stdio.h>
#include <stdlib.h>

/* IMPLEMENT ME: Declare your functions here */
int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
int divide(int a, int b);
int do_exit(int a, int b);

int main(void)
{
    int a = 6;
    int b = 3;
    char op;

    /* Array of function pointers */
    int (*ops[5])(int, int) = { add, subtract, multiply, divide, do_exit };

    /* Print operands */
    printf("Operand 'a' : %d | Operand 'b' : %d\n", a, b);
    printf("Specify the operation to perform (0 : add | 1 : subtract | 2 : Multiply | 3 : divide | 4 : exit): ");
    scanf(" %c", &op);

    /* Convert ASCII '0'..'4' to integer index */
    int index = op - '0';

    /* Call the function pointer */
    int x = ops[index](a, b);

    /* Print result (exit function never returns) */
    printf("x = %d\n", x);

    return 0;
}

/* IMPLEMENT ME: Define your functions here */

int add(int a, int b) { 
    printf("Adding 'a' and 'b'\n"); 
    return a + b; 
}

int subtract(int a, int b) {
    printf("Subtracting 'b' from 'a'\n");
    return a - b;
}

int multiply(int a, int b) {
    printf("Multiplying 'a' and 'b'\n");
    return a * b;
}

int divide(int a, int b) {
    printf("Dividing 'a' by 'b'\n");
    return a / b;   /* integer division */
}

int do_exit(int a, int b) {
    (void)a;
    (void)b;
    exit(0);
    return 0;
}
