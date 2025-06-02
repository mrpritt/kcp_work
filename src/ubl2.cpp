#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "kpc.hpp"
#include "utils.hpp"
#include "model.hpp"

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

void create_cliques_greedy_firstfit(int n, Partition &cliques,
                                    const AdjMatrix &adj, int i0 = 0) {
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

vector<vector<int>> coniglio_decreasing(KnapsackData data) {
  vector<int> idx(data.n, 0);
  iota(idx.begin(), idx.end(), 0);
  sort(idx.begin(), idx.end(), [&](int i, int j) {
    return data.p[i] * data.w[j] > data.p[j] * data.w[i];
  });
  data = arrange_data(idx, data);

  vector<vector<bool>> adj(data.n, vector<bool>(data.n, false));
  for (auto p : data.pairs) {
    adj[p.first - 1][p.second - 1] = true;
    adj[p.second - 1][p.first - 1] = true;
  }

  vector<vector<int>> UB_L2(data.n);

  Partition cliques;
  create_cliques_coniglio(data.n, cliques, adj);

  for (int j = 0; j < data.n; j++) {
    Partition P;
    for (const auto &C : cliques) {
      vector<int> C_;
      for (auto e : C)
        if (e >= j)
          C_.push_back(e);
      if (C_.size() > 0)
        P.push_back(C_);
    }

    vector<int> prev(data.W + 1, 0);
    vector<int> curr(data.W + 1, 0);

    for (int l = 0; l < P.size(); l++) {
      for (int s = 0; s <= data.W; s++) {
        int max_c = prev[s];
        for (auto e : P[l]) {
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

  return UB_L2;

}

// Conglio reverse profit-over-weight
vector<vector<int>> coniglio_increasing(KnapsackData data) {
  vector<int> idx(data.n, 0);
  iota(idx.begin(), idx.end(), 0);
  sort(idx.begin(), idx.end(), [&](int i, int j) {
    return data.p[i] * data.w[j] < data.p[j] * data.w[i];
  });
  data = arrange_data(idx, data);

  vector<vector<bool>> adj(data.n, vector<bool>(data.n, false));
  for (auto p : data.pairs) {
    adj[p.first - 1][p.second - 1] = true;
    adj[p.second - 1][p.first - 1] = true;
  }

  vector<vector<int>> UB_L2(data.n);

  Partition cliques;
  create_cliques_coniglio(data.n, cliques, adj);

  for (int j = 0; j < data.n; j++) {
    Partition P;
    for (const auto &C : cliques) {
      vector<int> C_;
      for (auto e : C)
        if (e >= j)
          C_.push_back(e);
      if (C_.size() > 0)
        P.push_back(C_);
    }

    vector<int> prev(data.W + 1, 0);
    vector<int> curr(data.W + 1, 0);

    for (int l = 0; l < P.size(); l++) {
      for (int s = 0; s <= data.W; s++) {
        int max_c = prev[s];
        for (auto e : P[l]) {
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

  return UB_L2;

}

vector<vector<int>> firstfit_decreasing(KnapsackData data) {
  vector<int> idx(data.n, 0);
  iota(idx.begin(), idx.end(), 0);
  sort(idx.begin(), idx.end(), [&](int i, int j) {
    return data.p[i] * data.w[j] > data.p[j] * data.w[i];
  });
  data = arrange_data(idx, data);

  vector<vector<bool>> adj(data.n, vector<bool>(data.n, false));
  for (auto p : data.pairs) {
    adj[p.first - 1][p.second - 1] = true;
    adj[p.second - 1][p.first - 1] = true;
  }

  vector<vector<int>> UB_L2(data.n);

  Partition cliques;
  create_cliques_greedy_firstfit(data.n, cliques, adj);

  for (int j = 0; j < data.n; j++) {
    Partition P;
    for (const auto &C : cliques) {
      vector<int> C_;
      for (auto e : C)
        if (e >= j)
          C_.push_back(e);
      if (C_.size() > 0)
        P.push_back(C_);
    }

    vector<int> prev(data.W + 1, 0);
    vector<int> curr(data.W + 1, 0);

    for (int l = 0; l < P.size(); l++) {
      for (int s = 0; s <= data.W; s++) {
        int max_c = prev[s];
        for (auto e : P[l]) {
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

  return UB_L2;

}

vector<vector<int>> firstfit_increasing(KnapsackData data) {
  vector<int> idx(data.n, 0);
  iota(idx.begin(), idx.end(), 0);
  sort(idx.begin(), idx.end(), [&](int i, int j) {
    return data.p[i] * data.w[j] < data.p[j] * data.w[i];
  });
  data = arrange_data(idx, data);

  vector<vector<bool>> adj(data.n, vector<bool>(data.n, false));
  for (auto p : data.pairs) {
    adj[p.first - 1][p.second - 1] = true;
    adj[p.second - 1][p.first - 1] = true;
  }

  vector<vector<int>> UB_L2(data.n);

  Partition cliques;
  create_cliques_greedy_firstfit(data.n, cliques, adj);

  for (int j = 0; j < data.n; j++) {
    Partition P;
    for (const auto &C : cliques) {
      vector<int> C_;
      for (auto e : C)
        if (e >= j)
          C_.push_back(e);
      if (C_.size() > 0)
        P.push_back(C_);
    }

    vector<int> prev(data.W + 1, 0);
    vector<int> curr(data.W + 1, 0);

    for (int l = 0; l < P.size(); l++) {
      for (int s = 0; s <= data.W; s++) {
        int max_c = prev[s];
        for (auto e : P[l]) {
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

  return UB_L2;

}

vector<vector<int>> mincut(KnapsackData data) {
  auto vv = pairs2vv(data.n, data.pairs);
  double alpha = -1.0;
  vector<int> idx(data.n, 0);
  iota(idx.begin(), idx.end(), 0);
  sort(idx.begin(), idx.end(), [&](int i, int j) {
      return ((double)data.p[i]/data.w[i]) + alpha * ((double)vv[i].size()/data.n) > ((double)data.p[j]/data.w[j]) + alpha * ((double)vv[j].size()/data.n);
  });
  data = arrange_data(idx, data);

  vector<vector<bool>> adj(data.n, vector<bool>(data.n, false));
  for (auto p : data.pairs) {
    adj[p.first - 1][p.second - 1] = true;
    adj[p.second - 1][p.first - 1] = true;
  }

  vector<vector<int>> UB_L2(data.n);

  Partition cliques;
  create_cliques_greedy_firstfit(data.n, cliques, adj);

  // Partition cliques;
  // MCC model(data, 60);
  // auto [mcc_v, mcc_x, mcc_status] = model.solve();
  // cout << mcc_v << endl;
  // vector<bool> added(data.n, false);
  // for (int i = 0; i < data.n; i++) {
  //   if (!added[i]) {
  //     cliques.push_back(vector<int>());
  //     added[i] = true;
  //     cliques[cliques.size() - 1].push_back(i);
  //     for (int j = i + 1; j < data.n; j++) {
  //       if (mcc_x[i*data.n + j]) {
  //         cliques[cliques.size() - 1].push_back(j);
  //         added[j] = true;
  //       }
  //     }
  //   }
  // }

  for (int j = 0; j < data.n; j++) {
    Partition P;
    for (const auto &C : cliques) {
      vector<int> C_;
      for (auto e : C)
        if (e >= j)
          C_.push_back(e);
      if (C_.size() > 0)
        P.push_back(C_);
    }

    vector<int> prev(data.W + 1, 0);
    vector<int> curr(data.W + 1, 0);

    for (int l = 0; l < P.size(); l++) {
      for (int s = 0; s <= data.W; s++) {
        int max_c = prev[s];
        for (auto e : P[l]) {
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

  return UB_L2;

}

int main(int argc, char **argv) {

  string filename = argv[1];
  KnapsackData data = parseAMPLFile(filename);

  // Upper bounds:
  // * UBL3 = max(UBL2[j2][cV - wj1] + pj1, UBL2[j2][cV])
  // * UBL4 = min(UBL2, UBL3)
  // * UBL5 : \hat{V}' = \hat{V} - N(j), UBL5 = UBL3(\hat{V}')
  // * UBL6 = min(UBL2, UBL5)
  // double d = (double)data.pairs.size() / ((double)(data.n * (data.n - 1)) / 2);

  // auto UB_L2_CR = coniglio_reverse(data);
  // auto UB_L2_CS = coniglio_standard(data);
  // auto UB_L2_FFR = firstfit_reverse(data);
  // auto UB_L2_FFS = firstfit_standard(data);
  auto UB_L2_MINCUT = mincut(data);

  // ILP2 kpc_model(data);
  // auto [kpc_v, kpc_x, kpc_status] = kpc_model.solve();

  cout << UB_L2_MINCUT[0][data.W] << endl;



  // // \hat{V} = I^*
  // ILP2 kpc_model(data);
  // auto [kpc_v, kpc_x, kpc_status] = kpc_model.solve();
  // int a1 = 0;
  // while (kpc_x[a1] != 1 && a1 < kpc_x.size()) a1+=1;
  // if (a1 == kpc_x.size()) {
  //   // Then just to \hat{V} = V
  //   a1 = 0;
  // }
  // int a2 = a1;
  // while (kpc_x[a2] != 1 && a2 < kpc_x.size()) a2+=1;
  // if (a2 == kpc_x.size()) {
  //   a2 = (a1 < kpc_x.size() - 1)? a1 + 1 : a1;
  // }
  //
  // int ub_l2_i = UB_L2[a1][data.W];
  // int ub_l3_i =
  //     max(UB_L2[a2][data.W],
  //         data.w[a1] <= data.W ? UB_L2[a2][data.W - data.w[a1]] + data.p[a1] : 0);
  // int ub_l4_i = min(ub_l2_i, ub_l3_i);
  // // Get first non-neighbour
  // int a2l = a2;
  // while (adj[a1][a2l] && a2l < data.n)
  //   a2l += 1;
  // if (a2l == data.n) {
  //   a2l = a2;
  // }
  // int ub_l5_i =
  //     max(UB_L2[a2l][data.W],
  //         data.w[a1] <= data.W ? UB_L2[a2l][data.W - data.w[a1]] + data.p[a1] : 0);
  // int ub_l6_i = min(ub_l2_i, ub_l5_i);
  //
  // cout << kpc_v << "," << ub_l2_i << "," << ub_l3_i << "," << ub_l4_i << "," << ub_l5_i << ","
  //      << ub_l6_i << endl ;



  return 0;
}
