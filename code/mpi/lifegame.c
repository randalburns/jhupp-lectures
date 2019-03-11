/*******************************************************************************
*
*    $Id: $
*
*    Randal C. Burns
*    Department of Computer Science
*    Johns Hopkins University
*
*    $Source: $
*    $Date: $        
*    $Revision: $
*
*******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mpi.h"

int newValue ( int row, int column, int dimension, int* grid )
{
  int sum = 0;

  /* Not doing updates in the first row or last row 
   *  These are replicated and are non-local data.
   */
  assert ( row > 0 );
  assert ( row < dimension + 2 );

  /* Pick a left and right neighbor (wrap around horizontally */
  int left = (column == 0 ? dimension-1 : column-1 );
  int right = ( column == dimension -1 ? 0 : column+1 );

//  printf ( "L %d, R %d\n", left, right ); 

  sum = grid [ (row-1) * dimension + left ] +
        grid [ (row-1) * dimension + column ] +
        grid [ (row-1) * dimension + right ] +
        grid [ (row) * dimension + left ] +
        grid [ (row) * dimension + right ] +
        grid [ (row+1) * dimension + left ] +
        grid [ (row+1) * dimension + column ] +
        grid [ (row+1) * dimension + right ];

//  printf ( "Cell %d,%d. Sum %d\n", row, column, sum );

  if ( sum == 3 )
  {
    return 1;
  }
  else if ( sum == 2 )
  {
    return grid [ row* dimension + column ];
  }
  else
  {
    return 0;
  }
}

int main ( int argc, char** argv )
{
  /* Iteration variables */
  int i,j,k;

  /* Simulation variables */
  int value = 0;
  int incoming;

  /* Local data */
  int num_els;    //in local array
  int offset;     //local-> global
  int * grid, * tmp_grid, * print_grid;

  int iterations = 12;

  /* Global constant data */
//  const int dimension = 8;     // assume a square grid
//  const int global_grid[ 64 ] = { 0,0,0,0,0,0,0,0,
//                                  0,0,0,0,0,0,0,0,
//                                  0,0,0,0,0,0,0,0,
//                                  0,0,0,0,0,0,0,0,
//                                  0,0,1,1,1,1,0,0,
//                                  0,0,0,0,0,0,0,0,
//                                  0,0,0,0,0,0,0,0,
//                                  0,0,0,0,0,0,0,0 };

  /* Global constant data */
  const int dimension = 8;     // assume a square grid
  const int global_grid[ 64 ] = { 0,1,0,0,0,0,0,0,
                                  0,0,1,0,0,0,0,0,
                                  1,1,1,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0,
                                  0,0,0,0,0,0,0,0 };


  /* MPI Standard variable */
  int num_procs;
  int ID;

  /* Messaging variables */
  MPI_Status stat;
  int prev;
  int next; 

  /* MPI Setup */
  if ( MPI_Init(&argc, &argv) != MPI_SUCCESS )
  {
    printf ( "MPI_Init error\n" );
  }

  MPI_Comm_size ( MPI_COMM_WORLD, &num_procs );
  MPI_Comm_rank ( MPI_COMM_WORLD, &ID );

  /* Initialize process specific state */
  next =  ( ID + 1 ) % num_procs;
  prev = ID == 0 ? num_procs -1 : ID-1;   

  if ( ID == 0 ) 
  {
    print_grid = malloc ( dimension * dimension * sizeof(int) );
  }

  /* Setup the game of life */
  assert ( dimension % num_procs == 0 );
  grid = malloc ( dimension * sizeof(int) * ( dimension / num_procs + 2 ));  
  tmp_grid = malloc ( dimension * sizeof(int) * ( dimension / num_procs + 2 ));  

  for ( k=0; k< dimension; k++ )
  {
    grid[k] = 8;
    grid[ dimension * ( dimension / num_procs + 1 ) + k ] = 9;
  }

  num_els = dimension * dimension / num_procs;
  offset = ID * num_els;

  /* Initialize the local grid 
   *   First row belongs to lower neighbor
   *   next dimension/num_procs rows are local
   *   last row belongs to upper neighboth
   */
  for ( i=dimension; i < dimension + num_els; i++ )
  {
    grid[i] = global_grid[ (i-dimension) + offset ];
  }

//  printf ( "%d:*Initial grid state \n", ID);
//  for ( j=0; j<dimension/num_procs + 2; j++ )
//  {
//    for ( k=0; k < dimension; k++ )
//    {
//      printf ( "%d ", grid[j*dimension + k] );
//    }
//    printf ( "\n" );
//  }

  for ( i =0; i < iterations; i++ )
  {
    /* Send and receive point to point messages */
    if ( ID % 2 == 0 )
    {
      MPI_Send ( &grid[num_els], dimension, MPI_INT, next, 2, MPI_COMM_WORLD ); 
      MPI_Send ( &grid[dimension], dimension, MPI_INT, prev, 2, MPI_COMM_WORLD ); 
      MPI_Recv ( &grid[0], dimension, MPI_INT, prev, 2, MPI_COMM_WORLD, &stat );
      MPI_Recv ( &grid[num_els+dimension], dimension, MPI_INT, next, 2, MPI_COMM_WORLD, &stat );
    }
    else 
    { 
      MPI_Recv ( &grid[0], dimension, MPI_INT, prev, 2, MPI_COMM_WORLD, &stat );
      MPI_Recv ( &grid[num_els+dimension], dimension, MPI_INT, next, 2, MPI_COMM_WORLD, &stat );
      MPI_Send ( &grid[num_els], dimension, MPI_INT, next, 2, MPI_COMM_WORLD ); 
      MPI_Send ( &grid[dimension], dimension, MPI_INT, prev, 2, MPI_COMM_WORLD ); 
    }

    /* update the values */
    for ( j=1; j< num_els / dimension + 1; j++ )
    {
      for ( k=0; k< dimension; k++ ) 
      {
        tmp_grid[ j * dimension + k ] = newValue ( j, k, dimension, grid ); 
//        printf ( "Call on element %d returned %d\n", j * dimension + k, tmp_grid[ j * dimension + k ]);
      }
    } 

    for ( j=1; j< num_els / dimension + 1; j++ )
    {
      for ( k=0; k< dimension; k++ ) 
      { 
        grid [ j* dimension + k ] = tmp_grid [ j* dimension + k ];
      }
    }
  
    if ( ID == 0 )
    {
      for ( j=1; j< num_els / dimension + 1; j++ )
      {
        for ( k=0; k< dimension; k++ )
        {
          print_grid [ (j-1)* dimension + k ] = grid [ j* dimension + k ];
        }
      }
    
      for ( j=1; j<num_procs; j++ )
      {
        MPI_Recv ( &print_grid[j*num_els], num_els, MPI_INT, j, 3, MPI_COMM_WORLD, &stat );
      }

      printf ( "Iteration %d: updated grid\n", i);
      for ( j=0; j<dimension; j++ )
      {
        for ( k=0; k < dimension; k++ )
        {
          printf ( "%d ", print_grid[j*dimension + k] );
        }
        printf ( "\n" );
      }
  }
  else 
  {
    MPI_Send ( &grid[dimension], num_els, MPI_INT, 0, 3, MPI_COMM_WORLD ); 
  }

//    printf ( "%d:%d updated grid state \n", ID, i);
//    for ( j=0; j<dimension/num_procs + 2; j++ )
//    {
//      for ( k=0; k < dimension; k++ )
//      {
//       printf ( "%d ", grid[j*dimension + k] );
//      }
//      printf ( "\n" );
//    }
  } 
 
}

/*******************************************************************************
*
*  Revsion History 
*    
*  $Log: $
*    
*******************************************************************************/
