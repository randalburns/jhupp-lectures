#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int cube(int x) { return x*x*x; }

int sum(int x) {
  int result = 0;
  for (int i = 1; i <= x; ++i) result += i;
  return result;
}

int sumOfCubes(int x) {
  int result = 0;
  for (int i = 1; i <= x; ++i) result += cube(i);
  return result;
}

static
bool isPrime(int x) {
  for (int i = 2; i <= (int)sqrt(x); ++i) {
    if (x % i == 0) return false;
  }
  return true;
}

int getPrime(int x) {
  int count = 0;
  int candidate = 2;
  while (count != x) {
    if (isPrime(candidate))
      ++count;
  }
  return candidate;
}

int 

int main ()
{
  // Initialize an A and B arrays
  int len = 8*1024;

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
  // Vector addition
  for (int i=0; i<len; i++)
  {
    c[i] = a[i] + b[i];
  }
  end = clock();


  int time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Addition took %d seconds", time_spent);

  return 0;
}
