Benjamin A. Slack
benjamin.a.slack@wmich.edu
CS6260
Assignment #2
Solving linear systems with
Back Substitution.
03.14.2018

Description:
Implements a parallel solution to 
solving systems of equations using
a triangular matrix and back sub-
stitution. Two different parallelizations
of the triangulate algorithm are 
implemented, a inner loop and middle 
loop parallelization. Outputs timing,
information for triangulation and 
back substitution.

Usage:

inner loop implementation:

a2 [-v] [-n] N-DIMENSION [-t] THREADS [-s] | [-d] | [-g] CHUNKSIZE

middle loop implementation:

a2_otr [-v] [-n] N-DIMENSION [-t] THREADS [-s] | [-d] | [-g] CHUNKSIZE

Options:

-v sets verbose mode, prints the contents of the generate matrix, 
   the triangulation, and finally the back substitution solution.

-n sets the dimension of the matrix

-t sets the number of threads

-s sets omp schedule to static with a given chunksize

-d sets omp schedule to dynamic with a given chunksize

-g sets omp schedule to guided with a given chunksize
