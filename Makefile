CC = gcc
LD = ld
CXXFLAGS += -std=c++11 -I$(TACO)/include -I$(TACO)/src
LDLIBS += -L$(TACO)/build/lib -ltaco -ldl

all: spmv_taco spmspv_taco alpha_taco_rle alpha_opencv triangle_taco all_pairs_opencv conv_opencv

clean:
	rm -rf spmv_taco
	rm -rf spmspv_taco
	rm -rf conv_opencv
	rm -rf triangle_taco
	rm -rf alpha_opencv
	rm -rf alpha_taco_rle
	rm -rf all_pairs_opencv
	rm -rf *.o *.dSYM *.trace

superclean: clean
	rm -rf $(TACO)/build
	rm -rf $(TACORLE)/build
	rm -rf $(OPENCV)/build
	rm -rf $(OPENCV)/install
	rm -rf scratch

spmv_taco: spmv_taco.o $(TACOBUILD)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

spmspv_taco: spmspv_taco.o $(TACOBUILD)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

triangle_taco: triangle_taco.o $(TACOBUILD)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

alpha_taco_rle: alpha_taco_rle.cpp $(TACORLEBUILD)
	$(CXX) $(CXXFLAGS_TACORLE) -o $@ $< $(LDLIBS_TACORLE)

alpha_opencv: alpha_opencv.cpp $(OPENCVBUILD)
	$(CXX) $(CXXFLAGS_CV) -o $@ $< $(LDLIBS_CV)

all_pairs_opencv: all_pairs_opencv.cpp $(OPENCVBUILD)
	$(CXX) $(CXXFLAGS_CV) -o $@ $< $(LDLIBS_CV)

conv_opencv: conv_opencv.cpp $(OPENCVBUILD)
	$(CXX) $(CXXFLAGS_CV) -o $@ $< $(LDLIBS_CV)

$(OPENCVBUILD):
	mkdir -p opencv/build ;\
	mkdir -p opencv/install ;\
	cd opencv/build ;\
	cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=../install -DBUILD_ZLIB=OFF -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_opencv_apps=OFF -DBUILD_PNG=ON -D WITH_FFMPEG=OFF -DBUILD_TIFF=ON .. ;\
	make -j$(NPROC_VAL) ;\
	make install

$(TACOBUILD):
	cd $(TACO) ;\
	mkdir -p build ;\
	cd build ;\
	cmake -DPYTHON=false -DCMAKE_BUILD_TYPE=Release .. ;\
	make taco -j$(NPROC_VAL)

$(TACORLEBUILD):
	cd $(TACORLE) ;\
	mkdir -p build ;\
	cd build ;\
	cmake -DPYTHON=false -DCMAKE_BUILD_TYPE=Release .. ;\
	make taco -j$(NPROC_VAL)
