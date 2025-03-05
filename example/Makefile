CC = gcc
LD = ld
CXXFLAGS += -std=c++20
LDLIBS +=

all: spmv spmm

clean:
	rm -rf spmv
	rm -rf spmm
	rm -rf *.o *.dSYM *.trace

spmv: spmv.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

spmm: spmm.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)
