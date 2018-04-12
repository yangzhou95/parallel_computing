#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

void myBarrier(int,int);  //Function declaration

int main (int argc,char **argv){

    int rank, size;
    MPI_Init (&argc, &argv); 
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
 
	printf( "Process before myBarrier in rank %d \n", rank);
    sleep(5);
    myBarrier(rank,size);
    sleep(5);
	printf( "Process after  myBarrier in rank %d \n", rank);

    MPI_Finalize();
    return 0;
}
 
//myBarrier function 
void myBarrier(int rank,int size){

    int k = (int)log2(size);  //calculate value of log2P

    //Below are arrival phase 
    int i=0;
    int j=0;
    int step = 0; 
    char msg_1[] = "lower process reaches to the barrier";
    for(i=k-1;i>=0;i--){
        step += 1;
	    for(j=pow(2,i);j<pow(2,i+1);j++){
		    if(rank == j){
		        MPI_Send(msg_1, strlen(msg_1),MPI_CHAR, j-(int)pow(2,i), 99, MPI_COMM_WORLD);
			}
		    if(rank == j-(int)pow(2,i)){
		        MPI_Status status;
		        char recv[strlen(msg_1)];
		        MPI_Recv(&recv, strlen(msg_1),MPI_CHAR, j, 99, MPI_COMM_WORLD, &status);
		        printf("Rank %d receives message: '%s' from %d in step %d \n",j-(int)pow(2,i),msg_1, j, step);
			}
        }
    }


    //Below are departure phase
    i=0;
    j=0;
    step = 0;
    char msg_2[] = "upper process finish the barrier";
    for(i=0;i<=k-1;i++){
        step += 1;
	    for(j=0;j<=pow(2,i)-1;j++){
		    if(rank==j){
                printf("Rank %d sends message: '%s' to %d in step %d \n", j, msg_2, j+(int)pow(2,i), step);
		        MPI_Send(msg_2, strlen(msg_2),MPI_CHAR, j+(int)pow(2,i), 98, MPI_COMM_WORLD);
			}
	    	if(rank== j+(int)pow(2,i)){
		        MPI_Status status;
		        char recv[strlen(msg_2)];
		        MPI_Recv(recv, strlen(msg_2),MPI_CHAR, j, 98, MPI_COMM_WORLD, &status);
			 }
        }
    }

}
