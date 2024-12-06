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
#include "kpc_dp.hpp"
#include "kpc_dp2.hpp"
#include "model.hpp"
#include "solution.hpp"
#include "timer.hpp"
#include "utils.hpp"
#include "heuristic.hpp"

void solver_cmp(KnapsackData data) {
  try {
    KPModel kp_model(data);
    KPCModel kpc_model(data);
    LPKPCModel lp_model(data);
    MWISModel wmis_model(data);

    auto [kp_v, kp_x, kp_status] = kp_model.solve();
    auto [kpc_v, kpc_x, kpc_status] = kpc_model.solve();
    auto [lp_v, lp_x, lp_status] = lp_model.solve();
    auto [mwis_v, mwis_x, wmis_status] = wmis_model.solve();

    Solution kp_s(kp_x, data);
    Solution kpc_s(kpc_x, data);
    Solution mwis_s(mwis_x,data);

    for (auto i : kpc_s.compatibleItems())
      fmt::print("{} ", i);
    fmt::print("\n");

    fmt::print("KP: {}\n", kp_s.value());
    kp_s.print();
    fmt::print("KPC: {}\n", kpc_s.value());
    kpc_s.print();
    fmt::print("WMIS: {}\n", mwis_s.value());
    mwis_s.print();

    // Order by efficiency
    vector<int> π(data.n, 0);
    iota(π.begin(), π.end(), 0);
    sort(π.begin(), π.end(), [&](int i, int j) {
      return data.p[i] * data.w[j] > data.p[j] * data.w[i];
    });

    fmt::print("LP: {}\n", lp_v);
    for (auto i = 0u; i != lp_x.size(); ++i) {
      fmt::print("{:.2f} {} | ", lp_x[π[i]], kpc_s[π[i]] ? "■" : ".");
      if ((i + 1) % 10 == 0)
        fmt::print("\n");
    }
    fmt::print("\n");

  } catch (IloCplex::Exception &e) {
    cerr << "CPLEX exception caught: " << e.getMessage() << endl;
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "Error: filename is required\n";
    cerr << "Usage: kpc <filename>\n";
    return 1;
  }

  string filename = argv[1];
  KnapsackData data = read_knapsack_data(filename);

  // heuristic linear arrangement to minimize conflict distances
  // auto conflicts = pairs2vv(data.n, data.pairs);
  // auto [linarr, J] = linear_arrangement(conflicts);
  // cout << "Linear arrangement:" << endl;
  // for (int i : linarr)
  //   cout << i << " ";
  // cout << "\n" << endl;
  // auto arrd_data = arrange_data(linarr, data);

  // Run solver comparison
  // solver_cmp(data);

  timer t_solver;
  KPCModel kpc_model(data);
  auto [kpc_v, kpc_x, kpc_status] = kpc_model.solve();
  Solution kpc_s(kpc_x, data);
  fmt::print("KPC: {}\ttime: {}\n", kpc_v, t_solver.elapsed());
  kpc_s.print();

  timer t_left;
  Solution sl = left_heuristic(data);
  fmt::print("HL: {}\ttime: {}\n", sl.value(), t_left.elapsed());
  sl.print();

  timer t_right;
  Solution sr = right_heuristic(data);
  fmt::print("HR: {}\ttime: {}\n", sr.value(), t_right.elapsed());
  sr.print();

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
  //   fmt::print("{}\t{}\t{}\t{}\t{}\t{:+.5f}\t{:+.5f}\t{}\t{}\t{:.5f}\n", j,
  //   stats_orig.nup, v_orig, v_fact, v_imp, (double) (v_imp - kpc_V)/kpc_V,
  //   (double) (v_imp - v_fact)/v_fact, c_items.size(), countXOR(S_fact,
  //   kpc_S), t.elapsed());
  // }

  return 0;
}
