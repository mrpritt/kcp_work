#include "utils.hpp"
#include <vector>
#include <numeric>
#include <algorithm>

#include "kcp.hpp"

using namespace std;

vector<int> inv_arr(const vector<int>& arr) {
  vector<int> new_arr = arr;
  for (int i = 0; i < arr.size(); ++i)
    new_arr[arr[i]] = i;
  return new_arr;
}

template <class T>
vector<T> arrange_arr(const vector<T>& arr, const vector<int>& idx) {
  vector<T> arrd = arr;
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
    vector<int> arr(conflicts.size(), 0);
    iota(arr.begin(), arr.end(), 0);
    int max_d = 0;
    for (int i : arr) {
        for (int j : conflicts[i]) {
            int diff = abs(i - j);
            max_d = (max_d < diff)? diff : max_d;
        }
    }
    return {arr, max_d};
}

KnapsackData arrange_data(const vector<int>& idx, const KnapsackData& data) {
  KnapsackData ndata;
  ndata.n = data.n;
  ndata.I = data.I;
  ndata.W = data.W;
  ndata.p = arrange_arr(data.p, idx);
  ndata.w = arrange_arr(data.w, idx);
  vector<pair<int, int>> npairs = data.pairs;
  for (auto pair : ndata.pairs) {
    pair.first = idx[pair.first];
    pair.second = idx[pair.second];
  }
  ndata.pairs = npairs;
  return ndata;
}
