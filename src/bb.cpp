#include <algorithm>
#include <numeric>
#include <string>
#include "kpc.hpp"
#include "utils.hpp"
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

Items I;

Items branch_and_bound(Items v_hat, const KPData& data) {
  weight_t c_v_hat = data.W - weight(I);

  while (!v_hat.empty()) {
    Item v = v_hat.back();
    v_hat.pop_back();

  }

  return I;
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
