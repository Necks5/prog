#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

enum { WITH_CHOISE_EL, NO_CHOISE_EL };
enum { PRIL1, PRIL2 };
double for_det = 1;
int col_per = 0;
void cool_func(double **mas, int n, int n2, int mode) {
    for_det = 1;
    col_per = 0;
    for(int i = 0; i < n; i++)
    {
        for_det *= mas[i][i];
        for(int j = i; j < n; j++)
        {
            if(mas[j][i] != 0)
            {
                double *temp = mas[i];
                mas[i] = mas[j];
                mas[j] = temp;
                col_per++;
                break;
            }
        }
        if(mode == WITH_CHOISE_EL) {
            int max_ind = i;
            for(int j = i; j < n; j++) {
                if(fabs(mas[i][max_ind]) < fabs(mas[i][j])) {
                    max_ind = j;
                }
            }
            if(max_ind != i) {
                for(int k = 0; k < n; k++) {
                    double temp = mas[k][i];
                    mas[k][i] = mas[k][max_ind];
                    mas[k][max_ind] = temp;
                }
                col_per++;
            }
        }
        double first_coef = mas[i][i];
        for(int j = i; j < n2; j++) {
            mas[i][j] /= first_coef;
        }
        for(int j = i + 1; j < n; j++)
        {
            double a = mas[i][i], b = mas[j][i];
            for(int k = i; k <= n2; k++)
            {
                mas[j][k] = mas[j][k] * a - mas[i][k] * b;
            }
        }
    }
}
double* gaus(double ** mas, int n, int mode) {
    cool_func(mas, n,n,  mode);
    double *ot = malloc(n * sizeof(double));
    for(int i = n - 1; i >= 0; i--)
    {
        for(int j = i + 1; j < n; j++)
        {
            mas[i][n] -= ot[j] * mas[i][j];
        }
        ot[i] = mas[i][n] / mas[i][i];
    }
    return ot;
}
double det(double ** mas, int n) {
    cool_func(mas, n, n, WITH_CHOISE_EL);
    int k = (col_per % 2) ? -1 : 1;
    return for_det * k;
}

double** inv(double ** mas, int n) {
    double ** new_mas = malloc( n * sizeof(double*));
    for(int i = 0; i < n; i++) {
        new_mas[i] = malloc(2 *n * sizeof(double));
    }
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            new_mas[i][j] = mas[i][j];
        }
        for(int j = n; j < 2 * n; j++) {
            new_mas[i][j] = (i == j - n) ? 1 : 0;
        }
    }
    cool_func(new_mas, n, 2*n,  NO_CHOISE_EL);
    for(int i = n-1; i > 0; i--) {
        for(int j = i - 1; j >= 0; j--) {
            double koef = new_mas[j][i];
            if(koef == 0) {
                continue;
            }
            for(int k = i; k < 2*n; k++) {
                new_mas[j][k] -= new_mas[i][k] * koef;
            }
        }
    }
    double **res = malloc(n * sizeof(double*));
    for(int i = 0; i < n; i++) {
        res[i] = malloc(n * sizeof(double));
        for(int j = 0; j < n; j++) {
            res[i][j] = new_mas[i][n + j];
        }
    }
    return res;
}
double norm(double ** mas, int n) {
    double max_sum = 0;
    for(int i = 0; i < n; i++) {
        double temp_sum = 0;
        for(int j = 0; j < n; j++) {
            temp_sum += fabs(mas[i][j]);
        }
        if(temp_sum > max_sum) {
            max_sum = temp_sum;
        }
    }
    return max_sum;
}
double get_el(int i, int j, int n, double x) {
    double q = 1.001 - 0.002;
    if(j == n) {
        50 * exp(x / i) * cos(x);
    }
    if(i == j) {
        return pow(q - 1, i + j);
    } else {
        double temp = pow(q, i + j);
        return temp + 0.1 * (j - i);
    }
}
int main(int argc, char ** argv) 
{
    FILE * read_mode;
    int n;
    double x;
    int flag_create;
    if(argc == 2) {
        read_mode = fopen(argv[1], "r");
        flag_create = PRIL1;
    } else {
        read_mode = stdin;
        flag_create = PRIL2;
    }
    if(flag_create == PRIL1) {
        fscanf(read_mode, "%d", &n);
    } else {
        n = 50;
        printf("Введите х для задания формулой:\n");
        fscanf(read_mode, "%lf", &x);
    }
    double **a = malloc(n * sizeof(double*));
    for(int i = 0; i < n; i++) {
        a[i] = malloc((n+1) * sizeof(double));
    }
    for(int i = 0; i < n; i++) {
        for(int j =0; j <= n; j++) {
            if(flag_create == PRIL1) {
                fscanf(read_mode, "%lf", &a[i][j]);
            } else {
                a[i][j] = get_el(i, j, n, x);
            }
        }
    }
    printf("Решения СЛАУ методом Гаусса:\n");
    double * ot_gaus = gaus(a, n, NO_CHOISE_EL);
    for(int i = 0; i < n; i++) {
        printf("x%d = %f\n", i + 1, ot_gaus[i]);
    }
    printf("Решения СЛАУ методом Гаусса с выбором главного элемента:\n");
    double * ot_gaus2 = gaus(a, n, NO_CHOISE_EL);
    for(int i = 0; i < n; i++) {
        printf("x%d = %f\n", i + 1, ot_gaus2[i]);
    }
    printf("det(A) = %f\n", det(a, n));
    double m = norm(a, n) * norm(inv(a, n), n);
    printf("Число обусловленноси М = %f\n", m);
    return 0;

}