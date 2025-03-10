#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>
using namespace std;

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include "kpc.hpp"
#include "solution.hpp"
#include "timer.hpp"
#include "utils.hpp"
#include "heuristic.hpp"

int main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "Error: filename is required\n";
    cerr << "Usage: hl <filename> <filetype>\n";
    cerr << "<filetype> = std | ampl\n";
    return 1;
  }

  string filename = argv[1];
  string filetype = argv[2];
  KnapsackData data;
  if (filetype == "ampl") {
    data = parseAMPLFile(filename);
  } else {
    data = read_knapsack_data(filename);
  }

  timer t_left;
  Solution sl = left_heuristic(data);
  fmt::print("HL: {}\ttime: {}\n", sl.value(), t_left.elapsed());
  sl.print();

  return 0;
}
