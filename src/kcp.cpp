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

using namespace std;

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

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Error: filename is required\n";
        cerr << "Usage: kcp <filename>\n";
        return 1;
    }

    string filename = argv[1];
    KnapsackData data = read_knapsack_data(filename);

    try {
      Model model;
      model.build(data);
      auto [S,status] = model.solve();

      vector<int> π(data.n, 0);
      iota(π.begin(), π.end(), 0);
      sort(π.begin(), π.end(), [&](int i, int j) { return data.p[i] * data.w[j] > data.p[j] * data.w[i]; });
      for(auto i=0u; i != S.size(); ++i)
	fmt::print("{}", S[π[i]] ? "⬛" : ".");
      fmt::print("\n");
    } catch (IloCplex::Exception& e) {
        cerr << "CPLEX exception caught: " << e.getMessage() << endl;
    }

    return 0;
}
