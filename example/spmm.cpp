#include "../src/benchmark.hpp"
#include <sys/stat.h>
#include <iostream>
#include <cstdint>

namespace fs = std::filesystem;

template <typename T, typename I>
void experiment_spmm_csr(benchmark_params_t params);

int main(int argc, char **argv){
    auto params = parse(argc, argv);
    auto A_desc = json::parse(std::ifstream(fs::path(params.input)/"A.bspnpy"/"binsparse.json"))["binsparse"]; 
    auto X_desc = json::parse(std::ifstream(fs::path(params.input)/"X.bspnpy"/"binsparse.json"))["binsparse"];

    //print format
    if (A_desc["format"] != "CSR") {throw std::runtime_error("Only CSR format for A is supported");}
    if (X_desc["format"] != "CSR") {throw std::runtime_error("Only CSR format for X is supported");}

    if (A_desc["data_types"]["pointers_to_1"] == "int32" &&
        A_desc["data_types"]["values"] == "float64") {
            experiment_spmm_csr<double, int32_t>(params);
    } else if (A_desc["data_types"]["pointers_to_1"] == "int64" &&
        A_desc["data_types"]["values"] == "float64") {
            experiment_spmm_csr<double, int64_t>(params);
    } else {
        std::cout << "pointers_to_1_type: " << A_desc["data_types"]["pointers_to_1"] << std::endl;
        std::cout << "values_type: " << A_desc["data_types"]["values"] << std::endl;
        throw std::runtime_error("Unsupported data types");
    }

    return 0;

}

template <typename T, typename I>
void experiment_spmm_csr(benchmark_params_t params){

    auto A_desc = json::parse(std::ifstream(fs::path(params.input)/"A.bspnpy"/"binsparse.json"))["binsparse"]; 
    auto X_desc = json::parse(std::ifstream(fs::path(params.input)/"X.bspnpy"/"binsparse.json"))["binsparse"]; 

    int m = A_desc["shape"][0];
    int k = A_desc["shape"][1];

    int d = X_desc["shape"][1];

    auto A_ptr = npy_load_vector<I>(fs::path(params.input)/"A.bspnpy"/"pointers_to_1.npy");
    auto A_idx = npy_load_vector<I>(fs::path(params.input)/"A.bspnpy"/"indices_1.npy");
    auto A_val = npy_load_vector<T>(fs::path(params.input)/"A.bspnpy"/"values.npy");

    auto X_ptr = npy_load_vector<I>(fs::path(params.input)/"X.bspnpy"/"pointers_to_1.npy");
    auto X_idx = npy_load_vector<I>(fs::path(params.input)/"X.bspnpy"/"indices_1.npy");
    auto X_val = npy_load_vector<T>(fs::path(params.input)/"X.bspnpy"/"values.npy");


    auto Y_val = std::vector<T>(m, std::vector<T>(d, 0));

    auto time = benchmark(
    []() {
    },
        [&y_val, &A_ptr, &A_val, &A_idx, &X_ptr, &A_val, &X_idx, &m, &k, &d]() {
            for (int i = 0; i < m; i++) {
                for (int p = A_ptr[i]; p < A_ptr[i + 1]; p++) {
                    int j = A_idx[p];
                    for (int l = X_ptr[j]; l < X_ptr[j + 1]; l++) {
                        int n = X_idx[l];
                        Y_val[i][n] += A_val[p] * X_val[l];
                    }
                }
            }
        }
    );

    std::filesystem::create_directory(fs::path(params.output)/"Y.bspnpy");
    json y_desc;
    y_desc["version"] = 0.5;
    y_desc["format"] = "CSR";
    y_desc["shape"] = {m, d};
    y_desc["nnz"] = m*d;
    y_desc["data_types"]["values_type"] = "float64";
    std::ofstream y_desc_file(fs::path(params.output)/"Y.bspnpy"/"binsparse.json");
    y_desc_file << y_desc;
    y_desc_file.close();

    npy_store_vector<T>(fs::path(params.output)/"Y.bspnpy"/"values.npy", Y_val);

    json measurements;
    measurements["time"] = time;
    measurements["memory"] = 0;
    std::ofstream measurements_file(fs::path(params.output)/"measurements.json");
    measurements_file << measurements;
    measurements_file.close();



}

