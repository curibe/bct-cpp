#include "bct.h"
#include <cmath>
#include <ctime>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_vector.h>

/*
 * Returns a latticized graph with equivalent degree sequence to the original
 * weighted undirected graph.  On average, each edge is rewired ITER time.
 * Strength distributions are not preserved for weighted graphs.
 */
gsl_matrix* bct::latmio_und(const gsl_matrix* R, int ITER) {
	if (safe_mode) check_status(R, UNDIRECTED, "latmio_und");
	if (R->size1 != R->size2) throw size_exception();
	
	gsl_rng_default_seed = std::time(NULL);
	gsl_rng* rng = gsl_rng_alloc(gsl_rng_default);
	
	// n=length(R);
	int n = length(R);
	
	// D=zeros(n);
	gsl_matrix* D = zeros_double(n);
	
	// u=[0 min([mod(1:n-1,n);mod(n-1:-1:1,n)])];
	gsl_vector* seq1 = sequence_double(1, n - 1);
	gsl_vector* seq2 = sequence_double(n - 1, -1, 1);
	gsl_matrix* seq1_seq2 = concatenate_columns(seq1, seq2);
	gsl_vector_free(seq1);
	gsl_vector_free(seq2);
	gsl_vector* min_seq1_seq2 = min(seq1_seq2);
	gsl_matrix_free(seq1_seq2);
	gsl_vector* u = concatenate(0.0, min_seq1_seq2);
	gsl_vector_free(min_seq1_seq2);
	
	// for v=1:ceil(n/2)
	for (int v = 1; v <= (int)std::ceil((double)n / 2.0); v++) {
		
		// D(n-v+1,:)=u([v+1:n 1:v]);
		gsl_vector* u_indices1 = sequence_double(v, n - 1);
		gsl_vector* u_indices2 = sequence_double(0, v - 1);
		gsl_vector* u_indices = concatenate(u_indices1, u_indices2);
		gsl_vector_free(u_indices1);
		gsl_vector_free(u_indices2);
		gsl_vector* u_idx = ordinal_index(u, u_indices);
		gsl_vector_free(u_indices);
		gsl_matrix_set_row(D, n - v, u_idx);
		gsl_vector_free(u_idx);
		
		// D(v,:)=D(n-v+1,n:-1:1);
		gsl_vector* D_rows = gsl_vector_alloc(1);
		gsl_vector_set(D_rows, 0, (double)(n - v));
		gsl_vector* D_cols = sequence_double(n - 1, -1, 0);
		gsl_matrix* D_idx = ordinal_index(D, D_rows, D_cols);
		gsl_vector_free(D_rows);
		gsl_vector_free(D_cols);
		gsl_vector* D_idx_v = to_vector(D_idx);
		gsl_matrix_free(D_idx);
		gsl_matrix_set_row(D, v - 1, D_idx_v);
		gsl_vector_free(D_idx_v);
	}
	
	// [i j]=find(tril(R));
	gsl_matrix* tril_R = tril(R);
	gsl_matrix* find_tril_R = find_ij(tril_R);
	gsl_matrix_free(tril_R);
	gsl_vector_view i = gsl_matrix_column(find_tril_R, 0);
	gsl_vector_view j = gsl_matrix_column(find_tril_R, 1);
	
	// K=length(i);
	int K = length(&i.vector);
	
	// ITER=K*ITER;
	ITER = K * ITER;
	
	gsl_matrix* _R = copy(R);
	
	// for iter=1:ITER
	for (int iter = 1; iter <= ITER; iter++) {
		
		// while 1
		while (true) {
			
			int e1, e2;
			int a, b, c, d;
			
			// while 1
			while (true) {
				
				// e1=ceil(K*rand);
				e1 = gsl_rng_uniform_int(rng, K);
				
				// e2=ceil(K*rand);
				e2 = gsl_rng_uniform_int(rng, K);
				
				// while (e2==e1),
				while (e2 == e1) {
					
					// e2=ceil(K*rand);
					e2 = gsl_rng_uniform_int(rng, K);
				}
				
				// a=i(e1); b=j(e1);
				a = (int)gsl_vector_get(&i.vector, e1);
				b = (int)gsl_vector_get(&j.vector, e1);
				
				// c=i(e2); d=j(e2);
				c = (int)gsl_vector_get(&i.vector, e2);
				d = (int)gsl_vector_get(&j.vector, e2);
				
				// if all(a~=[c d]) && all(b~=[c d]);
				if (a != c && a != d && b != c && b != d) {
					
					// break
					break;
				}
			}
			
			// if rand>0.5
			if (gsl_rng_uniform(rng) > 0.5) {
				
				// i(e2)=d; j(e2)=c;
				gsl_vector_set(&i.vector, e2, (double)d);
				gsl_vector_set(&j.vector, e2, (double)c);
				
				// c=i(e2); d=j(e2);
				c = (int)gsl_vector_get(&i.vector, e2);
				d = (int)gsl_vector_get(&j.vector, e2);
			}
			
			// if ~(R(a,d) || R(c,b))
			if (fp_zero(gsl_matrix_get(_R, a, d)) && fp_zero(gsl_matrix_get(_R, c, b))) {
				
				// if (D(a,b)+D(c,d))>=(D(a,d)+D(c,b))
				if (gsl_matrix_get(D, a, b) + gsl_matrix_get(D, c, d) >=
					gsl_matrix_get(D, a, d) + gsl_matrix_get(D, c, b)) {
					
					// R(a,d)=R(a,b); R(a,b)=0;
					gsl_matrix_set(_R, a, d, gsl_matrix_get(_R, a, b));
					gsl_matrix_set(_R, a, b, 0.0);
					
					// R(d,a)=R(b,a); R(b,a)=0;
					gsl_matrix_set(_R, d, a, gsl_matrix_get(_R, b, a));
					gsl_matrix_set(_R, b, a, 0.0);
					
					// R(c,b)=R(c,d); R(c,d)=0;
					gsl_matrix_set(_R, c, b, gsl_matrix_get(_R, c, d));
					gsl_matrix_set(_R, c, d, 0.0);
					
					// R(b,c)=R(d,c); R(d,c)=0;
					gsl_matrix_set(_R, b, c, gsl_matrix_get(_R, d, c));
					gsl_matrix_set(_R, d, c, 0.0);
					
					// j(e1) = d;
					gsl_vector_set(&j.vector, e1, (double)d);
					
					// j(e2) = b;
					gsl_vector_set(&j.vector, e2, (double)b);
					
					// break;
					break;
				}
			}
		}
	}
	
	gsl_rng_free(rng);
	gsl_matrix_free(D);
	gsl_matrix_free(find_tril_R);
	return _R;
}
