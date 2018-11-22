#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "NSMpi.h"

int main(int argc, char **argv) {
    int nx = 1024, ny = 128;

    MPI_Init(NULL, NULL);
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    class NSMpi *nsmpi = NULL;
    double *errors = NULL;
    double error = 1;
    if(world_rank == 0) {
        nsmpi = new NSMpi(world_rank, world_size, nx/world_size + nx%world_size + 2, ny);
        errors = new double[world_size];
    } else {
        nsmpi = new NSMpi(world_rank, world_size, nx/world_size + 2, ny);
    }

    int step = 0;
    double time = 0.0;
    MPI_Barrier(MPI_COMM_WORLD);
    time -= MPI_Wtime();
    if(world_size == 1){
        while(error > 0.04) {
            nsmpi->next();
            error = nsmpi->getErrorX();
            if((++step) % 100 == 0)
                printf("step %d : error = %f\n", step, error);
            nsmpi->exchange_message();
        }
        nsmpi->save();
    } else {
        while(error > 0.04) {
            nsmpi->next();
            nsmpi->gather_errors(errors);
            if(world_rank == 0) {
                error = 0;
                for(int i = 0; i < world_size; ++i) {
                    error += errors[i];
                }
                if((++step) % 1000 == 0)
                    printf("step %d : error = %f\n", step, error);
            }
            MPI_Bcast(&error, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            nsmpi->exchange_message();     
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    time += MPI_Wtime();
    if(world_rank == 0) {
        printf("time = %f \n", time);
        delete[] errors;

        FILE *fp;
        fp = fopen("log/log.out", "a+");
        if(argc == 2) {
            fprintf(fp, "%s ", argv[1]);
        }
        fprintf(fp, "%f\n", time); 
        fclose(fp);
    }
}
