#include "../src/benchmark.hpp"
#include <sys/stat.h>
#include <iostream>
#include <cstdint>
#include <unordered_map>

namespace fs = std::filesystem;

template <typename T, typename I>
void experiment_spgemm_csr(benchmark_params_t params);

int main(int argc, char **argv) {
    auto params = parse(argc, argv);
    auto A_desc = json::parse(std::ifstream(fs::path(params.input) / "A.bspnpy" / "binsparse.json"))["binsparse"];
    auto B_desc = json::parse(std::ifstream(fs::path(params.input) / "B.bspnpy" / "binsparse.json"))["binsparse"];

    if (A_desc["format"] != "CSR") {
        throw std::runtime_error("Only CSR format for A is supported");
    }
    if (B_desc["format"] != "CSR") {
        throw std::runtime_error("Only CSR format for B is supported");
    }
 
    if (A_desc["data_types"]["pointers_to_1"] == "int32" && A_desc["data_types"]["values"] == "float64" &&
        B_desc["data_types"]["pointers_to_1"] == "int32" && B_desc["data_types"]["values"] == "float64") {
        experiment_spgemm_csr<double, int32_t>(params);
    } else if (A_desc["data_types"]["pointers_to_1"] == "int64" && A_desc["data_types"]["values"] == "float64" &&
               B_desc["data_types"]["pointers_to_1"] == "int64" && B_desc["data_types"]["values"] == "float64") {
        experiment_spgemm_csr<double, int64_t>(params);
    } else {
        std::cerr << "A pointers_to_1_type: " << A_desc["data_types"]["pointers_to_1"] << std::endl;
        std::cerr << "A values_type: " << A_desc["data_types"]["values"] << std::endl;
        std::cerr << "B pointers_to_1_type: " << B_desc["data_types"]["pointers_to_1"] << std::endl;
        std::cerr << "B values_type: " << B_desc["data_types"]["values"] << std::endl;
        throw std::runtime_error("Unsupported data types");
    }

    return 0;
}

template <typename T, typename I>
void experiment_spgemm_csr(benchmark_params_t params) { 
    auto A_desc = json::parse(std::ifstream(fs::path(params.input) / "A.bspnpy" / "binsparse.json"))["binsparse"];
    auto B_desc = json::parse(std::ifstream(fs::path(params.input) / "B.bspnpy" / "binsparse.json"))["binsparse"];

    int m = A_desc["shape"][0];
    int k = A_desc["shape"][1];
    int n = B_desc["shape"][1];

    auto A_ptr = npy_load_vector<I>(fs::path(params.input) / "A.bspnpy" / "pointers_to_1.npy");
    auto A_idx = npy_load_vector<I>(fs::path(params.input) / "A.bspnpy" / "indices_1.npy");
    auto A_val = npy_load_vector<T>(fs::path(params.input) / "A.bspnpy" / "values.npy");

    auto B_ptr = npy_load_vector<I>(fs::path(params.input) / "B.bspnpy" / "pointers_to_1.npy");
    auto B_idx = npy_load_vector<I>(fs::path(params.input) / "B.bspnpy" / "indices_1.npy");
    auto B_val = npy_load_vector<T>(fs::path(params.input) / "B.bspnpy" / "values.npy");

    // result matrix C
    std::vector<I> C_ptr(m + 1, 0);
    std::vector<I> C_idx;
    std::vector<T> C_val;
 
    std::vector<std::unordered_map<I, T>> tempC(m);

    // perform SpGEMM (A * B = C)
    auto time = benchmark(
        []() {}, 
        [&A_ptr, &A_idx, &A_val, &B_ptr, &B_idx, &B_val, &tempC, m, k, n]() {
            for (int i = 0; i < m; ++i) {
                for (int p = A_ptr[i]; p < A_ptr[i + 1]; ++p) {
                    int a_col = A_idx[p];
                    T a_val = A_val[p];

                    for (int q = B_ptr[a_col]; q < B_ptr[a_col + 1]; ++q) {
                        int b_col = B_idx[q];
                        T b_val = B_val[q];

                        tempC[i][b_col] += a_val * b_val;
                    }
                }
            }
        }
    );

    // tempC into CSR format -> result matrix
    for (int i = 0; i < m; ++i) {
        for (const auto& entry : tempC[i]) {
            C_idx.push_back(entry.first);
            C_val.push_back(entry.second);
        }
        C_ptr[i + 1] = C_idx.size();
    }

    // output directory
    fs::create_directory(fs::path(params.output) / "C.bspnpy");
    json C_desc;
    C_desc["version"] = 0.5;
    C_desc["format"] = "CSR";
    C_desc["shape"] = {m, n};
    C_desc["nnz"] = C_val.size();
    C_desc["data_types"]["pointers_to_1"] = (sizeof(I) == 4) ? "int32" : "int64";
    C_desc["data_types"]["indices_1"] = (sizeof(I) == 4) ? "int32" : "int64";
    C_desc["data_types"]["values"] = "float64";
    std::ofstream C_desc_file(fs::path(params.output) / "C.bspnpy" / "binsparse.json");
    C_desc_file << C_desc;
    C_desc_file.close();

    npy_store_vector<I>(fs::path(params.output) / "C.bspnpy" / "pointers_to_1.npy", C_ptr);
    npy_store_vector<I>(fs::path(params.output) / "C.bspnpy" / "indices_1.npy", C_idx);
    npy_store_vector<T>(fs::path(params.output) / "C.bspnpy" / "values.npy", C_val);

    // benchmark measurements
    json measurements;
    measurements["time"] = time;
    measurements["memory"] = 0;
    std::ofstream measurements_file(fs::path(params.output) / "measurements.json");
    measurements_file << measurements;
    measurements_file.close();
}

