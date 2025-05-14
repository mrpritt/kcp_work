#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <cassert>

#include "kpc.hpp"
#include "utils.hpp"

using namespace std;

using AdjMatrix = vector<vector<bool>>;

// Check if `b` is adjacent to all vertices in `as`
bool is_clique(const AdjMatrix &adj, const vector<int> &as, int b) {
  for (auto a : as) {
    if (!adj[a][b])
      return false;
  }
  return true;
}

using Partition = vector<vector<int>>;

void create_cliques_greedy_firstfit(int n, Partition &cliques, const AdjMatrix &adj, int i0 = 0) {
  // assert: cliques is empty
  for (int i = i0; i < n; ++i) {
    bool found = false;
    for (auto &c : cliques) {
      if (is_clique(adj, c, i)) {
        c.push_back(i);
        found = true;
        break;
      }
    }
    if (!found)
      cliques.push_back(vector<int>(1, i));
  }
}

void create_cliques_coniglio(int n, Partition &cliques, const AdjMatrix &adj) {
  // assert: cliques is empty
  int nv = 0;
  vector<bool> free(n, true);

  while (nv < n) {
    vector<int> C; // current clique
    for (int i = 0; i < n; ++i) {
      if (!free[i])
        continue;

      if (is_clique(adj, C, i)) {
        C.push_back(i);
        free[i] = false;
      }
    }
    assert(C.size() > 0);
    nv += C.size();
    cliques.push_back(C);
  }
}

int main(int argc, char **argv) {

  string filename = argv[1];
  KnapsackData data = parseAMPLFile(filename);

  // Reorder in non-increasing efficiency
  vector<int> idx(data.n, 0);
  iota(idx.begin(), idx.end(), 0);
  sort(idx.begin(), idx.end(), [&](int i, int j) {
    return data.p[i] * data.w[j] < data.p[j] * data.w[i];
  });
  data = arrange_data(idx, data);

  // Create conflict matrix
  vector<vector<bool>> adj(data.n, vector<bool>(data.n, false));
  for (auto p : data.pairs) {
    adj[p.first - 1][p.second - 1] = true;
    adj[p.second - 1][p.first - 1] = true;
  }

  vector<vector<int>> UB_L2(data.n);

  // Greedy Clique Partition
  Partition cliques;
  create_cliques_coniglio(data.n, cliques, adj);

  for (int j = 0; j < data.n; j++) {
    // create P(V-hacek) from `cliques`
    Partition P;
    for(const auto & C : cliques) {
      vector<int> C_;
      for(auto e : C)
        if (e >= j)
          C_.push_back(e);
      if (C_.size() > 0)
        P.push_back(C_);
    }

    // DP MCKP
    vector<int> prev(data.W + 1, 0);
    vector<int> curr(data.W + 1, 0);

    for (int l = 0; l < P.size(); l++) {
      for (int s = 0; s <= data.W; s++) {
        int max_c = prev[s];
        for(auto e : P[l]) {
          const int wi = data.w[e];
          if (wi <= s)
            max_c = max(max_c, prev[s - wi] + data.p[e]);
        }
        curr[s] = max_c;
      }
      prev.swap(curr);
    }
    UB_L2[j] = prev;
  }

  int ub_l2_root = UB_L2[0][data.W];
  int ub_l2g_root =
      max(UB_L2[1][data.W],
          data.w[0] <= data.W ? UB_L2[1][data.W - data.w[0]] + data.p[0] : 0);

  cout << ub_l2_root << "," << ub_l2g_root << endl;

  return 0;
}
