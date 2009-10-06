#include "bct.h"
#include <gsl/gsl_matrix.h>

/*
 * Computes the connection density of the directed graph whose
 * adjaceny matrix is m. The total number of possible connections 
 * is given by the form N(N-1). N is the number of nodes.
 */
double bct::density_dir(const gsl_matrix* m) {
	int N,K;
	double kden;
	int nnz = 0;
	N = m->size1;
	K = bct::nnz(m);
	kden = K/(N*N-N);
}

