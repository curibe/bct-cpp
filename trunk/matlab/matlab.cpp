#define GSL_DOUBLE
#include "macros.h"

#include <cmath>
#include <cstring>
#include <ctime>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_vector.h>
#include "matlab.h"

/*
 * This file contains functions for MATLAB emulation that do not depend on the
 * desired floating-point precision.
 */

void matlab::dec2bin(int n, char* bin) {
	if (n <= 0) {
		bin[0] = '0';
		bin[1] = '\0';
		return;
	}
	int strlen = (int)(std::floor(1.0 + std::log(n) / std::log(2)));
	bin[strlen] = '\0';
	for (int i = strlen - 1; i >= 0; i--) {
		int remainder = n % 2;
		if (remainder) {
			bin[i] = '1';
		} else {
			bin[i] = '0';
		}
		n >>= 1;
	}
}

void matlab::dec2bin(int n, int len, char* bin) {
	dec2bin(n, bin);
	int strlen = std::strlen(bin);
	if (len > strlen) {
		for (int i = strlen, j = len; j >= 0; i--, j--) {
			if (i >= 0) {
				bin[j] = bin[i];
			} else {
				bin[j] = '0';
			}
		}
	}
}

gsl_matrix* matlab::inv(const gsl_matrix* m) {
	if (m->size1 != m->size2) {
		return NULL;
	}
	gsl_matrix* LU = copy(m);
	gsl_permutation* p = gsl_permutation_alloc(m->size1);
	int signum;
	gsl_linalg_LU_decomp(LU, p, &signum);
	gsl_matrix* inv_m = NULL;
	if (fp_nonzero(gsl_linalg_LU_det(LU, signum))) {
		inv_m = gsl_matrix_alloc(m->size1, m->size2);
		gsl_linalg_LU_invert(LU, p, inv_m);
	}
	gsl_matrix_free(LU);
	gsl_permutation_free(p);
	return inv_m;
}

/*
 * Generates a permutation of the integers 0 to (size - 1), whereas the MATLAB
 * version uses the integers 1 to size.
 */
gsl_permutation* matlab::randperm(int size) {
	gsl_rng_default_seed = std::time(NULL);
	gsl_rng* rng = gsl_rng_alloc(gsl_rng_default);
	double values[size];
	for (int i = 0; i < size; i++) {
		values[i] = (double)i;
	}
	gsl_ran_shuffle(rng, values, size, sizeof(double));
	gsl_vector_view values_vv = gsl_vector_view_array(values, size);
	gsl_permutation* values_p = to_permutation(&values_vv.vector);
	gsl_rng_free(rng);
	return values_p;
}

/*
 * Emulates (m1 \ m2) = (inv(m1) * m2).
 */
gsl_matrix* matlab::div_left(const gsl_matrix* m1, const gsl_matrix* m2) {
	if (m1->size1 != m1->size2 || m2->size1 != m2->size2 || m1->size1 != m2->size1) {
		return NULL;
	}
	gsl_matrix* inv_m1 = inv(m1);
	gsl_matrix* div_m = mul(inv_m1, m2);
	gsl_matrix_free(inv_m1);
	return div_m;
}

/*
 * Emulates (m1 / m2) = ((inv(m2') * m1')').
 */
gsl_matrix* matlab::div_right(const gsl_matrix* m1, const gsl_matrix* m2) {
	if (m1->size1 != m1->size2 || m2->size1 != m2->size2 || m1->size1 != m2->size1) {
		return NULL;
	}
	gsl_matrix* m2_transpose = gsl_matrix_alloc(m2->size2, m2->size1);
	gsl_matrix_transpose_memcpy(m2_transpose, m2);
	gsl_matrix* inv_m2_transpose = inv(m2_transpose);
	gsl_matrix_free(m2_transpose);
	gsl_matrix* m1_transpose = gsl_matrix_alloc(m1->size2, m1->size1);
	gsl_matrix_transpose_memcpy(m1_transpose, m1);
	gsl_matrix* div_m = mul(inv_m2_transpose, m1_transpose);
	gsl_matrix_free(inv_m2_transpose);
	gsl_matrix_free(m1_transpose);
	gsl_matrix_transpose(div_m);
	return div_m;
}

/*
 * Emulates (m1 * m2).
 */
gsl_matrix* matlab::mul(const gsl_matrix* m1, const gsl_matrix* m2) {
	gsl_matrix* mul_m = gsl_matrix_alloc(m1->size1, m2->size2);
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, m1, m2, 0.0, mul_m);
	return mul_m;
}

/*
 * Emulates (m ^ power).
 */
gsl_matrix* matlab::pow(const gsl_matrix* m, int power) {
	if (m->size1 != m->size2 || power < 1) {
		return NULL;
	}
	gsl_matrix* pow_m = copy(m);
	for (int i = 2; i <= power; i++) {
		gsl_matrix* temp_m = mul(pow_m, m);
		gsl_matrix_free(pow_m);
		pow_m = temp_m;
	}
	return pow_m;
}
