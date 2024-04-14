#include <iostream>
#include <chrono>
#include <vector>
#include <getopt.h>
#include "npy.hpp"
#include "json.hpp"

using json = nlohmann::json;

#define TIME_MAX 5.0
#define TRIAL_MAX 10000

template <typename Setup, typename Run>
long long benchmark(Setup setup, Run run) {
    auto time_total = std::chrono::high_resolution_clock::duration(0);
    auto time_min = std::chrono::high_resolution_clock::duration(0);
    int trial = 0;
    while (trial < TRIAL_MAX) {
        setup();
        auto tic = std::chrono::high_resolution_clock::now();
        run();
        auto toc = std::chrono::high_resolution_clock::now();
        if (toc < tic) {
            exit(EXIT_FAILURE);
        }
        auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(toc - tic);
        trial++;
        if (trial == 1 || time < time_min) {
            time_min = time;
        }
        time_total += time;
        if (time_total.count() * 1e-9 > TIME_MAX) {
            break;
        }
    }
    return static_cast<long long>(time_min.count());
}

template <typename T>
std::vector<T> npy_load_vector(std::string fname) {
    std::vector<T> vec;
    std::vector<unsigned long> shape;
    bool fortran_order;
    npy::LoadArrayFromNumpy<T>(fname, shape, fortran_order, vec);
    return vec;
}

template <typename T>
void npy_store_vector(std::string fname, std::vector<T> vec) {
    std::vector<unsigned long> shape = {vec.size(),};
    npy::SaveArrayAsNumpy(fname, false, shape.size(), shape.data(), vec);
}

struct BenchmarkParams {
    std::string input;
    std::string output;
    bool verbose;
};

BenchmarkParams parse(int argc, char **argv) {
    const char *optstring = "hi:o:v";
    const option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"input", required_argument, nullptr, 'i'},
        {"output", required_argument, nullptr, 'o'},
        {"verbose", no_argument, nullptr, 'v'},
        {nullptr, 0, nullptr, 0}
    };

    BenchmarkParams params;
    params.verbose = false;
    int c;
    while ((c = getopt_long(argc, argv, optstring, long_options, nullptr)) != -1) {
        switch (c) {
            case 'h':
                std::cout << "Options:" << std::endl;
                std::cout << "  -h, --help      Print this help message" << std::endl;
                std::cout << "  -i, --input     Specify the path for the inputs" << std::endl;
                std::cout << "  -o, --output    Specify the path for the outputs" << std::endl;
                std::cout << "  -v, --verbose   Print verbose output" << std::endl;
                std::cout << "  --              Kernel-specific arguments" << std::endl;
                exit(EXIT_SUCCESS);
            case 'i':
                params.input = optarg;
                break;
            case 'o':
                params.output = optarg;
                break;
            case 'v':
                params.verbose = true;
                break;
            case '?':
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }

    if (params.input.empty() || params.output.empty()) {
        std::cerr << "Missing required option" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (params.verbose) {
        std::cout << "Input path: " << params.input << std::endl;
        std::cout << "Output path: " << params.output << std::endl;
    }

    return params;
}

void experiment(const std::string& input, const std::string& output, bool verbose) {
    // Your experiment implementation goes here
}

int main(int argc, char **argv) {
    BenchmarkParams params = parse(argc, argv);
    experiment(params.input, params.output, params.verbose);
    return EXIT_SUCCESS;
}
