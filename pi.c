#include <mpi.h>
#include <stdio.h>

float f (float x){
    return (4/(1+(x*x)));
}

float trap (float local_a, float local_b, int local_n, float h) {
    float integral;
    float x;
    int i;
    integral = (f(local_a) + f(local_b))/2.0;
    x = local_a;
    for (i = 1; i <= local_n-1; i++) {
        x += h;
        integral += f(x);
    }
    integral *= h;
    return integral;
}


int main(int argc, char** argv) {

    float a = 0.0;
    float b = 1.0;
    int n = 1024;
    float h;
    float local_a;
    float local_b;
    int local_n;
    float integral;
    float total;
    int source;
    int dest = 0;
    int tag = 50;
    MPI_Init(argc, argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    h = (b-a)/n;
    local_n = n/world_size;

    local_a = a + world_rank*local_n*h;
    local_b = local_a + local_n * h;
    integral = trap(local_a, local_b, local_n, h);

    if(world_rank == 0){
      total = integral;
      for (source = 1; source < world_rank; source++) {
	MPI_Recv(&integral, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	total += integral;
      }

    } else {
      MPI_Send(&integral, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
     
    }
    if(world_rank == 0){
      printf("Estimation : %f\n", total);
    }
    
    MPI_Finalize();
}
