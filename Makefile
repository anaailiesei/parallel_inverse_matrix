CXX = g++
FLAGS = -Werror -Wall -I. -std=c++17

TARGETS = MPI/mpi sequential/secvential omp/omp PTHREADS/pthreads

.PHONY: all clean format

all: $(TARGETS)

helpers.o: helpers.cpp
	$(CXX) $(FLAGS) -c $^

sequential/secvential: sequential/secvential.cpp helpers.cpp
	$(CXX) $(FLAGS) $^ -o $@ -lstdc++fs

omp/omp: omp/omp.cpp helpers.cpp
	$(CXX) $(FLAGS) -fopenmp $^ -o $@ -lstdc++fs

PTHREADS/pthreads: PTHREADS/pthreads.cpp helpers.cpp
	$(CXX) $(FLAGS) -pthread $^ -o $@ -lstdc++fs

MPI/mpi: MPI/mpi.cpp helpers.cpp
	mpicxx $(FLAGS) $^ -o $@ -lstdc++fs

format:
	clang-format -i *.cpp

clean:
	rm -f $(TARGETS) *.o
