* MPI

** This is the MPI parallel programming for OpenCV image processing  

function of 3 types of image processing  
1. average blur  
2. negative  
3. rotate left 90 degree

Example call:  

	make
	make irun

or you can type in cmd with mpi command

compile:

	mpic++ -std=c++03 FILENAME.cpp -o FILENAME

and run with mpirun command

run:

	mpirun -np 3 -host master,slave01,slave02 FILENAME

The default FILENAME is "test"  