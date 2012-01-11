# TODO: -arch doesn't work anymore?
# TODO: -m32 needed on 64-bit machines, otherwise SWIG won't work?

CXXFLAGS                 = -Wall
# OUTPUT_OPTION            = -o .obj/$@
swig_flags               = -Wall -c++ -python -outputtuple
objects                  = assortativity.o \
                           betweenness_bin.o \
                           betweenness_wei.o \
                           breadth.o \
                           breadthdist.o \
                           cat.o \
                           charpath.o \
                           clustering_coef_bd.o \
                           clustering_coef_bu.o \
                           clustering_coef_wd.o \
                           clustering_coef_wu.o \
                           connectivity_length.o \
                           convert.o \
                           cycprob.o \
                           debug.o \
                           degrees_dir.o \
                           degrees_und.o \
                           density_dir.o \
                           density_und.o \
                           distance_bin.o \
                           distance_wei.o \
                           efficiency.o \
                           eigenvector_centrality.o \
                           erange.o \
                           find_motif34.o \
                           findpaths.o \
                           findwalks.o \
                           fve.o \
                           jdegree.o \
                           latmio_dir.o \
                           latmio_dir_connected.o \
                           latmio_und.o \
                           latmio_und_connected.o \
                           macaque.o \
                           make_motif34lib.o \
                           makeevenCIJ.o \
                           makefractalCIJ.o \
                           makelatticeCIJ.o \
                           makerandCIJ_bd.o \
                           makerandCIJ_bu.o \
                           makerandCIJ_wd.o \
                           makerandCIJ_wu.o \
                           makerandCIJdegreesfixed.o \
                           makeringlatticeCIJ.o \
                           maketoeplitzCIJ.o \
                           matching_ind.o \
                           matlab/compare.o \
                           matlab/convert.o \
                           matlab/functions.o \
                           matlab/index.o \
                           matlab/operators.o \
                           matlab/utility.o \
                           modularity_louvain.o \
                           modularity_newman.o \
                           module_degree_zscore.o \
                           motif3funct_bin.o \
                           motif3funct_wei.o \
                           motif3struct_bin.o \
                           motif3struct_wei.o \
                           motif4funct_bin.o \
                           motif4funct_wei.o \
                           motif4struct_bin.o \
                           motif4struct_wei.o \
                           normalized_path_length.o \
                           participation_coef.o \
                           randmio_dir.o \
                           randmio_dir_connected.o \
                           randmio_und.o \
                           randmio_und_connected.o \
                           reachdist.o \
                           status.o \
                           strengths_dir.o \
                           strengths_und.o \
                           threshold_absolute.o \
                           threshold_proportional.o \
                           utility.o

include Makefile.vars

# To make a multi-architecture library:
# Compile once for each architecture by adding -arch to CXXFLAGS and LDFLAGS during compilation
# Then execute "lipo -create <architecture-specific libraries> -output <universal library>"

.PHONY: all clean install swig uninstall

all: libbct.a

libbct.a: $(objects)
	$(AR) rcs libbct.a $(objects)

swig: $(objects)
	swig $(swig_flags) -o bct_gsl_wrap.cpp bct_gsl.i
	swig $(swig_flags) -o bct_py_wrap.cpp bct_py.i
	$(CXX) $(CXXFLAGS) -c -I$(python_dir) bct_gsl_wrap.cpp
	$(CXX) $(CXXFLAGS) -c -I$(python_dir) bct_py_wrap.cpp
	$(CXX) $(CXXFLAGS) -lgsl -lgslcblas $(swig_lib_flags) -o _bct_gsl.so $(objects) bct_gsl_wrap.o
	$(CXX) $(CXXFLAGS) -lgsl -lgslcblas $(swig_lib_flags) -o _bct_py.so $(objects) bct_py_wrap.o

$(objects): matlab/matlab.h bct.h

install: libbct.a
	if [ ! -d $(install_dir)/include/bct ]; then \
		mkdir $(install_dir)/include/bct; \
		mkdir $(install_dir)/include/bct/matlab; \
	fi
	if [[ "$(CXXFLAGS)" == *GSL_FLOAT* ]]; then \
		cat bct_float.h bct.h > $(install_dir)/include/bct/bct_float.h; \
	elif [[ "$(CXXFLAGS)" == *GSL_DOUBLE* ]]; then \
		cat bct_double.h bct.h > $(install_dir)/include/bct/bct.h; \
	elif [[ "$(CXXFLAGS)" == *GSL_LONG_DOUBLE* ]]; then \
		cat bct_long_double.h bct.h > $(install_dir)/include/bct/bct_long_double.h; \
	fi
	cp matlab/matlab.h $(install_dir)/include/bct/matlab
	cp matlab/sort.h $(install_dir)/include/bct/matlab
	cp precision.h $(install_dir)/include/bct
	if [[ "$(CXXFLAGS)" == *GSL_FLOAT* ]]; then \
		cp libbct.a $(install_dir)/lib/libbct_float.a; \
	elif [[ "$(CXXFLAGS)" == *GSL_DOUBLE* ]]; then \
		cp libbct.a $(install_dir)/lib/libbct.a; \
	elif [[ "$(CXXFLAGS)" == *GSL_LONG_DOUBLE* ]]; then \
		cp libbct.a $(install_dir)/lib/libbct_long_double.a; \
	fi

uninstall:
	-rm -rf $(install_dir)/include/bct
	-rm $(install_dir)/lib/libbct_float.a
	-rm $(install_dir)/lib/libbct.a
	-rm $(install_dir)/lib/libbct_long_double.a

clean:
	-rm _bct_gsl.so _bct_py.so bct_gsl.py bct_py.py bct_gsl.pyc bct_py.pyc bct_gsl_wrap.cpp bct_py_wrap.cpp bct_gsl_wrap.o bct_py_wrap.o libbct.a $(objects)
