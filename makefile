.DEFAULT_GOAL := default

run: default hostfile.txt
	mpirun -n 4 ./clock
default: clock.cpp
	mpic++ clock.cpp -o clock