#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))

const int INFTY = 1000000;

void findDist(int SOURCE, int p, int n, int *local_edge, int my_rank);
int choose(int *dist, int n, int *found);
int getEdge(int j, int rank_source, int p, int n,
            int SOURCE,int *local_edge,int my_rank);
int findRank(int j, int p, int n);
//void printEdge(int *local_edge, int n, int p);

int main(int argc, char * argv[]){
  int n = 4;
  int edge[4][4]={
    {0, 1, 3, 0},
    {1, 0, 1, 1},
    {3, 1, 0, 1},
    {0, 1 ,1, 0}
  };

  int* local_edge;
  MPI_Comm comm;
  int p, my_rank;
  int SOURCE = 5;

  MPI_Init(&argc, &argv);
  comm = MPI_COMM_WORLD;
  MPI_Status status;
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  local_edge = malloc(n*n/p * sizeof(int));

  //distribute edge to processes
  MPI_Scatter(edge, n*n/p, MPI_INT,local_edge, n*n/p, MPI_INT, 0, comm);
  findDist(SOURCE, p, n, local_edge, my_rank);
  // printf("The sub edge in rank %d is: \n", my_rank);
  // printEdge(local_edge, n, p);

  //determin  process from which SOURCE caculates
  // from my_rank = SOURCE/(n/p), the SOURCE%(n/p) row in that rank

  MPI_Finalize();
  return 0;
}

// findDist function, add another argument p to the function.
void findDist(int SOURCE, int p, int n, int *local_edge, int my_rank){
  int i, j, count, *found, *dist;
  int rank_source, tmp_edge;
  int local_Sour = SOURCE%(n/p);

  rank_source = findRank(SOURCE, p, n);
  found = malloc(n * sizeof(int));
  dist = malloc(n * sizeof(int));

  if(my_rank == rank_source){

    //initialization
    for(i = 0; i < n; i++){
      found[i] = 0;
      dist[i] = local_edge[local_Sour * n + i];
    }
    found[local_Sour] = 1;
    count = 1;

    //find the minimun distance from local_Sour to n
    while(count < n){
      j = choose(dist, n, found);
      tmp_edge = getEdge(j, rank_source, p, n, SOURCE, local_edge, my_rank);

      found[j] = 1;
      count ++;
      for(i = 0; i < n; i++){
        if( !(found[i]) )
          dist[i] = min(dist[i], dist[j] + tmp_edge);
      }
    }
    free(found);

  }
  for (int i = 0; i < n; i++){
    printf("The distance from %d to %d is %d\n",SOURCE, i, dist[i]);
  }
  printf("\n");
}

//choose the position with minimum distance in row local_Sour
int choose(int *dist, int n, int *found){
  int i, tmp, least = INFTY, leastPosition;
  for(i = 0; i < n; i++){
    tmp = dist[i];
    if( !(found[i]) && (tmp < least) ){
      least = tmp;
      leastPosition = i;
    }
  }
  return leastPosition;
}

int getEdge(int j, int rank_source, int p, int n,
            int SOURCE,int *local_edge, int my_rank){
  int rank_j, *tmp_j, *tmp_edge;
  MPI_Status status;

  rank_j = findRank(j, p, n);

  if(my_rank == rank_source){
    //send j's value to rank_j
    MPI_Send(&j, 1, MPI_INT,rank_j,99, MPI_COMM_WORLD);
    // receive the edge[j][i] from other rank, store in tmp_edge
    MPI_Recv(tmp_edge, 1, MPI_INT,MPI_ANY_SOURCE, 98,
      MPI_COMM_WORLD, &status);
  }else if(my_rank == rank_j){
    MPI_Recv(tmp_j, 1, MPI_INT, MPI_ANY_SOURCE, 99,
      MPI_COMM_WORLD, &status);
    // send the edge[j][i] to the rank where the SOURSE is
    MPI_Send(&local_edge[(*tmp_j) % (n/p) * n + SOURCE],1, MPI_INT,
      rank_source, 98, MPI_COMM_WORLD);
  }
  return *tmp_edge;
  free(tmp_edge);
}

// find j is in which rank
int findRank(int j, int p, int n){
  return j/(n/p);
}

// void printEdge(int *local_edge, int n, int p){
//   for (int i = 0; i < n/p; i++){
//     for(int j = 0; j < n; j++){
//       printf("%d\t", local_edge[i*n + j]);
//     }
//     printf("\n");
//   }
//   printf("\n");
// }
