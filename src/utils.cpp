#include "utils.hpp"
#include "kpc.hpp"
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cuthill_mckee_ordering.hpp>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <unordered_set>
#include <vector>

using namespace boost;
using namespace std;

KnapsackData parseAMPLFile(const std::string &filename) {
  KnapsackData data;
  std::ifstream file(filename);
  std::string line;

  // Parse the file line by line
  while (std::getline(file, line)) {
    std::string keyword;

    // Parse the parameters
    if (line.find("param n :=") != std::string::npos) {
      std::istringstream iss(line.substr(line.find(":=") + 2));
      iss >> data.n;
    } else if (line.find("param c :=") != std::string::npos) {
      std::istringstream iss(line.substr(line.find(":=") + 2));
      iss >> data.W;
    }
    // Parse the profits and weights
    else if (line.find("param : V : p w :=") != std::string::npos) {
      while (std::getline(file, line) && !line.empty() && line != ";") {
        int index, profit, weight;
        std::istringstream row(line);
        row >> index >> profit >> weight;
        data.p.push_back(profit);
        data.w.push_back(weight);
      }
    }
    // Parse the conflict pairs
    else if (line.find("set E :=") != std::string::npos) {
      while (std::getline(file, line) && !line.empty() && line != ";") {
        int item1, item2;
        std::istringstream pairRow(line);
        pairRow >> item1 >> item2;
        data.pairs.emplace_back(item1 + 1, item2 + 1);
      }
    }
  }

  return data;
}

KnapsackData read_knapsack_data(const string &filename) {
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

vector<int> inv_arr(const vector<int> &arr) {
  vector<int> new_arr = arr;
  for (unsigned i = 0u; i < arr.size(); ++i)
    new_arr[arr[i]] = i;
  return new_arr;
}

template <class T>
vector<T> arrange_arr(const vector<T> &arr, const vector<int> &idx) {
  vector<T> arrd(arr.size());
  std::transform(idx.begin(), idx.end(), arrd.begin(),
                 [&arr](int i) { return arr[i]; });
  return arrd;
}

// Specialization for bool
template <>
std::vector<bool> arrange_arr(const std::vector<bool> &arr,
                              const std::vector<int> &idx) {
  std::vector<bool> arrd(arr.size());
  std::transform(idx.begin(), idx.end(), arrd.begin(),
                 [&arr](int i) { return arr[i]; });
  return arrd;
}

vector<vector<int>> pairs2vv(int n, const vector<pair<int, int>> &pairs) {
  vector<vector<int>> vv(n);
  for (auto pair : pairs) {
    vv[pair.first - 1].push_back(pair.second - 1);
    vv[pair.second - 1].push_back(pair.first - 1);
  }
  return vv;
}

pair<vector<int>, int>
linear_arrangement(const vector<vector<int>> &conflicts) {
  // Define the Boost Graph
  using Graph = adjacency_list<vecS, vecS, undirectedS>;
  Graph g(conflicts.size());

  // Build the graph from the conflicts adjacency list
  for (unsigned int u = 0; u < conflicts.size(); ++u) {
    for (int v : conflicts[u]) {
      add_edge(u, v, g);
    }
  }

  // Compute the Cuthill-McKee ordering
  vector<int> ordering(num_vertices(g));
  cuthill_mckee_ordering(
      g, ordering.rbegin()); // Use reverse iterator for RCM ordering

  // Compute the maximum bandwidth (max_d)
  int max_d = 0;
  for (unsigned i = 0; i < ordering.size(); ++i) {
    for (int j : conflicts[ordering[i]]) {
      int diff = abs(
          (find(ordering.begin(), ordering.end(), j) - ordering.begin()) - i);
      max_d = max(max_d, diff);
    }
  }

  // fmt::print("Cuthill-McKee window size: {}\n", max_d);
  return {ordering, max_d};
}

KnapsackData arrange_data(const vector<int> &idx, const KnapsackData &data) {
  KnapsackData ndata;
  ndata.n = data.n;
  ndata.I = data.I;
  ndata.W = data.W;
  ndata.p = arrange_arr(data.p, idx);
  ndata.w = arrange_arr(data.w, idx);
  for (auto p : data.pairs) {
    ndata.pairs.push_back(
        {find(idx.begin(), idx.end(), p.first - 1) - idx.begin() + 1,
         find(idx.begin(), idx.end(), p.second - 1) - idx.begin() + 1});
  }
  return ndata;
}

pair<set<int>, KnapsackData> extract_conflicts(const vector<bool> &s,
                                               const KnapsackData &data) {
  int n = data.n;
  int W = data.W;
  auto conflicts = pairs2vv(data.n, data.pairs);
  vector<vector<int>> c_conflicts(n);
  KnapsackData sub;
  set<int> c_items;
  for (int i = 0; i < n; ++i) {
    if (s[i]) {
      for (auto j : conflicts[i]) {
        if (j > i && s[j]) {
          c_items.insert(i);
          c_items.insert(j);
          c_conflicts[i].push_back(j);
        }
      }
    }
  }
  vector<int> map(c_items.begin(), c_items.end());
  for (auto i = 0u; i < map.size(); ++i) {
    int j = map[i];
    sub.p.push_back(data.p[j]);
    sub.w.push_back(data.w[j]);
    for (auto c : c_conflicts[j]) {
      auto c_id = (find(map.begin(), map.end(), c) - map.begin());
      if (c_id > i)
        sub.pairs.push_back({i + 1, c_id + 1});
    }
  }
  sub.n = c_items.size();
  sub.W = W;

  return {c_items, sub};
}

vector<bool> combine_solutions(const vector<bool> &s, const vector<bool> &c,
                               const set<int> &ref) {
  vector<bool> ns = s;
  int i = 0;
  for (auto r : ref) {
    ns[r] = c[i];
    ++i;
  }

  return ns;
}

std::unordered_set<int> available_items(const vector<bool> &s,
                                        const KnapsackData &data) {
  auto conflicts = pairs2vv(data.n, data.pairs);
  vector<int> items(data.n, 0);
  iota(items.begin(), items.end(), 0);
  std::unordered_set<int> free(items.begin(), items.end());
  for (auto i = 0u; i < s.size(); ++i) {
    if (s[i]) {
      free.erase(i);
      for (auto j : conflicts[i])
        free.erase(j);
    }
  }
  return free;
}

vector<bool> greedy_improvement(const vector<bool> &s,
                                const KnapsackData &data) {
  vector<bool> ns = s;
  auto conflicts = pairs2vv(data.n, data.pairs);
  auto free = available_items(s, data);
  int curr_weight = solution_weight(s, data);
  bool canImprove = true;
  while (canImprove) {
    // Get max item
    int best_i = -1;
    double max_value = -1;
    for (auto i = 0u; i < s.size(); ++i) {
      // check for conflicts
      if (!ns[i] && curr_weight + data.w[i] <= data.W && free.count(i) &&
          data.p[i] / data.w[i] > max_value) {
        best_i = i;
        max_value = data.p[i] / data.w[i];
      }
    }
    // Add it
    if (best_i >= 0) {
      ns[best_i] = true;
      curr_weight += data.w[best_i];
      free.erase(best_i);
      for (auto j : conflicts[best_i])
        free.erase(j);
    } else {
      canImprove = false;
    }
  }
  return ns;
}

int solution_weight(const vector<bool> &s, const KnapsackData &data) {
  int weight = 0;
  for (auto i = 0u; i < s.size(); ++i) {
    if (s[i]) {
      weight += data.w[i];
    }
  }
  return weight;
}

int solution_value(const vector<bool> &s, const KnapsackData &data) {
  int value = 0;
  for (auto i = 0u; i < s.size(); ++i) {
    if (s[i]) {
      value += data.p[i];
    }
  }
  return value;
}

int countXOR(const vector<bool> &a, const vector<bool> &b) {
  // NOTE: assumes a.size() == b.size()

  int count = 0;
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i] ^ b[i])
      ++count;
  }
  return count;
}

void print_data(const KnapsackData &data) {
  cout << data.n << " " << data.I << " " << data.W << endl;
  for (auto i : data.p)
    cout << i << " ";
  cout << endl;
  for (auto i : data.w)
    cout << i << " ";
  cout << endl;
  for (auto p : data.pairs)
    cout << p.first << " " << p.second << endl;
  cout << endl;
}

KnapsackData extractSubproblem(const std::set<int> &items, int W,
                               const KnapsackData &data) {
  KnapsackData sub;
  sub.n = items.size();
  sub.W = W;
  sub.I = 0;
  sub.p = vector<int>(items.size());
  sub.w = vector<int>(items.size());
  sub.pairs = vector<pair<int, int>>();

  int j = 0;
  for (int i : items) {
    sub.p[j] = data.p[i];
    sub.w[j] = data.w[i];
    ++j;
  }

  for (auto pair : data.pairs) {
    if (items.count(pair.first - 1) && items.count(pair.second - 1)) {
      int idx1 = std::distance(items.cbegin(), items.find(pair.first - 1));
      int idx2 = std::distance(items.cbegin(), items.find(pair.second - 1));
      sub.pairs.emplace_back(idx1 + 1, idx2 + 1);
      ++sub.I;
    }
  }

  return sub;
}
