/*
 * Write a program to see what happens if we try to longjmp() into a function
 * that has already returned.
 */

#include "tlpi_hdr.h"
#include <setjmp.h>

static jmp_buf env;

void f1() {
  int x = 1;
  switch (setjmp(env)) {
  case 0:
    x = 2;
    printf("First setjmp call. x=%d\n", x);
    break;
  case 1:
    printf("Second setjmp call. x=%d\n", x);
    break;
  }
}

int main(void) {
  f1();

  longjmp(env, 1);
  printf("After longjmp\n");

  exit(EXIT_SUCCESS);
}
