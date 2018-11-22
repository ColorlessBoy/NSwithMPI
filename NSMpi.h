#ifndef _NSMPI_H
#define _NSMPI_H
#include "NavierStokes.h"
#include <mpi.h>

class NSMpi: public NavierStokes {
public:
    NSMpi(int world_rank, int world_size, int nx, int ny);
    void exchange_message();
    void gather_errors(double *errors);

private:
    int world_rank, world_size;
    void send_data();
    void recv_data();
};

#endif