all:
	gcc -c -Wall lib/gen_alg/gen_alg.c lib/simulation/simulation.c main.c
	gcc -Wall gen_alg.o simulation.o main.o -o main
	rm -rf *.o