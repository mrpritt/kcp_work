#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>
using namespace std;

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include "fmt/format.h"

#include "kpc.hpp"
#include "model.hpp"
#include "solution.hpp"
#include "timer.hpp"
#include "utils.hpp"

int main(int argc, char **argv) {
  if (argc < 3) {
    cerr << "Error: filename and type is required\n";
    cerr << "Usage: cplex_kpc <filename> <filetype>\n";
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

  timer t_solver;
  ILP2 kpc_model(data);
  auto [kpc_v, kpc_x, kpc_status] = kpc_model.solve();
  Solution kpc_s(kpc_x, data);
  fmt::print("KPC: {}\ttime: {}\n", kpc_v, t_solver.elapsed());
  kpc_s.print();

  return 0;
}

