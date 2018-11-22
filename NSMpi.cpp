#include "NSMpi.h"

NSMpi::NSMpi( int world_rank, int world_size, int nx, int ny)
    : NavierStokes(nx, ny), world_rank(world_rank), world_size(world_size) {}
    // matrix[0, :] and matrix[nx-1, :] are reserved for communication.

void 
NSMpi::exchange_message()
{
    if(world_size == 1) {
        for(int j = 0; j < ny; ++j) {
            p[0][j] = p[nx-2][j]; p[nx-1][j] = p[1][j];
            u[0][j] = u[nx-2][j]; u[nx-1][j] = u[1][j];
            v[0][j] = v[nx-2][j]; v[nx-1][j] = v[1][j];
        }
        return;
    }
    // prevent deadlock
    if(world_rank % 2 == 0) {
        send_data();
        recv_data();
    } else {
        recv_data();
        send_data();
    }
}

void
NSMpi::gather_errors(double *errors) 
{
    MPI_Gather(&err_x, 1, MPI_DOUBLE, errors, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void 
NSMpi::send_data()
{
    // send data to left neighbor
    if(world_rank > 0) {
        MPI_Send(p[1], ny, MPI_DOUBLE, world_rank-1, 0, MPI_COMM_WORLD);
        MPI_Send(u[1], ny, MPI_DOUBLE, world_rank-1, 0, MPI_COMM_WORLD);
        MPI_Send(v[1], ny, MPI_DOUBLE, world_rank-1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Send(p[1], ny, MPI_DOUBLE, world_size-1, 0, MPI_COMM_WORLD);
        MPI_Send(u[1], ny, MPI_DOUBLE, world_size-1, 0, MPI_COMM_WORLD);
        MPI_Send(v[1], ny, MPI_DOUBLE, world_size-1, 0, MPI_COMM_WORLD);
    }
    
    // send data to right neighbor
    if(world_rank < world_size-1) {
        MPI_Send(p[nx-2], ny, MPI_DOUBLE, world_rank+1, 0, MPI_COMM_WORLD);
        MPI_Send(u[nx-2], ny, MPI_DOUBLE, world_rank+1, 0, MPI_COMM_WORLD);
        MPI_Send(v[nx-2], ny, MPI_DOUBLE, world_rank+1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Send(p[nx-2], ny, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        MPI_Send(u[nx-2], ny, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        MPI_Send(v[nx-2], ny, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);       
    }
}

void 
NSMpi::recv_data()
{
    // receive data from right neighbor
    if(world_rank < world_size-1) {
    // world_size-1 is a king controler
        MPI_Recv(p[nx-1], ny, MPI_DOUBLE, world_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(u[nx-1], ny, MPI_DOUBLE, world_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(v[nx-1], ny, MPI_DOUBLE, world_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        MPI_Recv(p[nx-1], ny, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(u[nx-1], ny, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(v[nx-1], ny, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    // receive data from left neighbor
    if(world_rank > 0) {
        MPI_Recv(p[0], ny, MPI_DOUBLE, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(u[0], ny, MPI_DOUBLE, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(v[0], ny, MPI_DOUBLE, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        MPI_Recv(p[0], ny, MPI_DOUBLE, world_size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(u[0], ny, MPI_DOUBLE, world_size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(v[0], ny, MPI_DOUBLE, world_size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}
