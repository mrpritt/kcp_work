#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include "kpc.hpp"
#include <set>
#include <vector>

class Solution {
private:
  std::vector<bool> s_;
  const KnapsackData &i_;

public:
  Solution(const KnapsackData &);
  Solution(const std::vector<float> &, const KnapsackData &);
  bool operator[](std::size_t idx) { return s_[idx]; };
  void set(std::size_t idx, bool v) { s_[idx] = v; };
  int value();
  int weight();
  bool isFeasible();
  void print();
  std::set<int> compatibleItems();
  std::set<int> selectedItems();
};

#endif
