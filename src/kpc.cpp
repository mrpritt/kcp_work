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

KnapsackData read_knapsack_data(const string& filename) {
    KnapsackData data;
    ifstream in(filename);
    string line;

    if (in.is_open()) {
        in >> data.n >> data.I >> data.W;

        data.p.resize(data.n);
        for (int i = 0; i < data.n; ++i)
	  in >> data.p[i];

        data.w.resize(data.n);
        for (int i = 0; i < data.n; ++i)
	  in >> data.w[i];

        data.pairs.resize(data.I);
        for (int i = 0; i < data.I; ++i)
	  in >> data.pairs[i].first >> data.pairs[i].second;

        in.close();
    } else {
        cerr << "Unable to open file: " << filename << endl;
    }

    return data;
}

void print_solution(const vector<bool>& s, const KnapsackData& data) {
  // Order by efficiency
  vector<int> π(data.n, 0);
  iota(π.begin(), π.end(), 0);
  sort(π.begin(), π.end(), [&](int i, int j) { return data.p[i] * data.w[j] > data.p[j] * data.w[i]; });

  for(auto i=0u; i != s.size(); ++i)
    fmt::print("{}", s[π[i]] ? "■" : ".");
  fmt::print("\n");
}

void solver_cmp(KnapsackData data) {
  try {
    KPModel kp_model;
    KPCModel kpc_model;
    LPModel lp_model;
    WMISModel wmis_model;
    kp_model.build(data);
    kpc_model.build(data);
    lp_model.build(data);
    wmis_model.build(data);
    auto [kp_x,kp_status] = kp_model.solve();
    auto [kpc_x,kpc_status] = kpc_model.solve();
    auto [lp_x,lp_status] = lp_model.solve();
    auto [wmis_x,wmis_status] = wmis_model.solve();

    // Order by efficiency
    vector<int> π(data.n, 0);
    iota(π.begin(), π.end(), 0);
    sort(π.begin(), π.end(), [&](int i, int j) { return data.p[i] * data.w[j] > data.p[j] * data.w[i]; });

    auto [kp_S, kp_V] = kp_model.getSolution(kp_x);
    auto [kpc_S, kpc_V] = kpc_model.getSolution(kpc_x);
    auto [wmis_S, wmis_V] = wmis_model.getSolution(wmis_x);
    auto [lp_S, lp_V] = lp_model.getSolution(lp_x);

    fmt::print("KP: {}\n", kp_V);
    print_solution(kp_S, data);
    fmt::print("KPC: {}\n", kpc_V);
    print_solution(kpc_S, data);
    fmt::print("WMIS: {}\n", wmis_V);
    print_solution(wmis_S, data);

    fmt::print("LP: {}\n", lp_V);
    for(auto i=0u; i != lp_x.size(); ++i) {
        fmt::print("{:.2f} {} | ", lp_x[π[i]], kpc_S[π[i]] ? "■" : ".");
        if ((i + 1) % 10 == 0)
          fmt::print("\n");
    }
    fmt::print("\n");

    // Find a and b
    // uint a = 0;
    // uint b = 0;
    // for (auto i=0u; i != kp_S.size(); ++i) {
    //   if (kp_S[π[i]])
    //     b = i;
    //   if (kp_S[π[i]] && a == i)
    //     a++;
    // }
    // cout << "Core KP: " << a << " " << b << endl;

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

  // Run solver
  solver_cmp(data);

  // Run DP Algorithm
  auto [dp_V, dp_S] = knapsackWithConflicts(arrd_data);
  cout << "KPC_DParrd: " << dp_V << endl;
  print_solution(dp_S, arrd_data);
  auto [dp_V2, dp_S2] = knapsackWithConflicts(data);
  cout << "KPC_DP: " << dp_V2 << endl;

  // DP with sliding window
  for(auto j = 1; j != J + 1; ++j) {
    auto [kpcdp_V, kpcdp_S, nup, tim] = KPC_DP(arrd_data, j);
    auto inv_linarr = inv_arr(linarr);
    kpcdp_S = arrange_arr(kpcdp_S, inv_linarr);
    fmt::print("{}\t{}\t{}\t{}\n", j, kpcdp_V, nup, tim);
    // print_solution(kpcdp_S, data);
  }

  return 0;
}
