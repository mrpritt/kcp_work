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
    cerr << "Usage: cplex_mwis <filename> <filetype>\n";
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
  MWISModel mwis_model(data);
  auto [mwis_v, mwis_x, mwis_status] = mwis_model.solve();
  Solution mwis_s(mwis_x, data);
  fmt::print("MWIS: {}\ttime: {}\n", mwis_v, t_solver.elapsed());
  mwis_s.print();

  return 0;
}
