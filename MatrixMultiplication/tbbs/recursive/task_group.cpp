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
#include <emmintrin.h> 

#include "tbb/task.h"
#include "tbb/task_group.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"
#include "util.h"

#define SM (CLS / sizeof (double))
#define N 64
#define block N 
//int block;

void RecMult(int n, matrix a, matrix b, matrix c);
void RecAdd(int n, matrix a, matrix b, matrix c);

int main(int argc, char **argv) {

    struct timeval ts,tf;
    double tt;
    int n, nthreads;
    matrix a, b, c;

    check(argc >= 3, "main: Need matrix size and block size on command line");
    n = atoi(argv[1]);
    //block=atoi(argv[2]);
    nthreads=atoi(argv[3]);

    a = newmatrix(n);
    b = newmatrix(n);
    c = newmatrix(n);
    randomfill(n, a);
    randomfill(n, b);
    printf("before scheduler\n");
    tbb::task_scheduler_init init(nthreads);
    printf("after the scheduler\n");    
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

    printf("mult\n");
    matrix d;

    if (n <= block) {
        double sum, **mul1 = a->d, **mul2 = b->d, **res = c->d;
       // int i, j, k;

        printf("1\n");
        int i, i2, j, j2, k, k2;
        double *__restrict__ rres;
        double *__restrict__ rmul1;
        double *__restrict__ rmul2;
        printf("2\n");
        //for (i = 0; i < N; i += SM)
        for (i = 0; i < N; i++)
            for (j = 0; j < N; j += SM)
                for (k = 0; k < N; k += SM)
                    for (i2 = 0, rres = &res[i][j], rmul1 = &mul1[i][k]; i2 < SM;
                            ++i2, rres += N, rmul1 += N)
                    {
                        _mm_prefetch (&rmul1[8], _MM_HINT_NTA);
                        printf("3\n");
                        for (k2 = 0, rmul2 = &mul2[k][j]; k2 < SM; ++k2, rmul2 += N)
                        {
                            
                            printf("4\n");
                            __m128d m1d = _mm_load_sd (&rmul1[k2]);
                            m1d = _mm_unpacklo_pd (m1d, m1d);
                            for (j2 = 0; j2 < SM; j2 += 2)
                            {
                                printf("5\n");
                                __m128d m2 = _mm_load_pd (&rmul2[j2]);
                                __m128d r2 = _mm_load_pd (&rres[j2]);
                                _mm_store_pd (&rres[j2],_mm_add_pd (_mm_mul_pd (m2, m1d), r2));
                                /*for (i = 0; i < n; i++) {
                                  for (j = 0; j < n; j++) {
                                  for (sum = 0., k = 0; k < n; k++)
                                  sum += p[i][k] * q[k][j];
                                  r[i][j] = sum;
                                  }
                                  }*/
                            }
                        } 
                    }
        printf("end 1\n");
    } else {
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
        printf("in rect mul before wait\n"); 
        g.wait();
        printf("in rect mul wait before add\n"); 
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
    printf("read add\n");
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
