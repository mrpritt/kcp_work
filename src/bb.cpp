#include "kpc.hpp"
#include "utils.hpp"
#include <algorithm>
#include <numeric>
#include <string>
using namespace std;

using AdjMatrix = vector<vector<bool>>;
using Partition = vector<vector<int>>;
using Item = int;
using weight_t = int;
using profit_t = int;
using Items = vector<int>;
using Profits = vector<profit_t>;
using Weights = vector<weight_t>;

typedef struct {
  Profits p;
  Weights w;
  weight_t W;
  AdjMatrix adj;
} KPData;

Items I_inc;
profit_t LB = 0;

profit_t profit(const Items &I) { return 0; }
weight_t weight(const Items &I) { return 0; }

Items partition(Items v_hat, const Items &I, const KPData &data) {
  profit_t budget = LB - profit(I);
  Items B(v_hat.size());

  while (!v_hat.empty()) {
    auto j = v_hat.pop_back();
  }

  return B;
}

Items branch_and_bound(Items v_hat, const KPData &data) {
  weight_t c_v_hat = data.W - weight(I_inc);

  Items I;

  Items B = partition(v_hat, I, data);

  while (!v_hat.empty()) {
    Item v = v_hat.back();
    v_hat.pop_back();
  }

  return I_inc;
}

int main(int argc, char **argv) {

  string filename = argv[1];
  KnapsackData data = parseAMPLFile(filename);

  // (0) Internal representation
  vector<int> idx(data.n, 0);
  iota(idx.begin(), idx.end(), 0);
  sort(idx.begin(), idx.end(), [&](int i, int j) {
    return data.p[i] * data.w[j] > data.p[j] * data.w[i];
  });
  data = arrange_data(idx, data);
  AdjMatrix adj(data.n, vector<bool>(data.n, false));
  for (auto p : data.pairs) {
    adj[p.first - 1][p.second - 1] = true;
    adj[p.second - 1][p.first - 1] = true;
  }
  KPData i = {data.p, data.w, data.W, adj};

  // (1) Preprocessing

  // (2) Heuristic initial solution

  // (3) Bounds table generation

  // (4) Branch & Bound
  vector<int> v_hat(data.n, 0);
  iota(idx.begin(), idx.end(), 0);
  // TODO: make compatible with `I` from heuristic solution

  return 0;
}
