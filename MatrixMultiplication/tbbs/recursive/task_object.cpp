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


#include <tbb/task.h>
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/tick_count.h>
#include "util.h"


int block;


class MulTask: public tbb::task {
   
    public: 
        int n;
        matrix a, b, c;

        MulTask(int n_, matrix a_, matrix b_, matrix c_) {
            n = n_;
            a = a_;
            b = b_;
            c = c_;
        }

        task* execute() {
            
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
                d = newmatrix(n);
                n /= 2;
                
                MulTask& t1 = *new( tbb::task::allocate_child() ) MulTask(n, a11, b11, d11);
                MulTask& t2 = *new (allocate_child()) MulTask(n, a12, b21, c11);
                MulTask& t3 = *new (allocate_child()) MulTask(n, a11, b12, d12);
                MulTask& t4 = *new (allocate_child()) MulTask(n, a12, b22, c12);
                MulTask& t5 = *new (allocate_child()) MulTask(n, a21, b11, d21);
                MulTask& t6= *new (allocate_child()) MulTask(n, a22, b21, c21); 
                MulTask& t7 = *new (allocate_child()) MulTask(n, a21, b12, d22);
                MulTask& t8 = *new (allocate_child()) MulTask(n, a22, b22, c22);
                
                set_ref_count(9);

                tbb::task::spawn(t1);
                tbb::task::spawn(t2);
                tbb::task::spawn(t3);
                tbb::task::spawn(t4);
                tbb::task::spawn(t5);
                tbb::task::spawn(t6);
                tbb::task::spawn(t7);
                tbb::task::spawn(t8);
                
                tbb::task::wait_for_all();
            }
            return NULL;
        }
};

class AddTask: public tbb::task {

};

int main(int argc, char **argv) {

    struct timeval ts,tf;
    double tt;
    int n, nthreads;
    matrix a, b, c;

    check(argc >= 3, "main: Need matrix size, block size and thread no on command line");
    n = atoi(argv[1]);
    block=atoi(argv[2]);
    nthreads=atoi(argv[3]);

    a = newmatrix(n);
    b = newmatrix(n);
    c = newmatrix(n);
    randomfill(n, a);
    randomfill(n, b);

    gettimeofday(&ts,NULL);
    
    tbb::task_scheduler_init init(nthreads);
    
    MulTask &r = *new(tbb::task::allocate_root()) MulTask(n, a, b, c);
    tbb::task::spawn_root_and_wait(r);
    
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
