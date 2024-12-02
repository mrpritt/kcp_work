#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <numeric>
using namespace std;

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include "kpc.hpp"
#include "model.hpp"
#include "kpc_dp.hpp"
#include "kpc_dp2.hpp"
#include "utils.hpp"
#include "timer.hpp"

void solver_cmp(KnapsackData data) {
  try {
    KPModel kp_model;
    KPCModel kpc_model;
    LPModel lp_model;
    WMISModel wmis_model;

    auto [kp_S, kp_V] = kp_model.run(data);
    auto [kpc_S, kpc_V] = kpc_model.run(data);
    auto [lp_S, lp_V] = lp_model.run(data);
    auto [wmis_S, wmis_V] = wmis_model.run(data);

    fmt::print("KP: {}\n", kp_V);
    print_solution(kp_S, data);
    fmt::print("KPC: {}\n", kpc_V);
    print_solution(kpc_S, data);
    fmt::print("WMIS: {}\n", wmis_V);
    print_solution(wmis_S, data);

    // Order by efficiency
    vector<int> π(data.n, 0);
    iota(π.begin(), π.end(), 0);
    sort(π.begin(), π.end(), [&](int i, int j) { return data.p[i] * data.w[j] > data.p[j] * data.w[i]; });

    fmt::print("LP: {}\n", lp_V);
    for(auto i=0u; i != lp_S.size(); ++i) {
        fmt::print("{:.2f} {} | ", lp_S[π[i]], kpc_S[π[i]] ? "■" : ".");
        if ((i + 1) % 10 == 0)
          fmt::print("\n");
    }
    fmt::print("\n");

  } catch (IloCplex::Exception& e) {
    cerr << "CPLEX exception caught: " << e.getMessage() << endl;
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    cerr << "Error: filename is required\n";
    cerr << "Usage: kpc <filename>\n";
    return 1;
  }

  string filename = argv[1];
  KnapsackData data = read_knapsack_data(filename);

  // heuristic linear arrangement to minimize conflict distances
  auto conflicts = pairs2vv(data.n, data.pairs);
  auto [linarr, J] = linear_arrangement(conflicts);
  cout << "Linear arrangement:" << endl;
  for (int i : linarr)
       cout << i << " ";
  cout << "\n" << endl;
  auto arrd_data = arrange_data(linarr, data);

  // Run solver comparison
  // solver_cmp(data);

  KPCModel kpc_model;
  auto [kpc_S, kpc_V] = kpc_model.run(data);
  print_solution(kpc_S, data);

  // Run DP Algorithm
  // auto [dp_V, dp_S] = knapsackWithConflicts(arrd_data);
  // cout << "KPC_DParrd: " << dp_V << endl;
  // print_solution(dp_S, arrd_data);
  // auto [dp_V2, dp_S2] = knapsackWithConflicts(data);
  // cout << "KPC_DP: " << dp_V2 << endl;

  // DP with sliding window
  // fmt::print("J\tnup\tv_orig\tv_fact\tv_imp\timp*_diff\timp_fact_diff\tc_size\tXOR_f\ttime\n");
  // for(auto j = 1; j <= J; ++j) {
  //   timer t;
  //   auto [v_orig, S_orig, stats_orig] = KPC_DP(arrd_data, j);
  //   auto [c_items, c_data] = extract_conflicts(S_orig, arrd_data);
  //   auto [c_V, c_S] = knapsackWithConflicts(c_data);
  //   auto S_fact = combine_solutions(S_orig, c_S, c_items);
  //   int v_fact = solution_value(S_fact, arrd_data);
  //   auto S_imp = greedy_improvement(S_fact, arrd_data);
  //   int v_imp = solution_value(S_imp, arrd_data);
  //
  //   fmt::print("{}\t{}\t{}\t{}\t{}\t{:+.5f}\t{:+.5f}\t{}\t{}\t{:.5f}\n", j, stats_orig.nup, v_orig, v_fact, v_imp, (double) (v_imp - kpc_V)/kpc_V, (double) (v_imp - v_fact)/v_fact, c_items.size(), countXOR(S_fact, kpc_S), t.elapsed());
  // }

  return 0;
}
