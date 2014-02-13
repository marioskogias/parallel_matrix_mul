#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "util.h"

extern int block;
/* Recursive randomfill -- possibly useful for NUMA archs*/
/* Fill the matrix a with random values between 0 and 1 */
/*
   void randomfill(int n, matrix a) {
   if (n <= block) {
   int i, j;
   double **p = a->d, T = -(double)(1 << 31);

   for (i = 0; i < n; i++)
   for (j = 0; j < n; j++)
   p[i][j] = random() / T;
   } 
   else {
   n /= 2;
   randomfill(n, a11);
   randomfill(n, a12);
   randomfill(n, a21);
   randomfill(n, a22);
   }
   }
   */

/* fill n by n matrix with random numbers */
void randomfill(int n, matrix a) {
    int i,j;
    double T = -(double)(1 << 31);

    if (n <= block) {
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                a->d[i][j] = rand() / T;
    } 		
    else {
        for (i=0;i<n;i++)
            for (j=0;j<n;j++) 
                auxrandomfill(n,a,i,j);
    }
}

void auxrandomfill(int n, matrix a, int i, int j) {

    double T = -(double)(1 << 31);
    if (n<=block) 
        a->d[i][j]=rand()/T;
    else 
        auxrandomfill(n/2,a->p[(i>=n/2)*2+(j>=n/2)],i%(n/2),j%(n/2));
}

/* return new square n by n matrix */
matrix newmatrix(int n) {

    matrix a;
    a = (matrix)malloc(sizeof(*a));
    check(a != NULL, "newmatrix: out of space for matrix");
    if (n <= block) {
        int i;
        a->d = (double **)calloc(n, sizeof(double *));
        check(a->d != NULL,
                "newmatrix: out of space for row pointers");
        for (i = 0; i < n; i++) {
            a->d[i] = (double *)calloc(n, sizeof(double));
            check(a != NULL, "newmatrix: out of space for rows");
        }
    } 
    else {
        n /= 2;
        a->p = (matrix *)calloc(4, sizeof(matrix));
        check(a->p != NULL,"newmatrix: out of space for submatrices");
        a11 = newmatrix(n);
        a12 = newmatrix(n);
        a21 = newmatrix(n);
        a22 = newmatrix(n);
    }
    return a;
}

/* free square n by n matrix m */
void freematrix (matrix m, int n) {
    if (n<=block) {
        int i;
        for (i=0;i<n;i++)
            free(m->d[i]);
        free(m->d);
    }
    else {
        n/=2;	
        freematrix(m->p[0],n);
        freematrix(m->p[1],n);
        freematrix(m->p[2],n);
        freematrix(m->p[3],n);
    }
}

/* print n by n matrix into file f*/
void print(int n, matrix a, FILE * f) {
    int i,j;

    if (n <= block) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++)
                fprintf(f,"%lf ",a->d[i][j]);
            fprintf(f,"\n");	
        }
    } 		
    else {
        for (i=0;i<n;i++) {
            for (j=0;j<n;j++) 
                auxprint(n,a,f,i,j);
            fprintf(f,"\n");
        }
    }
}

void auxprint(int n, matrix a, FILE * f, int i, int j) {

    if (n<=block) 
        fprintf(f,"%lf ",a->d[i][j]);
    else 
        auxprint(n/2,a->p[(i>=n/2)*2+(j>=n/2)],f,i%(n/2),j%(n/2));
}

/*
 * If the expression e is false print the error message s and quit. 
 */

void check(int e, char *s)
{
    if (!e) {
        fprintf(stderr, "Fatal error -> %s\n", s);
        exit(1);
    }
}
