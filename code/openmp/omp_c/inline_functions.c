#include <stdio.h>
#include <stdlib.h>
#include <time.h>


double square(int x) { return x*x; }
double cube (int x) { return x*x*x; }

int main ()
{
  // Initialize an A and B arrays
  int len = 1024;

  double a [len];
  double b [len];
  double c [len];

  clock_t begin, end;
  
  // Initialize vectors	
  for (int i=0; i<len; i++)
      a[i] = rand_double();
      b[i] = rand_double();
  }    

  begin = clock();
  // ridiculous function calls 
  for (int i=0; i<len; i++)
  {
    c[i] = square(a[i])/cube(a[i]) + square(a[i])/cube(a[i]) + 
  }    
  end = clock();

  int time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Addition took %d seconds", time_spent);

  return 0;
}
