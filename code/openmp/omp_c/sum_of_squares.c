#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int square(int x) { return x*x; }

voidsum_of_squares(int len, int* a, int* b, int* c)
{
  for (int i=0; i<len; i++)
  {
    c[i] = square(a[i]) + square(b[i]);
  }    
}

int main ()
{
  // Initialize an A and B arrays
  int len = 1024;

  int a [len];
  int b [len];
  int c [len];

  clock_t begin, end;
  
  // Initialize vectors	
  for (int i=0; i<len; i++)
  {
      a[i] = rand();
      b[i] = rand();
  }

  begin = clock();
  sum_of_squares (len, a, b, c);
  end = clock();

  int time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Addition took %d seconds", time_spent);

  return 0;
}
