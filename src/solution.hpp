#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include "instance.hpp"
#include <set>
#include <vector>

class Solution {
private:
  std::vector<bool> s_;
  Instance &i_;

public:
  Solution(Instance &);
  Solution(const std::vector<float> &, Instance &);
  bool operator[](size_t idx) { return s_[idx]; };
  int value();
  int weight();
  bool isFeasible();
  void print();
  std::set<int> getCompatibleItems();
};

#endif
