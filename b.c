#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "umfpack.h"
//int n = 5 ;
//int Ap [ ] = {0, 2, 5, 9, 10, 12} ;
//int Ai [ ] = { 0, 1, 0, 2, 4, 1, 2, 3, 4, 2, 1, 4} ;
//double Ax [ ] = {2., 3., 3., -1., 4., 4., -3., 1., 2., 2., 6., 1.} ;
//double b [ ] = {8., 45., -3., 3., 19.} ;
//double x [5] ;


int main (int argc, char * argv[])
{
    double Inform[UMFPACK_INFO], Inform2[UMFPACK_INFO]; 
    time_t t2, t1 = time(NULL);
    if(argc != 2) {
        printf("Input argument\n");
	return 1;
    }
    FILE * f = fopen(argv[1],"r");
    if(f== NULL) {
	printf("file not open\n");
    }
    int n;
    fscanf(f, "%d", &n);
    int * Ap = malloc(sizeof(int) * (n+1));
    for(int i = 0; i <= n; i++) {
	fscanf(f, "%d", &Ap[i]);
	Ap[i]--;
    }
    int nnz = Ap[n] - Ap[0];
    int * Ai = malloc(sizeof(int) * nnz);
    for(int i = 0; i < nnz; i++) {
	fscanf(f, "%d", &Ai[i]);
	Ai[i]--;
    }
    
    double * Ax = malloc(sizeof(double) * nnz);
    for(int i = 0; i < nnz; i++) {
	fscanf(f, "%lf", &Ax[i]);
    }
    double *  b = malloc(sizeof(double) * n);
    for(int i = 0; i < n; i++) {
	b[i] = sin((double)i);
    }
    double * x = malloc(sizeof(double) * n);

    t2 = time(NULL);
  //  printf("n: %d\nInit time: %f\n", n, difftime(t2,t1));
    t1=t2;



    double *null = (double *) NULL ;
    int i ;
    void *Symbolic, *Numeric ;
    (void) umfpack_di_symbolic (n, n, Ap, Ai, Ax, &Symbolic, null, null) ;
    (void) umfpack_di_numeric (Ap, Ai, Ax, Symbolic, &Numeric, null, Inform) ;
    printf("n: %d\nInit time: %lf\n", n, Inform[UMFPACK_NUMERIC_TIME]);
    umfpack_di_free_symbolic (&Symbolic) ;
    (void) umfpack_di_solve (UMFPACK_A, Ap, Ai, Ax, x, b, Numeric, null, Inform2) ;
    umfpack_di_free_numeric (&Numeric) ;

    t2 = time(NULL);
    printf("Solve time: %f\n",Inform2[UMFPACK_SOLVE_TIME]);
   // for (i = 0 ; i < n ; i++) printf ("x [%d] = %g\n", i, x [i]) ;
    return (0) ;
}
