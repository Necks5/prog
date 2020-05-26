#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define n 4
#define nnz (4*(n-1) + 5*(n-2)*(n-2))
#define N n*n

double func(double x, double y) {  // function
    return pow((x-y) * (x-y) + 10e-5, 0.75);
}

double f(double x, double y) {   // laplas 
    double temp = (x-y) * (x-y) + 10e-5;
    double temp1 = 3 / pow(temp, 0.25);
    double temp2= 1.5 * (x-y) * (x-y) / pow(temp, 1.25);
    temp = temp1 - temp2;

    if(temp > 10e5) {
        temp = 10e5;
    }
    return temp;
}

const double h = 1.0 / (n - 1);
//const int nnz = 4*(n-1) + 5*(n-2)*(n-2);

int Ap[N+1];
int Ai[nnz];
double Ax[nnz];
double b[N], true_x[N], x[N];

double a[N][N];

int main() {
    
    for(int j = 0; j < n; j++) 
        b[j] = func(0, j*h)
        a[j,j] = 1
        b[n*(n-1) + j] = func(1, j*h)
        a[n*(n-1) + j,n*(n-1) + j] = 1
        
    for(int i = 2; i < n; i++) 
        b[n*(i-1)] = func(h*(i-1), 0)
        a[n*(i-1),n*(i-1)] = 1
        b[n*(i-1) + n-1] = func(h*(i-1), 1)
        a[n*(i-1) + n-1,n*(i-1) + n-1] = 1

        
    for(int i = 2; i < n; i++) 
         for j in range(2, n):  #   for(int j = 1; j < n-1; j++) 
            a[n*(i-1)+j-1,n*(i-1)+j-1] = -4.0 / (h*h)
            a[n*(i-1)+j-1,n*i + j-1] = 1.0 / (h*h)
            a[n*(i-1)+j-1,n*(i-2)+j-1] = 1.0/(h*h)
            a[n*(i-1)+j-1,n*(i-1)+j + 1 - 1] = 1.0/(h*h)
            a[n*(i-1)+j-1,n*(i-1)+j - 1 -1] = 1.0/(h*h)
            
            b[n*(i-1)+j-1] = f(h*i, h*j)

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

    for(int i = 2; i < n; i++ ){
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



    for(int i = 0; i < N; i++) {
        // printf("%d \n", Ap[i]);

        // for(int j = 0; j < Ap[i+1] - Ap[i]; j++) {
        //     printf("%f ", Ax[Ap[i] + j]);
        // } 
        // printf("\n");

        // for(int j = 0; j < Ap[i+1] - Ap[i]; j++) {
        //     printf("%d ", Ai[Ap[i] + j]);
        // } 
        // printf("\n");

        printf("%lf \t %lf \n", b[i], true_x[i]);
    }



    return 0;
}