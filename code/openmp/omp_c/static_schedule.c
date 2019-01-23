#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "omp.h"

int main () 
{
  #pragma omp parallel for schedule(static)
  for ( int i=0; i<40; i++ )
  { 
    int tid = omp_get_thread_num();
    printf ("%d th iteration of block in thread %d.\n", i, tid );
  }
}

