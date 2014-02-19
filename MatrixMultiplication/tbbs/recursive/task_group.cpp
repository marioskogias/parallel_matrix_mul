/*
 * mm_recursive.c
 *
 * Routines to realize the recursive matrix multiplication.
 * RecMult multiplies n by n matrices a and b, putting the result in c, and
 * using the matrix d as scratch space.  The recursive algorithm is: 
 *
 *      d11 = a11 * b11
 *      c11 = a12 * b21
 *      d12 = a11 * b12
 *      c12 = a12 * b22
 *      d21 = a21 * b11
 *      c21 = a22 * b21
 *      d22 = a21 * b12
 *      c22 = a22 * b22
 *      c11 = c11 + d11
 *      c12 = c12 + d12
 *      c21 = c21 + d21
 *      c22 = c22 + d22
 *
 * where the double indices refer to submatrices in an obvious way.
 * Each line of RecMult() that recursively calls itself computes one
 * of the submatrices.  Four scratch half-size matrices are required by the
 * sequence of computations here.
 *
 * The small matrix computations (i.e., for n <= block) can be
 * optimized considerably from those given here; in particular, this
 * is important to do before the value of block is chosen optimally. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "tbb/task.h"
#include "tbb/task_group.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"
#include "util.h"


int block;

void RecMult(int n, matrix a, matrix b, matrix c);
void RecAdd(int n, matrix a, matrix b, matrix c);

int main(int argc, char **argv) {

    struct timeval ts,tf;
    double tt;
    int n;
    matrix a, b, c;

    check(argc >= 3, "main: Need matrix size and block size on command line");
    n = atoi(argv[1]);
    block=atoi(argv[2]);

    a = newmatrix(n);
    b = newmatrix(n);
    c = newmatrix(n);
    randomfill(n, a);
    randomfill(n, b);

    gettimeofday(&ts,NULL);
    RecMult(n, a, b, c);	/* strassen algorithm */
    gettimeofday(&tf,NULL);
    tt=(tf.tv_sec-ts.tv_sec)+(tf.tv_usec-ts.tv_usec)*0.000001;

    printf("Recursive Size %d Block %d Time %lf\n",n,block,tt);

    char *filename=(char *)malloc(30*sizeof(char));
    sprintf(filename,"res_mm_recursive_%d",n);
    FILE * f=fopen(filename,"w");
    print(n,c,f);
    fclose(f);

    freematrix(a,n);
    freematrix(b,n);
    freematrix(c,n);

    return 0;
}

/* c = a*b */
void RecMult(int n, matrix a, matrix b, matrix c)
{

    matrix d;

    if (n <= block) {
        double sum, **p = a->d, **q = b->d, **r = c->d;
        int i, j, k;

        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                for (sum = 0., k = 0; k < n; k++)
                    sum += p[i][k] * q[k][j];
                r[i][j] = sum;
            }
        }
    } 
    else {
        d=newmatrix(n);
        n /= 2;
        tbb::task_group g;

        /*First do all the mutliplications*/
        g.run( [&]{ RecMult(n, a11, b11, d11); });
        g.run( [&]{ RecMult(n, a12, b21, c11); });
        g.run( [&]{ RecMult(n, a11, b12, d12); });
        g.run( [&]{ RecMult(n, a12, b22, c12); });
        g.run( [&]{ RecMult(n, a21, b11, d21); });
        g.run( [&]{ RecMult(n, a22, b21, c21); }); 
        g.run( [&]{ RecMult(n, a21, b12, d22); });
        g.run( [&]{ RecMult(n, a22, b22, c22); });
        g.wait();
    
        /*Then all the additions*/
        g.run( [&]{ RecAdd(n, d11, c11, c11); });
        g.run( [&]{ RecAdd(n, d12, c12, c12); });
        g.run( [&]{ RecAdd(n, d21, c21, c21); });
        g.run( [&]{ RecAdd(n, d22, c22, c22); });
        g.wait();
        
        freematrix(d,n*2);
    }
}

/* c = a+b */
void RecAdd(int n, matrix a, matrix b, matrix c) {
    if (n <= block) {
        double **p = a->d, **q = b->d, **r = c->d;
        int i, j;
        for (i = 0; i < n; i++) 
            for (j = 0; j < n; j++) 
                r[i][j] = p[i][j] + q[i][j];
    } 
    else {
        n /= 2;
        tbb::task_group g;
        g.run( [&]{ RecAdd(n, a11, b11, c11); });
        g.run( [&]{ RecAdd(n, a12, b12, c12); });
        g.run( [&]{ RecAdd(n, a21, b21, c21); });
        g.run( [&]{ RecAdd(n, a22, b22, c22); });
        g.wait();
    }
}
