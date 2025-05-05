CC = gcc
LD = ld
CXXFLAGS += -std=c++20
LDLIBS +=

all: spmv

clean:
	rm -rf spgemm
	rm -rf *.o *.dSYM *.trace

spgemm: spgemm.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)