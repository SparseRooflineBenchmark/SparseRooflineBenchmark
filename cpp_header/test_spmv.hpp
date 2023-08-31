#include <getopt.h>
#include <iostream>
#include <string>
#include "benchmark.hpp"

int main(int argc, char **argv) {
  // Define the long options
  static struct option long_options[] = {
    {"help", no_argument, &help, 1},
    {"file_A", required_argument, 0, 0},
    {"file_y", required_argument, 0, 0},
    {"file_x", required_argument, 0, 0},
    {"verbose", no_argument, &verbose, 1},
    {0, 0, 0, 0}
  };

  // Parse the options
  int option_index = 0;
  int c;
  while ((c = getopt_long(argc, argv, "A:y:x:v", long_options, &option_index)) != -1) {
    switch (c) {
      case 'A':
        file_A = optarg;
        break;
      case 'y':
        file_y = optarg;
        break;
      case 'x':
        file_x = optarg;
        break;
      case 'v':
        verbose = 1;
        break;
      case '?':
        // getopt_long already printed an error message
        break;
      default:
        abort();
    }
  }

  // Check for the correct number of arguments
  if (optind < argc) {
    std::cerr << "Unexpected argument: " << argv[optind] << std::endl;
    return 1;
  }

  // Print help message if requested
  if (help) {
    std::cout << "Usage: " << argv[0] << " [OPTIONS]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help          Print this help message" << std::endl;
    std::cout << "  --file_y FILE   Specify the file for output vector y" << std::endl;
    std::cout << "  --file_A FILE   Specify the file for input matrix A" << std::endl;
    std::cout << "  --file_x FILE   Specify the file for input vector x" << std::endl;
    std::cout << "  --verbose       Print verbose output" << std::endl;
    return 0;
  }

  // Check that all required options are present
  if (file_A.empty() || file_y.empty() || file_x.empty()) {
    std::cerr << "Missing required option" << std::endl;
    return 1;
  }

  // Open the HDF5 file
  H5::H5File file("filename.h5", H5F_ACC_RDONLY);
  // Open the dataset
  H5::DataSpace dataset = file.openDataSet("dataset_name").getspace();
  // Get the dataspace
  H5::DataSpace dataspace = dataset.getSpace();
  // Get the number of dimensions in the dataspace
  int ndims = dataspace.getSimpleExtentNdims();



  std::cout << time << std::endl;

  write(file_y, y);

  return 0;
}
