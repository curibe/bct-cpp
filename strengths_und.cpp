#include "bct.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

/*
 * Computes strength for an undirected graph.
 */
gsl_vector* bct::strengths_und(const gsl_matrix* CIJ) {
	if (safe_mode) check_status(CIJ, UNDIRECTED, "strengths_und");
	if (CIJ->size1 != CIJ->size2) throw size_exception();
	
	// str = sum(CIJ);
	return sum(CIJ);
}
