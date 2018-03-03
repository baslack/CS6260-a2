all: a2

a2: main.c
	cc -std=c99 -fopenmp -o a2 main.c

clean:
	rm a2