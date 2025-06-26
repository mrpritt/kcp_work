#include "bbtypes.h"
#include "kpc.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstdint>
#include <numeric>
#include <stack>
#include <string>

#include "bitscan.h"

/* NOTE:
 * bitarray.set_bit() sets all bits to 1, including the unused bits of the word
 */

using namespace std;

using Partition = vector<vector<int>>;
using weight_t = int;
using profit_t = int;
using Profits = vector<profit_t>;
using Weights = vector<weight_t>;

typedef struct {
  int n;
  Profits p;
  Weights w;
  weight_t W;
  vector<bitarray> N;
} KPData;

typedef struct {
  bitarray I;
  profit_t p;
  weight_t w;
  bitarray C;
} Node;

Node I_inc;
profit_t LB = 0;

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
bitarray partition(Node &n, const KPData &data) {
  // BUG: there is a bug here somewhere
  bool should_stop = true;
  bitarray CC(n.C);
  profit_t UB_p = 0;
  profit_t Pi_s = 0;
  bitarray C(data.n);
  bitarray tmp(data.n);

  CC.init_scan(bbo::NON_DESTRUCTIVE);
  int i = CC.next_bit();
  while (i != EMPTY_ELEM) {
    if (n.w + data.w[i] <= data.W) {
      int pi_s = max(Pi_s, data.p[i]);
      tmp = C;
      tmp &= data.N[i];
      // cout << "tmp = " << tmp << endl;
      // cout << "Ni = " << data.N[i] << endl;
      // cout << "C = " << C << endl;
      // cout << endl;
      if (tmp != C && (LB - n.p) >= (UB_p + pi_s)) {
        should_stop = false;
        Pi_s = pi_s;
        CC.erase_bit(i);
        C.set_bit(i);
      }
    } else {
      CC.erase_bit(i);
      n.C.erase_bit(i);
    }

    i = CC.next_bit();
  }

  while (!should_stop && !CC.is_empty()) {
    cout << "C = " << C << endl;
    UB_p += Pi_s;
    Pi_s = 0;
    C.erase_bit();
    should_stop = true;

    CC.init_scan(bbo::NON_DESTRUCTIVE);
    int i = CC.next_bit();
    while (i != EMPTY_ELEM) {
      int pi_s = max(Pi_s, data.p[i]);
      tmp = C;
      tmp &= data.N[i];
      if (tmp != C && (LB - n.p) >= (UB_p + pi_s)) {
        should_stop = false;
        Pi_s = pi_s;
        CC.erase_bit(i);
        C.set_bit(i);
      }

      i = CC.next_bit();
    }
  }

  return CC;
}

bool should_cut(const Node &n) { return false; }

void print_node(const Node &n) {
  cout << "Node:\n"
       << n.I << "\n"
       << n.p << " " << n.w << "\n"
       << n.C << endl;
}

Node add_item(const Node &n, int bit, const KPData &data) {
  Node new_node = n;
  new_node.I.set_bit(bit);
  new_node.p += data.p[bit];
  new_node.w += data.w[bit];
  new_node.C &= data.N[bit];
  new_node.C.erase_bit(bit);
  return new_node;
}

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
//
uint64_t nodes = 0;
Node branch_and_bound(const KPData &data, profit_t hLB = 0) {
  auto ones = bitarray(data.n);
  ones.set_bit(0, data.n - 1);
  I_inc = {bitarray(data.n), 0, 0, ones};
  LB = hLB;

  stack<Node> Q;
  Q.push(I_inc);
  while (!Q.empty()) {
    auto n = Q.top();
    // cout << "n = " << n.I << endl;
    Q.pop();
    nodes++;
    if (n.p > I_inc.p) {
      LB = n.p;
      I_inc = n;
    }
    if (!should_cut(n)) {
      bitarray B = partition(n, data);
      // cout << "B = " << B << endl;
      B.init_scan(bbo::NON_DESTRUCTIVE);
      auto bit = B.next_bit();
      while (bit != EMPTY_ELEM) {
        Q.push(add_item(n, bit, data));
        bit = B.next_bit();
      }
    }
    // cout << endl;
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
  vector<bitarray> adj(data.n);
  for (auto &barr : adj) {
    barr.init(data.n);
    barr.set_bit(0, data.n - 1);
  }
  for (auto p : data.pairs) {
    adj[p.first - 1].erase_bit(p.second - 1);
    adj[p.second - 1].erase_bit(p.first - 1);
  }
  KPData i = {data.n, data.p, data.w, data.W, adj};

  // (1) Preprocessing

  // (2) Heuristic initial solution

  // (3) Bounds table generation

  // (4) Branch & Bound
  Node s = branch_and_bound(i);
  cout << s.p << "\n" << s.I << endl;
  cout << nodes << endl;

  return 0;
}
