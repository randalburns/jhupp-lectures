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

/*
 *  onedlife.c : a simple cellular automata that implements a "game of life"
 *  like process in one dimension
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mpi.h"

/* 
 * Automata rules for cells that survive or die
 */
int newValue ( int lower, int value, int upper )
{
  if ((lower == 1) && (upper == 1))
  {
    return 1;
  }
  else if ((lower == 0) && (upper == 0)) 
  {
    return 0;
  }
  else 
  {
    return value;
  } 
}

/*
 * onedlife: monolithic main that conflates MPI stuff with algorithm: YUCK!
 */
int main ( int argc, char** argv )
{
  /* Iteration variables */
  int i,j,k;

  /* Simulation variables */
  /* Local data */
  int num_els;    //in local array
  int offset;     //local-> global
  int lower;      //value from lower neighbor
  int upper;      //value from upper neighbor
  int * grid, * tmp_grid;

  int iterations = 4;

  /* Global constant data */
  const int grid_width = 40;
  const int global_grid[ 40 ] = { 0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
                                  0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1 };
//  const int global_grid[ 40 ] = { 0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,
//                                  1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0 };

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

  /* Setup the 1-d game of life */
  assert ( grid_width % num_procs == 0 );
  grid = malloc ( sizeof(int) * grid_width / num_procs );  
  tmp_grid = malloc ( sizeof(int) * grid_width / num_procs );  

  num_els = grid_width / num_procs;
  offset = ID * num_els;

  for ( i=0; i < num_els; i++ )
  {
    grid[i] = global_grid[ i + offset ];
  }

  printf ( "%d:* Initial grid state ", ID);
  for ( j=0; j < num_els-1; j++ )
  {
    printf ( "%d, ", grid[j] );
  }
  printf ( "%d\n", grid[num_els -1] );

  for ( i =0; i < iterations; i++ )
  {
    /* Send and receive point to point messages */
    if ( ID % 2 == 0 )
    {
      MPI_Send ( &grid[num_els-1], 1, MPI_INT, next, 2, MPI_COMM_WORLD ); 
      MPI_Send ( &grid[0], 1, MPI_INT, prev, 2, MPI_COMM_WORLD ); 
      MPI_Recv ( &lower, 1, MPI_INT, prev, 2, MPI_COMM_WORLD, &stat );
      MPI_Recv ( &upper, 1, MPI_INT, next, 2, MPI_COMM_WORLD, &stat );
//      printf ( "%d:%d received %d from %d and %d from %d\n", ID, i, lower, prev, upper, next );
    }
    else 
    { 
      MPI_Recv ( &lower, 1, MPI_INT, prev, 2, MPI_COMM_WORLD, &stat );
      MPI_Recv ( &upper, 1, MPI_INT, next, 2, MPI_COMM_WORLD, &stat );
//      printf ( "%d:%d received %d from %d and %d from %d\n", ID, i, lower, prev, upper, next );
      MPI_Send ( &grid[num_els-1], 1, MPI_INT, next, 2, MPI_COMM_WORLD ); 
      MPI_Send ( &grid[0], 1, MPI_INT, prev, 2, MPI_COMM_WORLD ); 
    }

    /* update the values */
    tmp_grid[0] = newValue ( lower, grid[0], grid[1] ); 
    for ( j=1; j< num_els-1; j++ )
    {
      tmp_grid[j] = newValue ( grid[j-1], grid[j], grid[j+1] ); 
    }
    tmp_grid[num_els -1 ] = newValue ( grid[num_els-2], grid[num_els-1], upper ); 

    /* Copy over the old grid.  What else could we do here? */
    for ( j=0; j<num_els; j++ )
    {
      grid[j] = tmp_grid[j];
    }

    /* Output the update grid state */
    printf ( "%d:%d grid state ", ID, i );
    for ( j=0; j < num_els-1; j++ )
    {
      printf ( "%d, ", grid[j] );
    }
    printf ( "%d\n", grid[num_els -1] );
  }
}

/*******************************************************************************
*
*  Revsion History 
*    
*  $Log: $
*    
*******************************************************************************/
