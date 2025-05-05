#include "../../src/benchmark.hpp"
#include <sys/stat.h>
#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <cassert>
#include <cstdlib>

#include <filesystem>
#include <fstream>

#include <binsparse/binsparse.h>

namespace fs = std::filesystem;

template <typename T, typename I>
//void experiment_spgemm_csr(benchmark_params_t params);
void experiment_spgemm_csr(benchmark_params_t params, bsp_matrix_t A, bsp_matrix_t B);

int main(int argc, char **argv) {
    auto params = parse(argc, argv);

    std::cout << "Reading A from: " << (realpath((fs::path(params.input) / "662_bus.hdf5").c_str(), NULL)) << std::endl;
    std::cout << "Reading B from: " << (realpath((fs::path(params.input) / "662_bus.hdf5").c_str(), NULL)) << std::endl;
    
    bsp_matrix_t A = bsp_read_matrix((realpath((fs::path(params.input) / "662_bus.hdf5").c_str(), NULL)), NULL);
    bsp_matrix_t B = bsp_read_matrix((realpath((fs::path(params.input) / "662_bus.hdf5").c_str(), NULL)), NULL);

    std::cout << "Inputs read successfully!" << std::endl;

    assert(A.format == BSP_CSR && B.format == BSP_CSR);
    assert(A.values.type == B.values.type);
    //assert(A.ncols = B.nrows);

    // hid_t f = H5Fcreate("values.hdf5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    // bsp_write_array(f, "values", A.values, 0);
    // H5Fclose(f);

    bsp_print_matrix_info(A);

    float* vals = (float *) A.values.data;
    // for (int i = 0; i < A.values.size; i++){
    //     // float x;
    //     // bsp_array_read(A.values, i, x);
    //     // std::cout << "v:" << x << " " << std::endl;
    //     std::cout << "v:" << vals[i] << " ";
    // }
    std::cout << std::endl;

    uint16_t* i1s = (uint16_t *) A.indices_1.data;
    // for (int i = 0; i < A.indices_1.size; i++){
    //     // uint8_t x;
    //     // bsp_array_read(A.indices_1, i, x);
    //     std::cout << "i1:" << ((int) i1s[i]) << " ";
    // }
    std::cout << std::endl;
    uint16_t* pt1s = (uint16_t *) A.pointers_to_1.data;
    for (int i = 0; i < A.pointers_to_1.size; i++){
        // uint8_t *apt = (uint8_t *) A.pointers_to_1.data;
        // bsp_array_read(A.pointers_to_1, i, apt[i]);
        std::cout << "pt1:" << (int) pt1s[i] << " ";
    }
    std::cout << std::endl;
    //print A.values.type
    //std::cout << "AAAAAAAAA type: " << A.values.type << std::endl;
    switch (A.values.type){
        case BSP_FLOAT32:
            experiment_spgemm_csr<float, int>(params, A, B);
            break;
        case BSP_FLOAT64:
            experiment_spgemm_csr<double, int>(params, A, B);
            break;
        default:
            //std::cout << "type: " << A.values.type << std::endl;
            throw std::runtime_error("bad type!");
    }

    std::cout << "multiplication done" << std::endl;

    bsp_destroy_matrix_t(A);
    bsp_destroy_matrix_t(B);
    return 0;
}

template <typename T, typename I>
void experiment_spgemm_csr(benchmark_params_t params, bsp_matrix_t A, bsp_matrix_t B) { 

    bsp_array_t A_ptr = A.pointers_to_1;
    bsp_array_t A_idx = A.indices_1;
    bsp_array_t A_val = A.values;

    bsp_array_t B_ptr = B.pointers_to_1;
    bsp_array_t B_idx = B.indices_1;
    bsp_array_t B_val = B.values;

    int m = A.nrows;
    int k = A.ncols;
    int n = B.ncols;

    // result matrix C
    std::vector<I> C_ptr(m + 1, 0);
    std::vector<I> C_idx;
    std::vector<T> C_val;
 
    std::vector<std::unordered_map<I, T>> tempC(m);

    std::cout << "beginning benchmark..." << std::endl;
    T test;
    bsp_array_read(A.values, 3, test);
    std::cout << "A.values" << test << std::endl;

    //perform SpGEMM (A * B = C)
    auto time = benchmark(
        []() {}, 
        [&A_ptr, &A_idx, &A_val, &B_ptr, &B_idx, &B_val, &tempC, m, k, n]() {
            int p, pmax, a_col, q, qmax, b_col;
            T val_a, val_b;
            for (int i = 0; i < m; ++i) {
                for (int j = 0; j < n; ++j) {
                    tempC[i][j] = 0;
                }
            }
            for (int i = 0; i < m; ++i) {
                bsp_array_read(A_ptr, i, p);
                bsp_array_read(A_ptr, i + 1, pmax);
    
                for (; p < pmax; ++p) {
                    bsp_array_read(A_idx, p, a_col);
                    bsp_array_read(A_val, p, val_a);
    
                    bsp_array_read(B_ptr, a_col, q);
                    bsp_array_read(B_ptr, a_col + 1, qmax);
    
                    for (; q < qmax; ++q) {
                        bsp_array_read(B_idx, q, b_col);
                        bsp_array_read(B_val, q, val_b);
    
                        tempC[i][b_col] += val_a * val_b; 
                    }
                }
            }
        }
    );
    //print tempC

    std::cout << "bleh" << std::endl;
    // tempC into CSR format -> result matrix
    for (int i = 0; i< m; ++i){
	    for (const auto& entry : tempC[i]){
		    if (entry.second != 0) {
		    	C_idx.push_back(entry.first);
		    	C_val.push_back(entry.second);
		    }
		}
	  	C_ptr[i + 1] = C_idx.size();
	}
    //print C_val
    // for (int i = 0; i < C_val.size(); i++){
    //     std::cout << "C_val:" << C_val[i] << " ";
    // }
    std::cout << "done" << std::endl;
    bsp_matrix_t C = bsp_construct_default_matrix_t();
    C.format = BSP_CSR;
    C.nrows = m;
    C.ncols = n;
    C.nnz = C_val.size();
    std::cout << "done1" << std::endl;
    //C.values = bsp_construct_default_array_t();
   
    C.values = bsp_construct_array_t(C.nnz, BSP_FLOAT64);
    memcpy(C.values.data, C_val.data(), C.nnz * sizeof(double));
    std::cout << "done2" << std::endl;
    C.pointers_to_1 = bsp_construct_array_t(C_ptr.size(), BSP_INT32);
    memcpy(C.pointers_to_1.data, C_ptr.data(), C_ptr.size() * sizeof(int32_t));

// Allocate and copy for C.indices_1
    C.indices_1 = bsp_construct_array_t(C_idx.size(), BSP_INT32);   
    memcpy(C.indices_1.data, C_idx.data(), C_idx.size() * sizeof(int32_t));
    std::cout << "done4" << std::endl;
    bsp_write_matrix((fs::path(params.output) / "C.hdf5").string().c_str(), C, NULL, NULL, 9);
    std::cout << "done5" << std::endl;
    //bsp_destroy_matrix_t(C);
    std::cout << "done6" << std::endl;
    // benchmark measurement
    json measurements;
    std::cout << "done7" << std::endl;
    measurements["time"] = time;
    std::cout << time << std::endl;
    measurements["memory"] = 0;
    std::cout << "done8" << std::endl;
    std::ofstream measurements_file(fs::path(params.output) / "measurements.json");
    std::cout << "done9" << std::endl;
    measurements_file << measurements;
    measurements_file.close();
    std::cout << "done10" << std::endl;
}