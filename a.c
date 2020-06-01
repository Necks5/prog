#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "umfpack.h"


#define n 1000
#define nnz (4*(n-1) + 5*(n-2)*(n-2))
#define N n*n

double func(double x, double y) {  // function
   // return x*x + y*y;
    return pow((x-y) * (x-y) + 10e-8, 0.75);
}

double f(double x, double y) {   // laplas 
   // return 4;
    double temp = (x-y) * (x-y) + 10e-8;
    double temp1 = 3 / pow(temp, 0.25);
    double temp2= 1.5 * (x-y) * (x-y) / pow(temp, 1.25);
    temp = temp1 - temp2;

    if(temp > 10e5) {
      printf("big\n");
      temp = 10e5;
    }
    return temp;
}

const double h = 1.0 / (n - 1);
//const int nnz = 4*(n-1) + 5*(n-2)*(n-2);

//int Ap[N+1];
//int Ai[nnz];
//double Ax[nnz];
//double b[N], true_x[N], x[N];

//double a[N][N];

int main() {
    int *Ap = malloc(sizeof(int) * (N+1));
    int *Ai = malloc(sizeof(int) * nnz);
    double *Ax = malloc(sizeof(double) * nnz);
    double *b = malloc(sizeof(double) * N);
    double *true_x = malloc(sizeof(double) * N);
    double *x = malloc(sizeof(double) * N);

    Ap[N] = nnz;
    int temp = 0;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            int ind = i*n+j;
            if(i == 0 || i == n-1 || j == 0 || j == n-1) {
                Ap[ind] = temp;
                Ax[temp] = 1;
                Ai[temp] = i*n + j;
                b[ind] = func(i*h, j*h);
                temp++;
            } else {
                Ap[ind] = temp;
                temp += 5;
            }

            true_x[ind] = func(i*h, j*h);

        }
    }
    printf("temp:  %d, nnz: %d\n", temp, nnz);

    for(int i = 2; i < n; i++ ) {
        for(int j = 2; j < n; j++) {
            int ind = n*(i-1)+j-1;
            int start_poz = Ap[ind];

            Ax[start_poz] = 1.0/(h*h);
            Ai[start_poz] = n*(i-2)+j-1;

            Ax[start_poz+1] = 1.0/(h*h);
            Ai[start_poz+1] = n*(i-1)+j - 2;

            Ax[start_poz+2] = -4.0 / (h*h);
            Ai[start_poz+2] = n*(i-1)+j-1;

            Ax[start_poz+3] = 1.0/(h*h);
            Ai[start_poz+3] = n*(i-1)+j;

            Ax[start_poz+4] = 1.0/(h*h);
            Ai[start_poz+4] = n*i + j-1;

            b[ind] = f(i*h, j*h);


        }
    }



    

    double Inform[UMFPACK_INFO], Inform2[UMFPACK_INFO]; 
    double *null = (double *) NULL ;
    int i ;
    void *Symbolic, *Numeric ;
    (void) umfpack_di_symbolic (N, N, Ap, Ai, Ax, &Symbolic, null, null) ;
    (void) umfpack_di_numeric (Ap, Ai, Ax, Symbolic, &Numeric, null, Inform) ;
    printf("n: %d\nInit time: %lf\n", n, Inform[UMFPACK_NUMERIC_TIME]);
    umfpack_di_free_symbolic (&Symbolic) ;
    (void) umfpack_di_solve (UMFPACK_Aat, Ap, Ai, Ax, x, b, Numeric, null, Inform2) ;
    umfpack_di_free_numeric (&Numeric) ;
    printf("Solve time: %f \n", Inform2[UMFPACK_SOLVE_TIME]);
   // for(int i = 0; i < N; i++) {
        // printf("%d \n", Ap[i]);

        // for(int j = 0; j < Ap[i+1] - Ap[i]; j++) {
        //     printf("%f ", Ax[Ap[i] + j]);
        // } 
        // printf("\n");

        // for(int j = 0; j < Ap[i+1] - Ap[i]; j++) {
        //     printf("%d ", Ai[Ap[i] + j]);
        // } 
        // printf("\n");

    //    printf("%lf \t %lf \t %lf \t %d\n", x[i], true_x[i], b[i], Ap[i]);
    //}
    double norm_max = 0;
    for(int i =  0; i < N; i++) {
        double ss = fabs(x[i] - true_x[i]);
	if(ss > norm_max) norm_max = ss;

    }
    printf("error norm:  %lf \n", norm_max);

    free(Ap);
    free(Ai);
    free(Ax);
    free(b);
    free(x);
    free(true_x);
    return 0;
}
