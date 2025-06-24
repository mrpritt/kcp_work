#include "kpc.hpp"
#include "utils.hpp"
#include <algorithm>
#include <numeric>
#include <string>

#include "bitscan.h"

using namespace std;

using AdjMatrix = vector<vector<bool>>;
using Partition = vector<vector<int>>;
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

// typedef struct {
//   bitset I;
//   profit_t p;
//   weight_t w;
//   bitset C;
// } Node;

// PARTITION(n)
// should_stop = true     # should stop if no items in CC fits in P
// CC = n.C               # copy of candidates
// UB_p = 0;              # partition upper bound up until previous clique
// Pi_s = 0;              # current clique max profit => UB_p + Pi_s = UB_P
// C = 0                  # current clique being built
// for i in CC {
//    if (n.w + w[i] <= c) {
//        pi_s = max(Pi_s, p[i])
//        if (C & N[i] == C && 
//            LB - n.p >= UB_p + pi_s) { # if i forms clique and budget okay
//            should_stop = false;
//            Pi_s = pi_s;
//            CC[i] = 0;
//            C[i] = 1;
//        }
//    } else {        # remove from all successor nodes if item does not fit
//        CC[i] = 0;
//        n.C = 0;
//    }
// }
// while (!should_stop && CC > 0) {
//    UB_p += Pi_s;
//    C = 0;
//    Pi_s = 0;
//    should_stop = true;
//    for i in CC {
//        pi_s = max(Pi_s, p[i])
//        if (C & N[i] == C && 
//            LB - n.p >= UB_p + pi_s) { # if i forms clique and budget okay
//            should_stop = false;
//            Pi_s = pi_s;
//            CC[i] = 0;
//            C[i] = 1;
//        }
//    }
// }
// # Now CC is the branching set
//

// BRANCH_AND_BOUND(data)
// # create root node
// Q = {r}
// while (!Q.empty()) {
//    n = Q.front()
//    Q.pop()
//    if (!should_cut(n)) {
//        B = partition(n);
//        for i in B {
//            Q.push(add_item(n, i));
//        }
//    }
// }

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
