#include "kpc.hpp"
#include "utils.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>

// Check if `b` is adjacent to all vertices in `as`
bool cut1(const std::vector<std::vector<bool>>& adj, const std::vector<int>& as, int b) {
  for (auto a : as) {
    if (!adj[a][b])
      return false;
  }
  return true;
}

int main(int argc, char **argv) {

  std::string filename = argv[1];
  KnapsackData data = parseAMPLFile(filename);

  // Reorder in non-increasing efficiency
  std::vector<int> idx(data.n, 0);
  iota(idx.begin(), idx.end(), 0);
  sort(idx.begin(), idx.end(),
       [&](int i, int j) { return data.p[i] * data.w[j] > data.p[j] * data.w[i]; });
  data = arrange_data(idx, data);

  // Create conflict matrix
  std::vector<std::vector<bool>> adj(data.n, std::vector<bool>(data.n, false));
  for (auto p : data.pairs) {
    adj[p.first - 1][p.second - 1] = true;
    adj[p.second - 1][p.first - 1] = true;
  }

  std::vector<std::vector<int>> UB_L2(data.n);

  for (int j = 0; j < data.n; j++) {

    // Greedy Clique Partition
    std::vector<std::vector<int>> cliques;
    for (int i = j; i < data.n; ++i) {
      bool found = false;
      for (auto& c : cliques) {
        if (cut1(adj, c, i)) {
          c.push_back(i);
          found = true;
          break;
        } 
      }
      if (!found)
        cliques.push_back(std::vector<int>(1, i));
    }

    // DP MCKP
    std::vector<int> prev(data.W + 1, 0);
    std::vector<int> curr(data.W + 1, 0);

    for (int l = 0; l < cliques.size(); l++) {
      for (int s = 0; s <= data.W; s++) {
        int max_c = 0;
        for (int i = 0; i < cliques[l].size(); i++) {
          int wi = data.w[cliques[l][i]];
          int pi = data.p[cliques[l][i]];
          if (wi <= s) {
            max_c = std::max(max_c, prev[s - wi] + pi);
          }
        }
        curr[s] = std::max(max_c, prev[s]);

      }
      prev.swap(curr);
    }
    UB_L2[j] = prev;
  }


  int ub_l2_root = UB_L2[0][data.W];
  int ub_l2g_root = std::max(UB_L2[1][data.W], UB_L2[1][data.W - data.w[0]] + data.p[0]);

  std::cout << ub_l2_root << "," << ub_l2g_root << std::endl;

  return 0;
}
