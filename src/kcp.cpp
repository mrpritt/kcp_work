#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <numeric>
using namespace std;

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include "kcp.hpp"
#include "model.hpp"
#include "kpc_dp.hpp"
#include "kpc_dp2.hpp"

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
    for(auto i=0u; i != kp_S.size(); ++i)
      fmt::print("{}", kp_S[π[i]] ? "■" : ".");
    fmt::print("\n");
    fmt::print("KPC: {}\n", kpc_V);
    for(auto i=0u; i != kpc_S.size(); ++i)
      fmt::print("{}", kpc_S[π[i]] ? "■" : ".");
    fmt::print("\n");
    fmt::print("WMIS: {}\n", wmis_V);
    for(auto i=0u; i != wmis_S.size(); ++i)
      fmt::print("{}", wmis_S[π[i]] ? "■" : ".");
    fmt::print("\n");
    fmt::print("LP: {}\n", lp_V);
    for(auto i=0u; i != lp_x.size(); ++i) {
        fmt::print("{:.2f} {} | ", lp_x[π[i]], kpc_S[π[i]] ? "■" : ".");
        if ((i + 1) % 10 == 0)
          fmt::print("\n");
    }
    fmt::print("\n");

    // Find a and b
    uint a = 0;
    uint b = 0;
    for (auto i=0u; i != kp_S.size(); ++i) {
      if (kp_S[π[i]])
        b = i;
      if (kp_S[π[i]] && a == i)
        a++;
    }
    cout << "Core KP: " << a << " " << b << endl;

  } catch (IloCplex::Exception& e) {
    cerr << "CPLEX exception caught: " << e.getMessage() << endl;
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    cerr << "Error: filename is required\n";
    cerr << "Usage: kcp <filename>\n";
    return 1;
  }

  string filename = argv[1];
  KnapsackData data = read_knapsack_data(filename);

  // Run solver
  solver_cmp(data);

  // Run DP Algorithm
  // TODO: knapsackWithConflicts() is broken, fix it!
  //
  auto [dp_V, dp_S] = knapsackWithConflicts(data);
  cout << dp_V << endl;

  auto [kpcdp_V, kpcdp_S] = KPC_DP(data);
  cout << kpcdp_V << endl;

  return 0;
}
