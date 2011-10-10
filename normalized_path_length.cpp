#include "bct.h"
#include <cmath>
#include <gsl/gsl_matrix.h>

/*
 * Given a distance matrix, computes the normalized shortest path length.
 */
double bct::normalized_path_length(const gsl_matrix* D, double wmax) {
	int N = D->size1;
	double dmin = 1.0 / wmax;
	double dmax = (double)N / wmax;
	double sum = 0.0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (i == j) {
				continue;
			}
			double d = gsl_matrix_get(D, i, j);
			sum += (d < dmax) ? d : dmax;
		}
	}
	return std::abs(((sum / (double)(N * (N - 1))) - dmin) / (dmax - dmin));
}

/**
 * Computes the normalized shortest path length using dmax = N * lmean, where
 * lmean is the average distance between all directly connected nodes.
 */
double bct::normalized_path_length_m(const gsl_matrix* G, double wmax) {
	int N = G->size1;
	gsl_matrix* L = invert_elements(G);
	int nonzeros = 0;
	double lmean = 0.0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (i == j) {
				continue;
			}
			double l = gsl_matrix_get(L, i, j);
			if (fp_nonzero(l)) {
				nonzeros++;
				lmean += l;
			}
		}
	}
	lmean /= nonzeros;
	gsl_matrix* D = distance_wei(L);
	gsl_matrix_free(L);
	double dmin = 1.0 / wmax;
	double dmax = (double)N * lmean;
	double sum = 0.0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (i == j) {
				continue;
			}
			double d = gsl_matrix_get(D, i, j);
			sum += (d < dmax) ? d : dmax;
		}
	}
	gsl_matrix_free(D);
	return std::abs(((sum / (double)(N * (N - 1))) - dmin) / (dmax - dmin));
}
