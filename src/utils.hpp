#ifndef UTILS_HPP
#define UTILS_HPP

#include "kpc.hpp"
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

typedef struct {
  unsigned int nup;
  double tim;
} stats;

KnapsackData parseAMPLFile(const std::string &filename);
KnapsackData read_knapsack_data(const std::string &filename);

void print_data(const KnapsackData&);

KnapsackData extractSubproblem(const std::set<int>&, int, const KnapsackData &);

int countXOR(const std::vector<bool> &a, const std::vector<bool> &b);

std::vector<int> inv_arr(const std::vector<int> &arr);

template <class T>
std::vector<T> arrange_arr(const std::vector<T> &arr,
                           const std::vector<int> &idx);

template <>
std::vector<bool> arrange_arr(const std::vector<bool> &arr,
                              const std::vector<int> &idx);

std::vector<std::vector<int>>
pairs2vv(int n, const std::vector<std::pair<int, int>> &pairs);

std::vector<std::vector<int>>
pairs2vb(int n, const std::vector<std::pair<int, int>> &pairs);

std::pair<std::vector<int>, int>
linear_arrangement(const std::vector<std::vector<int>> &conflicts);

std::pair<std::set<int>, KnapsackData>
extract_conflicts(const std::vector<bool> &s, const KnapsackData &data);

std::vector<bool> combine_solutions(const std::vector<bool> &s,
                                    const std::vector<bool> &c,
                                    const std::set<int> &ref);

KnapsackData arrange_data(const std::vector<int> &idx,
                          const KnapsackData &data);

std::vector<bool> greedy_improvement(const std::vector<bool> &s,
                                     const KnapsackData &data);

int solution_value(const std::vector<bool> &s, const KnapsackData &data);

std::unordered_set<int> available_items(const std::vector<bool> &s,
                                        const KnapsackData &data);

int solution_weight(const std::vector<bool> &s, const KnapsackData &data);

#endif
