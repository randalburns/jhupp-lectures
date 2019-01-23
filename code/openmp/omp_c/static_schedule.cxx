#include <iostream>
#include "omp.h"


int main () 
{
  int count=0;

  std::cout.setf(std::ios::unitbuf);

  #pragma omp parallel for schedule(static) firstprivate(count)
  for ( int i=0; i<40; i++ )
  { 
    int tid = omp_get_thread_num();
    count++;
    std::cout << count << " th iteration of block in thread " << tid << std::endl;
  }
}

