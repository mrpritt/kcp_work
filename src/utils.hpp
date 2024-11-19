#include <vector>
#include "kpc.hpp"

std::vector<int> inv_arr(const std::vector<int>& arr);

template <class T>
std::vector<T> arrange_arr(const std::vector<T>& arr, const std::vector<int>& idx);

template <>
std::vector<bool> arrange_arr(const std::vector<bool>& arr, const std::vector<int>& idx);

std::vector<std::vector<int>> pairs2vv(int n, const std::vector<std::pair<int, int>>& pairs);

// TODO: implement some sort of linear arrangement
// this just calculates the max distance in the default arrangement
std::pair<std::vector<int>, int> linear_arrangement(const std::vector<std::vector<int>>& conflicts);

KnapsackData arrange_data(const std::vector<int>& idx, const KnapsackData& data);
