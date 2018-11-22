EXECS = nsmpi
GXX = g++
MPICXX ?= mpicxx

all: ${EXECS}

NavierStokes.o: NavierStokes.cpp
	${GXX} -c NavierStokes.cpp

NSMpi.o: NSMpi.cpp
	${MPICXX} -c NSMpi.cpp

${EXECS}: NavierStokes.o NSMpi.o testNSMpi.cpp
	${MPICXX} -o ${EXECS} testNSMpi.cpp NSMpi.o NavierStokes.o

run: ${EXECS}
	mpirun -n 1 ./${EXECS} 1
	mpirun -n 2 ./${EXECS} 2
	mpirun -n 4 ./${EXECS} 4
	mpirun -n 8 ./${EXECS} 8
	mpirun -n 16 ./${EXECS} 16
	mpirun -n 32 ./${EXECS} 32
	mpirun -n 64 ./${EXECS} 64 
	mpirun -n 128 ./${EXECS} 128 

clean:
	rm -f ${EXECS} *.o *.out
