all: a2 a2_outer

a2: main.c
	cc -std=c99 -fopenmp -o a2 main.c -lm

a2_outer: main_outer.c
	cc -std=c99 -fopenmp -o a2_otr main_outer.c -lm

clean:
	rm a2 a2_otr
