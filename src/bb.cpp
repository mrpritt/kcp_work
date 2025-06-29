#include "bbtypes.h"
#include "kpc.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
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
uint64_t nodes = 0;
uint64_t ub_l2_cuts = 0;
uint64_t ub_mt_cuts = 0;
uint64_t ub_p_cuts = 0;
vector<vector<profit_t>> UB_L2_T;

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
  UB_L2_T.resize(data.n);
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
    bitarray mask(n);
    mask.set_bit();
    if (j > 0)
      mask.erase_bit(0, j - 1);
    vector<int> prev(data.W + 1, 0);
    vector<int> curr(data.W + 1, 0);

    for (int l = 0; l < cliques.size(); l++) {
      bitarray C_(cliques[l]);
      C_ &= mask;
      if (C_.is_empty())
        continue;
      for (int s = 0; s <= data.W; s++) {
        int max_c = prev[s];
        C_.init_scan(bbo::NON_DESTRUCTIVE);
        auto i = C_.next_bit();
        while (i != EMPTY_ELEM) {
          const int wi = data.w[i];
          if (wi <= s)
            max_c = max(max_c, prev[s - wi] + data.p[i]);

          i = C_.next_bit();
        }
        curr[s] = max_c;
      }
      prev.swap(curr);
    }
    UB_L2_T[j] = prev;
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
      if (tmp == C && (I_inc.p - n.p) >= (UB_p + pi_s)) {
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
      if (tmp == C && (I_inc.p - n.p) >= (UB_p + pi_s)) {
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

profit_t UB_L2(Node &n, const KPData &data) {
  auto j = n.C.lsbn64();
  weight_t cV = data.W - n.w;
  if (j == EMPTY_ELEM || cV < 0) {
    return 0;
  }
  return UB_L2_T[j][cV];
}

profit_t UB_MT(Node &n, const KPData &data) {
  profit_t UB_mt = std::numeric_limits<profit_t>::max();
  weight_t cV = data.W - n.w;
  profit_t ub_mt_base = 0;
  weight_t w = 0;
  n.C.init_scan(bbo::NON_DESTRUCTIVE);
  int tm1 = EMPTY_ELEM;
  int t = n.C.next_bit();
  if (t == EMPTY_ELEM) {
    return 0;
  }
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
    UB_mt = max(ub_0, ub_1);
  }
  return UB_mt;
}

profit_t UB_P(Node &n, const KPData &data) {
  weight_t cV = data.W - n.w;
  if (cV < 0)
    return 0;
  // (0) Partition n.C into cliques using first fit and simultaneously
  // calculate the MCKP critical item
  double bar_w = cV;
  int bar_j = data.n;
  Partition cliques;
  vector<double> c_profits;

  n.C.init_scan(bbo::NON_DESTRUCTIVE);
  int i = n.C.next_bit();
  while (i != EMPTY_ELEM) {
    bool found = false;
    for (int j = 0; j < cliques.size(); j++) {
      auto &C = cliques[j];
      bitarray tmp(C);
      tmp &= data.N[i];
      if (tmp == C) {
        C.set_bit(i);
        if (data.p[i] > c_profits[j]) {
          if (data.w[i] <= bar_w) {
            bar_w = bar_w - (data.p[i] - c_profits[j]) *
                                ((double)data.w[i] / data.p[i]);
          } else {
            bar_j = min(bar_j, i);
          }
          c_profits[j] = data.p[i];
        }
        found = true;
        break;
      }
    }
    if (!found) {
      bitarray C_(data.n);
      C_.set_bit(i);
      cliques.push_back(C_);
      c_profits.push_back(data.p[i]);
    }

    i = n.C.next_bit();
  }

  profit_t UB_p;
  // (1) Check for closed form solution
  if (bar_j == data.n) {
    UB_p = 0;
    for (auto p : c_profits)
      UB_p += p;
  } else {
    // (2) Otherwise calculate beta_hat and pi_c_hat
    double bar_beta = (double)data.p[bar_j] / data.w[bar_j];
    for (int c = 0; c < cliques.size(); c++) {
      cliques[c].init_scan(bbo::NON_DESTRUCTIVE);
      int i = cliques[c].next_bit();
      c_profits[c] = data.p[i] - bar_beta * data.w[i];
      while (i != EMPTY_ELEM) {
        c_profits[c] = max(c_profits[c], data.p[i] - bar_beta * data.w[i]);
        i = cliques[c].next_bit();
      }
      c_profits[c] = max(c_profits[c], .0);
    }
    double ub_p = 0;
    for (auto bar_pi_c : c_profits)
      ub_p += bar_pi_c;
    ub_p += bar_beta * cV;
    UB_p = floor(ub_p);
  }
  return UB_p;
}

profit_t UB(Node &n, const KPData &data) {
  profit_t ub_l2 = UB_L2(n, data);
  profit_t ub_mt = UB_MT(n, data);
  profit_t ub_p = UB_P(n, data);
  return min(ub_l2, min(ub_mt, ub_p));
}

bool should_cut(Node &n, const KPData &data) {
  // UB_L2_T
  if (UB_L2(n, data) <= I_inc.p - n.p) {
    ub_l2_cuts++;
    return true;
  }

  // UB_MT
  if (UB_MT(n, data) <= I_inc.p - n.p) {
    ub_mt_cuts++;
    return true;
  }

  // UB_p
  if (UB_P(n, data) <= I_inc.p - n.p) {
    ub_p_cuts++;
    return true;
  }

  return false;
}

Node branch_and_bound(const KPData &data, Node root) {
  stack<Node> Q;
  Q.push(root);
  while (!Q.empty()) {
    auto n = Q.top();
    Q.pop();
    if (!should_cut(n, data)) {
      nodes++;
      bitarray B = partition(n, data);
      B.init_scan(bbo::NON_DESTRUCTIVE_REVERSE);
      auto bit = B.previous_bit();
      while (bit != EMPTY_ELEM) {
        auto nl = add_item(n, bit, data);
        if (nl.p > I_inc.p) {
          I_inc = nl;
        }
        Q.push(nl);
        bit = B.previous_bit();
      }
    }
  }

  return I_inc;
}

Node _heuristic1(const KPData &data, int force_item = -1) {
  Node S = {bitarray(data.n), 0, 0, bitarray(data.n)};
  S.C.set_bit(0, data.n - 1);
  if (force_item >= 0 && force_item < data.n) {
    S.I.set_bit(force_item);
    S.C &= data.NC[force_item];
  }
  for (int i = 0; i < data.n; i++) {
    if (S.C.is_bit(i) && S.w + data.w[i] <= data.W) {
      S.I.set_bit(i);
      S.p += data.p[i];
      S.w += data.w[i];
      S.C &= data.NC[i];
      S.C.erase_bit(i);
    }
  }
  return S;
}

Node heuristic1(const KPData &data) {
  Node h1 = {bitarray(data.n), 0, 0, bitarray(data.n)};
  for (int i = 0; i < data.n; i++) {
    auto _h1 = _heuristic1(data, i);
    if (_h1.p > h1.p)
      h1 = _h1;
  }
  return h1;
}

Node heuristic2(const KPData &data) {
  Node S = {bitarray(data.n), 0, 0, bitarray(data.n)};
  return S;
}

bool verify_node(Node &n, const KPData &data) {
  profit_t p = 0;
  weight_t w = 0;
  n.I.init_scan(bbo::NON_DESTRUCTIVE);
  auto i = n.I.next_bit();
  while (i != EMPTY_ELEM) {
    p += data.p[i];
    w += data.w[i];
    i = n.I.next_bit();
  }
  if (w > data.W) {
    return false;
  }
  bitarray tmp(n.I);
  n.I.init_scan(bbo::NON_DESTRUCTIVE);
  i = n.I.next_bit();
  while (i != EMPTY_ELEM) {
    tmp &= data.NC[i];
    i = n.I.next_bit();
  }
  if (tmp != n.I) {
    return false;
  }

  return true;
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
  vector<bitarray> adjC(data.n, bitarray(data.n));
  for (int i = 0; i < data.n; i++) {
    adjC[i].set_bit(0, data.n - 1);
  }
  for (auto p : data.pairs) {
    adj[p.first - 1].set_bit(p.second - 1);
    adj[p.second - 1].set_bit(p.first - 1);
    adjC[p.first - 1].erase_bit(p.second - 1);
    adjC[p.second - 1].erase_bit(p.first - 1);
  }
  KPData instance = {data.n, data.p, data.w, data.W, adj, adjC};

  // (1) Heuristic initial solution
  auto h1 = heuristic1(instance);
  auto h2 = heuristic2(instance);
  I_inc.I.init(instance.n);
  I_inc.C.init(instance.n);
  I_inc = (h1.p > h2.p) ? h1 : h2;
  profit_t LBi = I_inc.p;

  // (2) Bounds table generation
  generate_ubl2_table(instance);

  // (3) Preprocessing
  Node root = {bitarray(data.n), 0, 0, bitarray(data.n)};
  root.C.set_bit(0, data.n - 1);
  Node tmp = {bitarray(data.n), 0, 0, bitarray(data.n)};
  for (int i = 0; i < data.n; i++) {
    tmp.I.erase_bit();
    tmp.I.set_bit(i);
    tmp.p = data.p[i];
    tmp.w = data.w[i];
    tmp.C = instance.NC[i];
    if (I_inc.p >= (UB(tmp, instance) + data.p[i])) {
      root.C.erase_bit(i);
    }
    tmp.I.erase_bit();
    tmp.p = 0;
    tmp.w = 0;
    tmp.C.set_bit(0, data.n - 1);
    tmp.C.erase_bit(i);
    if (I_inc.p >= UB(tmp, instance)) {
      if (root.w + data.w[i] <= data.W && root.C.is_bit(i)) {
        root.I.set_bit(i);
        root.p += data.p[i];
        root.w += data.w[i];
        root.C &= instance.NC[i];
        root.C.erase_bit(i);
      }
    }
  }

  // (4) Branch & Bound
  Node s = branch_and_bound(instance, root);
  cout << s.p << "," << LBi << "," << nodes << "," << ub_l2_cuts << ","
       << ub_mt_cuts << "," << ub_p_cuts << "," << verify_node(s, instance)
       << endl;

  return 0;
}
