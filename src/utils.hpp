#ifndef UTILS_HPP
#define UTILS_HPP

#include "kpc.hpp"
#include <set>
#include <unordered_set>
#include <vector>

std::vector<int> inv_arr(const std::vector<int>& arr);

template <class T>
std::vector<T> arrange_arr(const std::vector<T>& arr, const std::vector<int>& idx);

template <>
std::vector<bool> arrange_arr(const std::vector<bool>& arr, const std::vector<int>& idx);

std::vector<std::vector<int>> pairs2vv(int n, const std::vector<std::pair<int, int>>& pairs);

std::pair<std::vector<int>, int> linear_arrangement(const std::vector<std::vector<int>>& conflicts);

std::pair<std::set<int>, KnapsackData> extract_conflicts(const std::vector<bool>& s, const KnapsackData& data);

std::vector<bool> combine_solutions(const std::vector<bool>& s, const std::vector<bool>& c, const std::set<int>& ref);

KnapsackData arrange_data(const std::vector<int>& idx, const KnapsackData& data);

std::vector<bool> greedy_improvement(const std::vector<bool>& s, const KnapsackData& data);

int solution_value(const std::vector<bool>& s, const KnapsackData& data);

std::unordered_set<int> available_items(const std::vector<bool>& s, const KnapsackData& data);

int solution_weight(const std::vector<bool>& s, const KnapsackData& data);

#endif
