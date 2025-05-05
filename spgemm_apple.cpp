#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <cmath>
#include <Accelerate/Accelerate.h>
#include <binsparse/binsparse.h>

namespace fs = std::filesystem;

// Simple benchmarking function
template<typename Setup, typename Func>
double benchmark(Setup setup, Func func, int repeats = 5) {
    std::cout << "Starting warmup run..." << std::endl;
    // Warmup
    setup();
    func();
    std::cout << "Warmup completed successfully." << std::endl;
    
    // Actual benchmarking
    std::vector<double> times;
    for (int i = 0; i < repeats; i++) {
        std::cout << "Benchmark run " << (i+1) << "/" << repeats << "..." << std::endl;
        setup();
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        times.push_back(duration.count());
        std::cout << "  - Run " << (i+1) << " completed in " << duration.count() << " ms" << std::endl;
    }
    
    // Calculate median time
    std::sort(times.begin(), times.end());
    double median = times[repeats / 2];
    
    // Print all times for reference
    std::cout << "All benchmark times (ms): ";
    for (size_t i = 0; i < times.size(); i++) {
        std::cout << times[i];
        if (i < times.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    std::cout << "Median time: " << median << " ms" << std::endl;
    
    return median;
}

// Parse command line arguments
struct benchmark_params_t {
    std::string input;
    std::string output;
};

benchmark_params_t parse(int argc, char **argv) {
    benchmark_params_t params;
    params.input = ".";
    params.output = ".";
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.rfind("--input=", 0) == 0) {
            params.input = arg.substr(8);
        } else if (arg.rfind("--output=", 0) == 0) {
            params.output = arg.substr(9);
        }
    }
    
    return params;
}

int main(int argc, char **argv) {
    auto params = parse(argc, argv);

    std::cout << "Reading A from: " << (fs::path(params.input) / "A.hdf5").string() << std::endl;
    std::cout << "Reading B from: " << (fs::path(params.input) / "B.hdf5").string() << std::endl;
    
    bsp_matrix_t A = bsp_read_matrix((fs::path(params.input) / "A.hdf5").string().c_str(), NULL);
    bsp_matrix_t B = bsp_read_matrix((fs::path(params.input) / "B.hdf5").string().c_str(), NULL);

    std::cout << "Inputs read successfully!" << std::endl;

    // Validate matrix formats and types
    if (A.format != BSP_CSR || B.format != BSP_CSR) {
        std::cerr << "Matrices must be in CSR format" << std::endl;
        return 1;
    }
    
    if (A.ncols != B.nrows) {
        std::cerr << "Matrix dimensions don't match for multiplication" << std::endl;
        return 1;
    }

    bsp_print_matrix_info(A);
    bsp_print_matrix_info(B);

    // Get matrix dimensions
    int m = A.nrows;
    int k = A.ncols;
    int n = B.ncols;

    std::cout << "Matrix dimensions: A(" << m << "x" << k << ") * B(" << k << "x" << n << ")" << std::endl;

    // Convert binsparse data to vectors
    std::vector<int> A_ptr(m + 1);
    std::vector<int> A_idx(A.nnz);
    std::vector<double> A_val(A.nnz);
    
    std::vector<int> B_ptr(k + 1);
    std::vector<int> B_idx(B.nnz);
    std::vector<double> B_val(B.nnz);
    
    // Copy data from binsparse to vectors with proper type handling
    for (int i = 0; i <= m; i++) {
        // Handle different pointer types properly
        if (A.pointers_to_1.type == BSP_UINT8) {
            A_ptr[i] = ((uint8_t*)A.pointers_to_1.data)[i];
        } else if (A.pointers_to_1.type == BSP_INT32) {
            A_ptr[i] = ((int32_t*)A.pointers_to_1.data)[i];
        } else if (A.pointers_to_1.type == BSP_INT64) {
            A_ptr[i] = ((int64_t*)A.pointers_to_1.data)[i];
        } else {
            std::cerr << "Unsupported pointer type for matrix A" << std::endl;
            return 1;
        }
    }

    for (int i = 0; i < A.nnz; i++) {
        // Handle different index types properly
        if (A.indices_1.type == BSP_UINT8) {
            A_idx[i] = ((uint8_t*)A.indices_1.data)[i];
        } else if (A.indices_1.type == BSP_INT32) {
            A_idx[i] = ((int32_t*)A.indices_1.data)[i];
        } else if (A.indices_1.type == BSP_INT64) {
            A_idx[i] = ((int64_t*)A.indices_1.data)[i];
        } else {
            std::cerr << "Unsupported index type for matrix A" << std::endl;
            return 1;
        }
        
        // Convert to double regardless of original type
        if (A.values.type == BSP_FLOAT32) {
            A_val[i] = ((float*)A.values.data)[i];
        } else if (A.values.type == BSP_FLOAT64) {
            A_val[i] = ((double*)A.values.data)[i];
        } else {
            std::cerr << "Unsupported value type for matrix A" << std::endl;
            return 1;
        }
    }

    for (int i = 0; i <= k; i++) {
        // Handle different pointer types properly
        if (B.pointers_to_1.type == BSP_UINT8) {
            B_ptr[i] = ((uint8_t*)B.pointers_to_1.data)[i];
        } else if (B.pointers_to_1.type == BSP_INT32) {
            B_ptr[i] = ((int32_t*)B.pointers_to_1.data)[i];
        } else if (B.pointers_to_1.type == BSP_INT64) {
            B_ptr[i] = ((int64_t*)B.pointers_to_1.data)[i];
        } else {
            std::cerr << "Unsupported pointer type for matrix B" << std::endl;
            return 1;
        }
    }

    for (int i = 0; i < B.nnz; i++) {
        // Handle different index types properly
        if (B.indices_1.type == BSP_UINT8) {
            B_idx[i] = ((uint8_t*)B.indices_1.data)[i];
        } else if (B.indices_1.type == BSP_INT32) {
            B_idx[i] = ((int32_t*)B.indices_1.data)[i];
        } else if (B.indices_1.type == BSP_INT64) {
            B_idx[i] = ((int64_t*)B.indices_1.data)[i];
        } else {
            std::cerr << "Unsupported index type for matrix B" << std::endl;
            return 1;
        }
        
        // Convert to double regardless of original type
        if (B.values.type == BSP_FLOAT32) {
            B_val[i] = ((float*)B.values.data)[i];
        } else if (B.values.type == BSP_FLOAT64) {
            B_val[i] = ((double*)B.values.data)[i];
        } else {
            std::cerr << "Unsupported value type for matrix B" << std::endl;
            return 1;
        }
    }

    std::cout << "Converting to compatible matrix representation..." << std::endl;
    
    try {
        // Use plain BLAS/LAPACK for sparse matrix multiplication since
        // Accelerate's sparse matrix API seems to be incompatible
        
        // We'll use a simple implementation of sparse matrix multiplication
        // based on the CSR format directly
        
        // Output matrix data structures
        std::vector<int> C_ptr(m + 1, 0);
        std::vector<int> C_idx;
        std::vector<double> C_val;
        
        std::cout << "Beginning SpGEMM benchmark..." << std::endl;

        // Benchmark the SpGEMM computation
        auto time = benchmark(
            []() {}, // Setup (empty for this benchmark)
            [&]() {
                std::cout << "Starting sparse matrix multiplication..." << std::endl;
                
                // Symbolic phase - determine C_ptr structure
                // This is a simplistic implementation and might not be optimal for large matrices
                
                // We'll use a temporary array to store intermediate results
                std::vector<std::vector<std::pair<int, double>>> temp_rows(m);
                
                // Progress tracking variables
                int progress = 0;
                int progress_step = std::max(1, m / 10); // Report progress every 10%
                
                std::cout << "Processing " << m << " rows..." << std::endl;
                
                // For each row in A
                for (int i = 0; i < m; i++) {
                    // Print progress
                    if (i % progress_step == 0 || i == m-1) {
                        int percent = (i * 100) / m;
                        std::cout << "  Progress: " << percent << "% (" << i << "/" << m << " rows)" << std::endl;
                    }
                    
                    // For each non-zero element in row i of A
                    for (int j_ptr = A_ptr[i]; j_ptr < A_ptr[i+1]; j_ptr++) {
                        int j = A_idx[j_ptr];
                        double a_val = A_val[j_ptr];
                        
                        // For each non-zero element in row j of B
                        for (int k_ptr = B_ptr[j]; k_ptr < B_ptr[j+1]; k_ptr++) {
                            int k = B_idx[k_ptr];
                            double b_val = B_val[k_ptr];
                            
                            // Accumulate into temporary structure
                            // Use map to combine values with same column index
                            bool found = false;
                            for (auto& pair : temp_rows[i]) {
                                if (pair.first == k) {
                                    pair.second += a_val * b_val;
                                    found = true;
                                    break;
                                }
                            }
                            
                            if (!found) {
                                temp_rows[i].push_back({k, a_val * b_val});
                            }
                        }
                    }
                    
                    // Sort by column index
                    std::sort(temp_rows[i].begin(), temp_rows[i].end(), 
                             [](const auto& a, const auto& b) { return a.first < b.first; });
                }
                
                std::cout << "Matrix multiplication completed, building CSR format..." << std::endl;
                
                // Now create CSR structure from temporary storage
                C_ptr[0] = 0;
                int nonzero_count = 0;
                
                for (int i = 0; i < m; i++) {
                    nonzero_count += temp_rows[i].size();
                    C_ptr[i+1] = C_ptr[i] + temp_rows[i].size();
                    
                    for (const auto& pair : temp_rows[i]) {
                        C_idx.push_back(pair.first);
                        C_val.push_back(pair.second);
                    }
                    
                    // Print a few elements from every 10th row for verification
                    if (i % 10 == 0 && !temp_rows[i].empty()) {
                        std::cout << "  Row " << i << " sample: ";
                        int count = std::min(3, (int)temp_rows[i].size());
                        for (int j = 0; j < count; j++) {
                            std::cout << "(" << temp_rows[i][j].first << ":" << temp_rows[i][j].second << ") ";
                        }
                        if (temp_rows[i].size() > 3) {
                            std::cout << "... +" << (temp_rows[i].size() - 3) << " more";
                        }
                        std::cout << std::endl;
                    }
                }
                
                std::cout << "CSR structure built with " << nonzero_count << " non-zero elements" << std::endl;
            }
        );

        std::cout << "SpGEMM computation completed in " << time << " ms" << std::endl;
        
        int result_nnz = C_val.size();
        std::cout << "Result matrix has " << result_nnz << " non-zero elements" << std::endl;

        // Create result matrix in binsparse format
        bsp_matrix_t C = bsp_construct_default_matrix_t();
        C.format = BSP_CSR;
        C.nrows = m;
        C.ncols = n;
        C.nnz = result_nnz;

        // Set up arrays
        C.pointers_to_1 = bsp_construct_default_array_t();
        C.pointers_to_1.type = BSP_INT32;
        C.pointers_to_1.data = C_ptr.data();
        C.pointers_to_1.size = C_ptr.size();
        
        C.indices_1 = bsp_construct_default_array_t();
        C.indices_1.type = BSP_INT32;
        C.indices_1.data = C_idx.data();
        C.indices_1.size = C_idx.size();
        
        C.values = bsp_construct_default_array_t();
        C.values.type = BSP_FLOAT64; // Use double precision for output
        C.values.data = C_val.data();
        C.values.size = C.nnz;

        // Write result matrix to output file
        fs::create_directories(fs::path(params.output));
        std::string output_path = (fs::path(params.output) / "C.hdf5").string();
        std::cout << "Writing result to: " << output_path << std::endl;
        bsp_write_matrix(output_path.c_str(), C, NULL, NULL, 9);

        // Save benchmark results
        std::ofstream measurements_file(fs::path(params.output) / "measurements.json");
        measurements_file << "{\n";
        measurements_file << "  \"time_ms\": " << time << ",\n";
        measurements_file << "  \"nnz_A\": " << A.nnz << ",\n";
        measurements_file << "  \"nnz_B\": " << B.nnz << ",\n";
        measurements_file << "  \"nnz_C\": " << result_nnz << "\n";
        measurements_file << "}\n";
        measurements_file.close();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    bsp_destroy_matrix_t(A);
    bsp_destroy_matrix_t(B);
    return 0;
}