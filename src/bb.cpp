#include "bbobject.h"
#include "bbtypes.h"
#include "kpc.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <stack>
#include <string>

#include "bitscan.h"

using namespace std;

using Partition = vector<bitarray>;
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
  vector<bitarray> NC;
} KPData;

typedef struct {
  bitarray I;
  profit_t p;
  weight_t w;
  bitarray C;
} Node;

Node I_inc;
profit_t LB = 0;
uint64_t nodes = 0;
vector<vector<profit_t>> UB_L2;

void print_node(const Node &n) {
  cout << "Node:\n" << n.I << "\n" << n.p << " " << n.w << "\n" << n.C << endl;
}

Node add_item(const Node &n, int bit, const KPData &data) {
  Node new_node = n;
  new_node.I.set_bit(bit);
  new_node.p += data.p[bit];
  new_node.w += data.w[bit];
  new_node.C &= data.NC[bit];
  new_node.C.erase_bit(bit);
  return new_node;
}

void generate_ubl2_table(const KPData &data) {
  auto n = data.n;
  UB_L2.resize(data.n);
  Partition cliques;

  // Partition cliques
  int nv = 0;
  vector<bool> free(n, true);
  while (nv < n) {
    bitarray C(n);
    int bits = 0;
    for (int i = 0; i < n; i++) {
      if (!free[i])
        continue;
      bitarray tmp(C);
      tmp &= data.N[i];
      if (tmp == C) { // is_clique
        C.set_bit(i);
        free[i] = false;
        bits++;
      }
    }
    nv += bits;
    cliques.push_back(C);
  }

  for (int j = 0; j < n; j++) {
    Partition P;
    bitarray mask(n);
    mask.set_bit();
    if (j > 0)
      mask.erase_bit(0, j - 1);
    for (const auto &C : cliques) {
      bitarray C_(C);
      C_ &= mask;
      if (!C_.is_empty())
        P.push_back(C_);
    }

    vector<int> prev(data.W + 1, 0);
    vector<int> curr(data.W + 1, 0);

    for (int l = 0; l < P.size(); l++) {
      for (int s = 0; s <= data.W; s++) {
        int max_c = prev[s];
        P[l].init_scan(bbo::NON_DESTRUCTIVE);
        auto i = P[l].next_bit();
        while (i != EMPTY_ELEM) {
          const int wi = data.w[i];
          if (wi <= s)
            max_c = max(max_c, prev[s - wi] + data.p[i]);

          i = P[l].next_bit();
        }
        curr[s] = max_c;
      }
      prev.swap(curr);
    }
    UB_L2[j] = prev;
  }
}

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
// return CC # Now CC is the branching set
bitarray partition(Node &n, const KPData &data) {
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
      if (tmp == C && (LB - n.p) >= (UB_p + pi_s)) {
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
      if (tmp == C && (LB - n.p) >= (UB_p + pi_s)) {
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

// SHOULD_CUT (n, data)
// j = alpha(n.I)
// cV = data.W - n.w
// # UB_L2
// if (UB_L2[j][cV] <= LB - n.p) {
//    return true;
// }
// # UB_MT
//
// # UB_p
//
// return false;
bool should_cut(Node &n, const KPData &data) {
  if (n.C.is_empty())
    return true;

  auto j = n.C.lsbn64();
  weight_t cV = data.W - n.w;

  // UB_L2
  if (UB_L2[j][cV] <= LB - n.p)
    return true;

  // UB_MT
  profit_t ub_mt_base = 0;
  weight_t w = 0;
  n.C.init_scan(bbo::NON_DESTRUCTIVE);
  int tm1 = EMPTY_ELEM;
  int t = n.C.next_bit();
  int tp1 = n.C.next_bit();
  while (tp1 != EMPTY_ELEM && w + data.w[t] <= cV) {
    ub_mt_base += data.p[t];
    w += data.w[t];
    tm1 = t;
    t = tp1;
    tp1 = n.C.next_bit();
  }
  if (tp1 != EMPTY_ELEM && tm1 != EMPTY_ELEM) {
    weight_t cVb = cV - w;
    profit_t ub_0 = ub_mt_base + floor(cVb * (data.p[tp1] / data.w[tp1]));
    profit_t ub_1 =
        ub_mt_base +
        floor(data.p[t] - ((data.w[t] - cVb) * (data.p[tm1] / data.w[tm1])));
    profit_t ub_mt = max(ub_0, ub_1);
    if (ub_mt <= LB - n.p)
      return true;
  }

  // BUG: needs debugging, currently seg faults
  //
  // // UB_p
  // // (0) Partition n.C into cliques using first fit and simultaneously calculate
  // //     the MCKP critical item
  // double bar_w = cV;
  // int bar_j = data.n;
  // Partition cliques;
  // vector<double> c_profits;
  // vector<weight_t> c_weights;
  // cliques.push_back(bitarray(data.n));
  //
  // n.C.init_scan(bbo::NON_DESTRUCTIVE);
  // int i = n.C.next_bit();
  // while (i != EMPTY_ELEM) {
  //   bool found = false;
  //   for (int j = 0; j < cliques.size(); j++) {
  //     auto &C = cliques[j];
  //     bitarray tmp(C);
  //     tmp &= data.N[i];
  //     if (tmp == C) {
  //       C.set_bit(i);
  //       if (data.p[i] > c_profits[j]) {
  //         if (data.w[i] <= bar_w) {
  //           bar_w = bar_w - (data.p[i] - c_profits[j]) *
  //                               ((double)data.w[j] / data.p[j]);
  //         } else {
  //           bar_j = min(bar_j, j);
  //         }
  //         c_profits[j] = c_profits[j];
  //         c_weights[j] = data.w[j];
  //       }
  //       found = true;
  //       break;
  //     }
  //   }
  //   if (!found) {
  //     bitarray C_(data.n);
  //     C_.set_bit(i);
  //     cliques.push_back(C_);
  //     c_profits.push_back(data.p[i]);
  //     c_weights.push_back(data.w[i]);
  //   }
  //   i = n.C.next_bit();
  // }
  // // (1) Check for closed form solution
  // weight_t wc = 0;
  // for (auto w : c_weights)
  //   wc += w;
  //
  // profit_t UB_p;
  // if (wc <= cV) {
  //   UB_p = 0;
  //   for (auto p : c_profits)
  //     UB_p += p;
  // } else {
  //   // (2) Calculate beta_hat and pi_c_hat
  //   double bar_beta = (double)data.p[bar_j] / data.w[bar_j];
  //   for (int c = 0; c < cliques.size(); c++) {
  //     cliques[c].init_scan(bbo::NON_DESTRUCTIVE);
  //     int i = cliques[c].next_bit();
  //     c_profits[c] = data.p[i] - bar_beta * data.w[i];
  //     while (i != EMPTY_ELEM) {
  //       c_profits[c] = max(c_profits[c], data.p[i] - bar_beta * data.w[i]);
  //       i = cliques[c].next_bit();
  //     }
  //     c_profits[c] = max(c_profits[c], .0);
  //   }
  //   double ub_p = 0;
  //   for (auto bar_pi_c : c_profits)
  //     ub_p += bar_pi_c;
  //   ub_p += bar_beta * cV;
  //   UB_p = floor(ub_p);
  // }
  //
  // // (3) Bound
  // if (UB_p <= LB - n.p) {
  //   return true;
  // }

  return false;
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
Node branch_and_bound(const KPData &data, profit_t hLB = 0) {
  I_inc.I.init(data.n);
  I_inc.p = 0;
  I_inc.w = 0;
  I_inc.C.init(data.n);
  I_inc.C.set_bit(0, data.n - 1);
  LB = hLB;

  stack<Node> Q;
  Q.push(I_inc);
  while (!Q.empty()) {
    auto n = Q.top();
    Q.pop();
    nodes++;
    if (n.p > I_inc.p) {
      LB = n.p;
      I_inc = n;
    }
    if (!should_cut(n, data)) {
      bitarray B = partition(n, data);
      B.init_scan(bbo::NON_DESTRUCTIVE_REVERSE);
      auto bit = B.previous_bit();
      while (bit != EMPTY_ELEM) {
        Q.push(add_item(n, bit, data));
        bit = B.previous_bit();
      }
    }
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
  vector<bitarray> adj(data.n, bitarray(data.n));
  for (auto p : data.pairs) {
    adj[p.first - 1].set_bit(p.second - 1);
    adj[p.second - 1].set_bit(p.first - 1);
  }
  vector<bitarray> adjC(data.n, bitarray(data.n));
  for (int i = 0; i < data.n; i++) {
    adjC[i] = adj[i];
    adjC[i].flip();
  }
  KPData instance = {data.n, data.p, data.w, data.W, adj, adjC};

  // (1) Heuristic initial solution

  // (2) Bounds table generation
  generate_ubl2_table(instance);

  // (3) Preprocessing

  // (4) Branch & Bound
  Node s = branch_and_bound(instance);
  cout << s.p << "," << nodes << endl;

  return 0;
}
