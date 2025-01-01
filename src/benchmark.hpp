#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include "npy.hpp"
#include "json.hpp"

using json = nlohmann::json;

#define TIME_MAX 5.0
#define TRIAL_MAX 10000

/*
  Benchmark a function `run` by running it multiple times and measuring the
  time. The function `setup` is called before each run to prepare the input
  data. The function returns the minimum time in nanoseconds of all the runs.
  Runs at most `TRIAL_MAX` times or until the total time exceeds `TIME_MAX`.
*/
template <typename Setup, typename Run>
long long benchmark(Setup setup, Run run){
  auto time_total = std::chrono::high_resolution_clock::duration(0);
  auto time_min = std::chrono::high_resolution_clock::duration(0);
  int trial = 0;
  while(trial < TRIAL_MAX){
    setup();
    auto tic = std::chrono::high_resolution_clock::now();
    run();
    auto toc = std::chrono::high_resolution_clock::now();
    if(toc < tic){
      exit(EXIT_FAILURE);
    }
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(toc-tic);
    trial++;
    if(trial == 1 || time < time_min){
      time_min = time;
    }
    time_total += time;
    if(time_total.count() * 1e-9 > TIME_MAX){
      break;
    }
  }
  return (long long) time_min.count();
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

void experiment(std::string input, std::string output, int verbose);

struct benchmark_params_t {
  std::string input;
  std::string output;
  bool verbose;
  int argc;
  char **argv;
};

benchmark_params_t parse(int argc, char **argv) {
  // Define the long options
  static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"input", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
    {"verbose", no_argument, 0, 'v'},
    {0, 0, 0, 0}
  };

  // Parse the options
  int option_index = 0;
  int c;
  benchmark_params_t params;
  params.verbose = false;
  while ((c = getopt_long(argc, argv, "hi:o:v", long_options, &option_index)) != -1) {
    switch (c) {
      case 'h':
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help      Print this help message" << std::endl;
        std::cout << "  -i, --input     Specify the path for the inputs" << std::endl;
        std::cout << "  -o, --output    Specify the path for the outputs" << std::endl;
        std::cout << "  -v, --verbose   Print verbose output" << std::endl;
        std::cout << "  --              Kernel-specific arguments" << std::endl;
        exit(0);
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
        break;
      default:
        abort();
    }
  }

  // Check that all required options are present
  if (params.input.empty() || params.output.empty()) {
    std::cerr << "Missing required option" << std::endl;
    exit(1);
  }

  // Print verbose output if requested
  if (params.verbose) {
    std::cout << "Input path: " << params.input << std::endl;
    std::cout << "Output path: " << params.output << std::endl;
  }

  // Store the remaining command-line arguments
  params.argc = argc - optind + 1;
  params.argv = argv + optind - 1;

  return params;
}
