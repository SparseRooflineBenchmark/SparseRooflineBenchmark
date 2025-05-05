#include "/root/SparseRooflineBenchmark/src/benchmark.hpp"
#include <sys/stat.h>
#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <binsparse/binsparse.h>
#include "aoclsparse.h"

namespace fs = std::filesystem;

int main(int argc, char **argv) {
    auto params = parse(argc, argv);

    std::cout << "Reading A from: " << (fs::path(params.input) / "662_bus.hdf5").string() << std::endl;
    std::cout << "Reading B from: " << (fs::path(params.input) / "662_bus.hdf5").string() << std::endl;

    bsp_matrix_t A = bsp_read_matrix((fs::path(params.input) / "662_bus.hdf5").string().c_str(), NULL);
    bsp_matrix_t B = bsp_read_matrix((fs::path(params.input) / "662_bus.hdf5").string().c_str(), NULL);
    //print the matrix info
    //print type of A.values.type A.pointers_to_1.type A.indices_1.type
    std::cout << "A.values.type: " << A.values.type << std::endl;
    std::cout << "A.pointers_to_1.type: " << A.pointers_to_1.type << std::endl;
    std::cout << "A.indices_1.type: " << A.indices_1.type << std::endl;
    uint16_t* pt1s = (uint16_t *) A.pointers_to_1.data;
    // for (int i = 0; i < A.pointers_to_1.size; i++){
    //     // uint8_t *apt = (uint8_t *) A.pointers_to_1.data;
    //     // bsp_array_read(A.pointers_to_1, i, apt[i]);
    //     std::cout << "pt1:" << ((int) pt1s[i]) << " ";
    // }
    double* vals = (double *) A.values.data;
    double* vals2 = (double *) B.values.data;

    // for (long i = 0; i < A.values.size; i++){
    //     // float x;
    //     // bsp_array_read(A.values, i, x);
    //     // std::cout << "v:" << x << " " << std::endl;
    //     std::cout << "v:" << vals[i] << " ";
    // }
    std::cout << std::endl;

    uint16_t* i1s = (uint16_t *) A.indices_1.data;
    uint16_t* i2s = (uint16_t *) B.indices_1.data;
    // for (int i = 0; i < A.indices_1.size; i++){
    //     // uint8_t x;
    //     // bsp_array_read(A.indices_1, i, x);
    //     std::cout << "i1:" << ((uint32_t) i1s[i]) << " ";
    // }
//     std::cout << std::endl;

    uint16_t* pt2s = (uint16_t *) B.pointers_to_1.data;
    // for (int i = 0; i < A.pointers_to_1.size; i++){
    //     // uint8_t *apt = (uint8_t *) A.pointers_to_1.data;
    //     // bsp_array_read(A.pointers_to_1, i, apt[i]);
    //     std::cout << "pt1:" << (int) pt1s[i] << " ";
    // }

    std::cout << "Inputs read successfully!" << std::endl;
    assert(A.format == BSP_CSR && B.format == BSP_CSR);
    assert(A.values.type == B.values.type);
    assert(A.ncols == B.nrows);

    aoclsparse_int m = A.nrows, n = B.ncols, k = A.ncols;
    aoclsparse_int nnz_A = A.values.size, nnz_B = B.values.size;
    std::cout << "m: " << m << " n: " << n << " k: " << k << " nnz_A: " << nnz_A << " nnz_B: " << nnz_B << std::endl;
   //print A.pointers_to_1.size
    std::cout << "A.pointers_to_1.size: " << A.pointers_to_1.size << std::endl;
    aoclsparse_int row_ptr_A[A.pointers_to_1.size];

    //use for loop to assign valuw from pt1s to row_ptr_A

    for (int i = 0; i < A.pointers_to_1.size; i++){
        row_ptr_A[i] = pt1s[i];
    }
    aoclsparse_int row_ptr_B[B.pointers_to_1.size];
    //use for loop to assign valuw from pt2s to row_ptr_B
    for (int i = 0; i < B.pointers_to_1.size; i++){
        row_ptr_B[i] = pt2s[i];
    }
    //do the same thing for values
    //print A.indices_1.size
    std::cout << "A.indices_1.size: " << A.indices_1.size << std::endl;
    aoclsparse_int col_ind_A[A.indices_1.size];
    for (int i = 0; i < A.indices_1.size; i++){
        col_ind_A[i] = i1s[i];
    }
    aoclsparse_int col_ind_B[B.indices_1.size];
    for (int i = 0; i < B.indices_1.size; i++){
        col_ind_B[i] = i2s[i];
    }
    double val_A[A.values.size];
    std::cout  << "A.values.size: " << A.values.size << std::endl;
    for (int i = 0; i < A.values.size; i++){
        val_A[i] = vals[i];
    }
    double val_B[B.values.size];
    for (int i = 0; i < B.values.size; i++){
        val_B[i] = vals2[i];
    }
   // print row col and val
    // for (int i = 0; i < A.pointers_to_1.size; i++){
    //     std::cout << "row_ptr_A: " << row_ptr_A[i] << " ";
    //     //print type of row_ptr_A[i]
    //     //std::cout << "row_ptr_A type: " << A.pointers_to_1.type << std::endl;
    // }
    // std::cout << std::endl;
    // for (int i = 0; i < A.indices_1.size; i++){
    //     std::cout << "col_ind_A: " << col_ind_A[i] << " ";
    // }
    // std::cout << std::endl;
    // for (int i = 0; i < A.values.size; i++){
    //     std::cout << "val_A: " << val_A[i] << " ";
    // }
    // std::cout << std::endl;
    // for (int i = 0; i < B.pointers_to_1.size; i++){
    //     std::cout << "row_ptr_B: " << row_ptr_B[i] << " ";
    // }
    // std::cout << std::endl;
    // for (int i = 0; i < B.indices_1.size; i++){
    //     std::cout << "col_ind_B: " << col_ind_B[i] << " ";
    // }
    // std::cout << std::endl;
    // for (int i = 0; i < B.values.size; i++){
    //     std::cout << "val_B: " << val_B[i] << " ";
    // }
    std::cout << std::endl;
    aoclsparse_status     status;
     aoclsparse_int        nnz_C;
     aoclsparse_request    request;
     aoclsparse_index_base base   = aoclsparse_index_base_zero;
     aoclsparse_operation  transA = aoclsparse_operation_none;
     aoclsparse_operation  transB = aoclsparse_operation_none;
     aoclsparse_matrix csrC          = NULL;
     aoclsparse_int   *csr_row_ptr_C = NULL;
     aoclsparse_int   *csr_col_ind_C = NULL;
     double            *csr_val_C     = NULL;
     aoclsparse_int    C_M, C_N;
     // Print aoclsparse version
     std::cout << aoclsparse_get_version() << std::endl;

     // Initialise matrix descriptor and csr matrix structure of inputs A and B
     aoclsparse_mat_descr descrA;
     aoclsparse_mat_descr descrB;
     aoclsparse_matrix    csrA;
     aoclsparse_matrix    csrB;

     // Create matrix descriptor of input matrices
     // aoclsparse_create_mat_descr set aoclsparse_matrix_type to aoclsparse_matrix_type_general
     // and aoclsparse_index_base to aoclsparse_index_base_zero.
      aoclsparse_create_mat_descr(&descrA);
      if (status != aoclsparse_status_success) {
        std::cerr << "Error: Failed to create descrA. Status code: " << status << std::endl;
        return EXIT_FAILURE;
    }
    else{
        std::cout << "descrA created successfully" << std::endl;
    }

     aoclsparse_create_mat_descr(&descrB);

    // aoclsparse_int row_ptr_A[] = {0, 1, 2, 3,5};
    //  aoclsparse_int col_ind_A[] = {1, 2, 2, 0, 3};
    //  float          val_A[]     = {5, 3, 2, 1, 5};
    //  aoclsparse_int row_ptr_B[] = {0, 1, 1, 3, 5};
    //  aoclsparse_int col_ind_B[] = {3, 0, 3, 0, 2};
    //  float          val_B[]     = {6, 2, 4, 3, 3};

     status = aoclsparse_create_dcsr(&csrA, base, m, k, nnz_A, row_ptr_A, col_ind_A, val_A);
     //print arguments

     if (status != aoclsparse_status_success) {
        std::cerr << "Error: Failed to create csrA" << status << std::endl;
        return EXIT_FAILURE;
    }
     aoclsparse_create_dcsr(&csrB, base, k, n, nnz_B, row_ptr_B, col_ind_B, val_B);


     std::cout << "Invoking aoclsparse_scsr2m with aoclsparse_stage_full_computation..";
     auto time = benchmark(
        []() {}, 
        [&]() {
     request = aoclsparse_stage_full_computation;
     status  = aoclsparse_dcsr2m(transA, descrA, csrA, transB, descrB, csrB, request, &csrC);
     if(status == aoclsparse_status_success)
         std::cout << "DONE\n";
     else
     {  
        std::cerr << "ERROR in aoclsparse_scsr2m: " << status << std::endl;
         std::cout << "ERROR in aoclsparse_scsr2m\n";
         exit(EXIT_FAILURE);
     }

     aoclsparse_export_dcsr(
         csrC, &base, &C_M, &C_N, &nnz_C, &csr_row_ptr_C, &csr_col_ind_C, &csr_val_C);

         //print C_M, C_N, nnz_C value

    } );

    // std::cout << "C_M" << C_M << "\t" << "C_N" << C_N << "\t" << "nnz_C" << nnz_C << std::endl;
    // //print csr_row_ptr_C
    // std::cout << "csr_row_ptr_C: " << std::endl;
    // for(aoclsparse_int i = 0; i < C_M + 1; i++)
    //     std::cout << csr_row_ptr_C[i] << "\t";
    // //print csr_col_ind_C
    // std::cout << "csr_col_ind_C: " << std::endl;
    // for(aoclsparse_int i = 0; i < nnz_C; i++)
    //     std::cout << csr_col_ind_C[i] << "\t";
    // //print csr_val_C
    // std::cout << "csr_val_C: " << std::endl;
    // for(aoclsparse_int i = 0; i < nnz_C; i++)
    //     std::cout << csr_val_C[i] << "\t";
    // auto time = benchmark(
    //     []() {},
    //     [&]() {


       // }

    // );

    bsp_matrix_t C = bsp_construct_default_matrix_t();
    C.format = BSP_CSR;
    C.nrows = C_M;
    C.ncols = C_N;
    C.nnz = nnz_C;
    std::cout << "C_M: " << C_M << " C_N: " << C_N << " nnz_C: " << nnz_C << std::endl;

    C.values = bsp_construct_array_t(nnz_C, BSP_FLOAT64);
    memcpy(C.values.data, csr_val_C, nnz_C * sizeof(double));
   // bsp_array_fill_random(C.values, 100);
    // Allocate and copy for pointers_to_1 (csr_row_ptr_C)
C.pointers_to_1 = bsp_construct_array_t(C_M + 1, BSP_INT32);
memcpy(C.pointers_to_1.data, csr_row_ptr_C, (C_M + 1) * sizeof(int32_t));

// Allocate and copy for indices_1 (csr_col_ind_C)
C.indices_1 = bsp_construct_array_t(nnz_C, BSP_INT32);
memcpy(C.indices_1.data, csr_col_ind_C, nnz_C * sizeof(int32_t));


    std::cout << "C_M" << C_M << "\t" << "C_N" << C_N << "\t" << "nnz_C" << nnz_C << std::endl;
            //print csr_row_ptr_C
            // for(aoclsparse_int i = 0; i < C_M + 1; i++)
            //     std::cout << csr_row_ptr_C[i] << "\t";
            // //print csr_col_ind_C
            // for(aoclsparse_int i = 0; i < nnz_C; i++)
            //     std::cout << csr_col_ind_C[i] << "\t";
            // //print csr_val_C
            // for(aoclsparse_int i = 0; i < nnz_C; i++)
            //     std::cout << csr_val_C[i] << "\t";
    bsp_write_matrix("C_amd.hdf5", C, NULL, NULL, 9);
    //bsp_destroy_matrix_t(C);

    json measurements;
    measurements["time"] = time;
    measurements["memory"] = 0;
    std::ofstream measurements_file( "measurements.json");
    measurements_file << measurements;
    measurements_file.close();
    std::cout << "Measurements written to: " << "measurements.json" << std::endl;
    aoclsparse_destroy_mat_descr(descrA);
    aoclsparse_destroy_mat_descr(descrB);
    aoclsparse_destroy(&csrA);
    aoclsparse_destroy(&csrB);
    aoclsparse_destroy(&csrC);

    bsp_destroy_matrix_t(A);
    bsp_destroy_matrix_t(B);
    std::cout << time << std::endl;
    std::cout << "Multiplication done!" << std::endl;

    return 0;
}


// The way I currently understand your slide:
// phase 1: convert application format to format to be input to library; create sparse matrices for library
// phase 2: spgemm inspect phase
// phase 2+3: spgemm computation and output
// phase 4: destroy sparse matrices and convert matrix output back to application format
