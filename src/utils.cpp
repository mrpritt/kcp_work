#include "utils.hpp"
#include <vector>
#include <numeric>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cuthill_mckee_ordering.hpp>

#include "kpc.hpp"

#define FMT_HEADER_ONLY
#include "fmt/format.h"


using namespace boost;
using namespace std;

vector<int> inv_arr(const vector<int>& arr) {
  vector<int> new_arr = arr;
  for (unsigned i = 0u; i < arr.size(); ++i)
    new_arr[arr[i]] = i;
  return new_arr;
}

template <class T>
vector<T> arrange_arr(const vector<T>& arr, const vector<int>& idx) {
  vector<T> arrd(arr.size());
  std::transform(idx.begin(), idx.end(), arrd.begin(),
                 [&arr](int i) { return arr[i]; });
  return arrd;
}

// Specialization for bool
template <>
std::vector<bool> arrange_arr(const std::vector<bool>& arr, const std::vector<int>& idx) {
    std::vector<bool> arrd(arr.size());
    std::transform(idx.begin(), idx.end(), arrd.begin(),
                   [&arr](int i) { return arr[i]; });
    return arrd;
}

vector<vector<int>> pairs2vv(int n, const vector<pair<int, int>>& pairs) {
    vector<vector<int>> vv(n);
    for (auto pair : pairs) {
        vv[pair.first - 1].push_back(pair.second - 1);
        vv[pair.second - 1].push_back(pair.first - 1);
    }
    return vv;
}

pair<vector<int>, int> linear_arrangement(const vector<vector<int>>& conflicts) {
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
    cuthill_mckee_ordering(g, ordering.rbegin()); // Use reverse iterator for RCM ordering

    // Compute the maximum bandwidth (max_d)
    int max_d = 0;
    for (unsigned i = 0; i < ordering.size(); ++i) {
        for (int j : conflicts[ordering[i]]) {
            int diff = abs((find(ordering.begin(), ordering.end(), j) - ordering.begin()) - i);
            max_d = max(max_d, diff);
        }
    }

    fmt::print("Cuthill-McKee window size: {}\n", max_d);
    return {ordering, max_d};
}

KnapsackData arrange_data(const vector<int>& idx, const KnapsackData& data) {
  KnapsackData ndata;
  ndata.n = data.n;
  ndata.I = data.I;
  ndata.W = data.W;
  ndata.p = arrange_arr(data.p, idx);
  ndata.w = arrange_arr(data.w, idx);
  for (auto p : data.pairs) {
    ndata.pairs.push_back({find(idx.begin(), idx.end(), p.first-1) - idx.begin() + 1,
                          find(idx.begin(), idx.end(), p.second-1) - idx.begin() + 1});
  }
  return ndata;
}
