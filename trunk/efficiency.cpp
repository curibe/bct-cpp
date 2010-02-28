#include "bct.h"
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

gsl_matrix* distance_inv(const gsl_matrix*);

/*
 * Computes global efficiency for a binary undirected graph.
 */
gsl_matrix* bct::efficiency_global(const gsl_matrix* G) {
	
	// E=distance_inv(G);
	return distance_inv(G);
}

/*
 * Computes local efficiency for a binary undirected graph.
 */
gsl_vector* bct::efficiency_local(const gsl_matrix* G) {
	
	// N=length(G);
	int N = length(G);
	
	// E=zeros(N,1);
	gsl_vector* E = zeros_vector_double(N);
	
	// for u=1:N
	for (int u = 0; u < N; u++) {
		
		// V=find(G(u,:));
		gsl_vector_const_view G_row_u = gsl_matrix_const_row(G, u);
		gsl_vector* V = find(&G_row_u.vector);
		if (V != NULL) {
			
			// k=length(V);
			int k = length(V);
			
			// if k>=2;
			if (k >= 2) {
				
				// e=distance_inv(G(V,V));
				gsl_matrix* G_idx = ordinal_index(G, V, V);
				gsl_matrix* e = distance_inv(G_idx);
				gsl_matrix_free(G_idx);
				
				// E(u)=sum(e(:))./(k.^2-k);
				gsl_vector* e_v = to_vector(e);
				double sum_e = sum(e_v);
				gsl_vector_free(e_v);
				gsl_vector_set(E, u, sum_e / (double)(k * k - k));
			}
			
			gsl_vector_free(V);
		}
	}
	
	return E;
}

gsl_matrix* distance_inv(const gsl_matrix* g) {
	using namespace bct;
	
	// D=eye(length(g));
	gsl_matrix* D = eye_double(length(g));
	
	// n=1;
	int n = 1;
	
	// nPATH=g;
	gsl_matrix* nPATH = copy(g);
	
	// L=(nPATH~=0);
	gsl_matrix* L = compare_elements(nPATH, fp_not_equal, 0.0);
	
	// while find(L,1);
	gsl_vector* find_L = find(L, 1);
	while (find_L != NULL) {
		gsl_vector_free(find_L);
		
		// D=D+n.*L;
		gsl_matrix_scale(L, (double)n);
		gsl_matrix_add(D, L);
		
		// n=n+1;
		n++;
		
		// nPATH=nPATH*g;
		gsl_matrix* temp = mul(nPATH, g);
		gsl_matrix_free(nPATH);
		nPATH = temp;
		
		// L=(nPATH~=0).*(D==0);
		gsl_matrix_free(L);
		L = compare_elements(nPATH, fp_not_equal, 0.0);
		gsl_matrix* D_eq_0 = compare_elements(D, fp_equal, 0.0);
		gsl_matrix_mul_elements(L, D_eq_0);
		gsl_matrix_free(D_eq_0);
		
		find_L = find(L, 1);
	}
	
	gsl_matrix_free(nPATH);
	gsl_matrix_free(L);
	
	// D(~D)=inf;
	gsl_matrix* not_D = logical_not(D);
	logical_index_assign(D, not_D, GSL_POSINF);
	gsl_matrix_free(not_D);
	
	// D=1./D;
	gsl_matrix* temp = pow_elements(D, -1.0);
	gsl_matrix_free(D);
	D = temp;
	
	// D=D-eye(length(g));
	gsl_matrix* eye_length_g = eye_double(length(g));
	gsl_matrix_sub(D, eye_length_g);
	gsl_matrix_free(eye_length_g);
	
	return D;
}
