#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include "matlab.h"

/*
 * Our indexing-with-assignment functions do not automatically resize vectors
 * and matrices as MATLAB's do.  Thus, while the MATLAB code
 * (A = [1 2 3]; A([4 5]) = 5) yields (A = [1 2 3 5 5]), the equivalent C++ code
 * would result in an error.  In general, it is the caller's responsibility to
 * ensure that a call to one of these functions does not attempt to index past
 * the end of a vector or matrix.
 */

// Vector-by-vector indexing

gsl_vector* matlab::ordinal_index(const gsl_vector* v, const gsl_vector* indices) {
	gsl_vector* index_v = gsl_vector_alloc(indices->size);
	for (int i = 0; i < indices->size; i++) {
		int index = (int)gsl_vector_get(indices, i);
		double value = gsl_vector_get(v, index);
		gsl_vector_set(index_v, i, value);
	}
	return index_v;
}

void matlab::ordinal_index_assign(gsl_vector* v, const gsl_vector* indices, double value) {
	for (int i = 0; i < indices->size; i++) {
		int index = (int)gsl_vector_get(indices, i);
		gsl_vector_set(v, index, value);
	}
}

void matlab::ordinal_index_assign(gsl_vector* v, const gsl_vector* indices, const gsl_vector* values) {
	for (int i = 0; i < indices->size; i++) {
		int index = (int)gsl_vector_get(indices, i);
		double value = gsl_vector_get(values, i);
		gsl_vector_set(v, index, value);
	}
}

gsl_vector* matlab::logical_index(const gsl_vector* v, const gsl_vector* logical_v) {
	int n_indexed = nnz(logical_v);
	if (n_indexed == 0) {
		return NULL;
	}
	gsl_vector* index_v = gsl_vector_alloc(n_indexed);
	for (int i = 0, index = 0; i < logical_v->size; i++) {
		if (fp_nonzero(gsl_vector_get(logical_v, i))) {
			double value = gsl_vector_get(v, i);
			gsl_vector_set(index_v, index++, value);
		}
	}
	return index_v;
}

void matlab::logical_index_assign(gsl_vector* v, const gsl_vector* logical_v, double value) {
	for (int i = 0; i < logical_v->size; i++) {
		if (fp_nonzero(gsl_vector_get(logical_v, i))) {
			gsl_vector_set(v, i, value);
		}
	}
}

void matlab::logical_index_assign(gsl_vector* v, const gsl_vector* logical_v, const gsl_vector* values) {
	for (int i = 0, index = 0; i < logical_v->size; i++) {
		if (fp_nonzero(gsl_vector_get(logical_v, i))) {
			double value = gsl_vector_get(values, index++);
			gsl_vector_set(v, i, value);
		}
	}
}

// Matrix-by-integer indexing

double matlab::ordinal_index(const gsl_matrix* m, int index) {
	int row = index % (int)m->size1;
	int column = index / (int)m->size1;
	return gsl_matrix_get(m, row, column);
}

void matlab::ordinal_index_assign(gsl_matrix* m, int index, double value) {
	int row = index % (int)m->size1;
	int column = index / (int)m->size1;
	gsl_matrix_set(m, row, column, value);
}

// Matrix-by-vector indexing

gsl_vector* matlab::ordinal_index(const gsl_matrix* m, const gsl_vector* indices) {
	gsl_vector* index_v = gsl_vector_alloc(indices->size);
	for (int i = 0; i < indices->size; i++) {
		int index = (int)gsl_vector_get(indices, i);
		double value = ordinal_index(m, index);
		gsl_vector_set(index_v, i, value);
	}
	return index_v;
}

void matlab::ordinal_index_assign(gsl_matrix* m, const gsl_vector* indices, double value) {
	for (int i = 0; i < indices->size; i++) {
		int index = (int)gsl_vector_get(indices, i);
		ordinal_index_assign(m, index, value);
	}
}

void matlab::ordinal_index_assign(gsl_matrix* m, const gsl_vector* indices, const gsl_vector* values) {
	for (int i = 0; i < indices->size; i++) {
		int index = (int)gsl_vector_get(indices, i);
		double value = gsl_vector_get(values, i);
		ordinal_index_assign(m, index, value);
	}
}

gsl_vector* matlab::logical_index(const gsl_matrix* m, const gsl_vector* logical_v) {
	int n_indexed = nnz(logical_v);
	if (n_indexed == 0) {
		return NULL;
	}
	gsl_vector* index_v = gsl_vector_alloc(n_indexed);
	for (int i = 0, index = 0; i < logical_v->size; i++) {
		if (fp_nonzero(gsl_vector_get(logical_v, i))) {
			double value = ordinal_index(m, i);
			gsl_vector_set(index_v, index++, value);
		}
	}
	return index_v;
}

void matlab::logical_index_assign(gsl_matrix* m, const gsl_vector* logical_v, double value) {
	for (int i = 0; i < logical_v->size; i++) {
		if (fp_nonzero(gsl_vector_get(logical_v, i))) {
			ordinal_index_assign(m, i, value);
		}
	}
}

void matlab::logical_index_assign(gsl_matrix* m, const gsl_vector* logical_v, const gsl_vector* values) {
	for (int i = 0, index = 0; i < logical_v->size; i++) {
		if (fp_nonzero(gsl_vector_get(logical_v, i))) {
			double value = gsl_vector_get(values, index++);
			ordinal_index_assign(m, i, value);
		}
	}
}

// Matrix-by-two-vectors indexing (non-mixed)

gsl_matrix* matlab::ordinal_index(const gsl_matrix* m, const gsl_vector* rows, const gsl_vector* columns) {
	gsl_matrix* indexed_m = gsl_matrix_alloc(rows->size, columns->size);
	for (int i = 0; i < rows->size; i++) {
		int row = (int)gsl_vector_get(rows, i);
		for (int j = 0; j < columns->size; j++) {
			int column = (int)gsl_vector_get(columns, j);
			double value = gsl_matrix_get(m, row, column);
			gsl_matrix_set(indexed_m, i, j, value);
		}
	}
	return indexed_m;
}

void matlab::ordinal_index_assign(gsl_matrix* m, const gsl_vector* rows, const gsl_vector* columns, double value) {
	for (int i = 0; i < rows->size; i++) {
		int row = (int)gsl_vector_get(rows, i);
		for (int j = 0; j < columns->size; j++) {
			int column = (int)gsl_vector_get(columns, j);
			gsl_matrix_set(m, row, column, value);
		}
	}
}

void matlab::ordinal_index_assign(gsl_matrix* m, const gsl_vector* rows, const gsl_vector* columns, const gsl_matrix* values) {
	for (int i = 0; i < rows->size; i++) {
		int row = (int)gsl_vector_get(rows, i);
		for (int j = 0; j < columns->size; j++) {
			int column = (int)gsl_vector_get(columns, j);
			double value = gsl_matrix_get(values, i, j);
			gsl_matrix_set(m, row, column, value);
		}
	}
}

/*
 * Emulates matrix indexing by another matrix.
 */
gsl_matrix* matlab::index(const gsl_matrix* m, const gsl_matrix* indices) {
	gsl_matrix* indexed = gsl_matrix_alloc(indices->size1, indices->size2);
	for (int i = 0; i < indices->size1; i++) {
		for (int j = 0; j < indices->size2; j++) {
			double value = ordinal_index(m, gsl_matrix_get(indices, i, j));
			gsl_matrix_set(indexed, i, j, value);
		}
	}
	return indexed;
}

/*
 * Emulates matrix indexing and assignment (m(indices) = x).
 */
void matlab::index_assign(gsl_matrix* m, const gsl_matrix* indices, double x) {
	for (int i = 0; i < indices->size1; i++) {
		for (int j = 0; j < indices->size2; j++) {
			int index = (int)gsl_matrix_get(indices, i, j);
			ordinal_index_assign(m, index, x);
		}
	}
}

/*
 * Emulates m(r,c) where r is a vector of actual row indices and c is a vector of 1's and 0's
 */
gsl_matrix* matlab::mixed_logical_index(const gsl_matrix* m, const gsl_vector* rows, const gsl_vector* logical_cols) {
	int columns = nnz(logical_cols);
	if (columns == 0 || rows == NULL || m == NULL) {
		return NULL;
	}
	gsl_matrix* indexed = gsl_matrix_alloc(rows->size, columns);
	int column = 0;
	for (int j = 0; j < logical_cols->size; j++) {
		if (fp_nonzero(gsl_vector_get(logical_cols, j))) {
			for (int i = 0; i < rows->size; i++) {
				int row = (int)gsl_vector_get(rows, i);
				double value = gsl_matrix_get(m, row, j);
				gsl_matrix_set(indexed, i, column, value);
			}
			column += 1;
		}
	}
	return indexed;
}

/*
 * Emulates matrix logical indexing by another matrix.
 */
gsl_vector* matlab::logical_index(const gsl_matrix* m, const gsl_matrix* lm) {
	int size = nnz(lm);
	int m_size = (int)m->size1 * (int)m->size2;
	int lm_size = (int)lm->size1 * (int)lm->size2;
	if (size == 0 || m_size < lm_size) {
		return NULL;
	}
	gsl_vector* indexed = gsl_vector_alloc(size);
	int position = 0;
	for (int j = 0; j < lm->size2; j++) {
		for (int i = 0; i < lm->size1; i++) {
			if (fp_nonzero(gsl_matrix_get(lm, i, j))) {
				double value = ordinal_index(m, j * lm->size1 + i);
				gsl_vector_set(indexed, position, value);
				position++;
			}
		}
	}
	return indexed;
}

/*
 * Emulates matrix logical indexing and assignment (m(lm) = x).
 */
void matlab::logical_index_assign(gsl_matrix* m, const gsl_matrix* lm, double x) {
	for (int j = 0; j < lm->size2; j++) {
		for (int i = 0; i < lm->size1; i++) {
			if (fp_nonzero(gsl_matrix_get(lm, i, j))) {
				int index = j * lm->size1 + i;
				ordinal_index_assign(m, index, x);
			}
		}
	}
}
