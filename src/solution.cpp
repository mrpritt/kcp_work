#include "solution.hpp"

#include "instance.hpp"
#include <algorithm>
#include <numeric>
#include <stdexcept>
#define FMT_HEADER_ONLY
#include "fmt/format.h"

Solution::Solution(Instance &i) : s_(i.n, false), i_(i) {};

Solution::Solution(const std::vector<float> &x, Instance &i)
    : s_(x.size()), i_(i) {
  if (x.size() != (size_t)i_.n) {
    throw std::invalid_argument("Size of x does not match size of Instance");
  }
  for (size_t j = 0; j < x.size(); ++j)
    s_[j] = (x[j] > 0.5);
}

int Solution::value() {
  int sum = 0;
  for (size_t i = 0; i < s_.size(); ++i)
    sum = (s_[i]) ? sum + i_.p[i] : sum;
  return sum;
}

int Solution::weight() {
  int sum = 0;
  for (size_t i = 0; i < s_.size(); ++i)
    sum = (s_[i]) ? sum + i_.w[i] : sum;
  return sum;
}

bool Solution::isFeasible() {
  // TODO:
  return true;
}

void Solution::print() {
  // Order by efficiency
  std::vector<int> π(i_.n, 0);
  iota(π.begin(), π.end(), 0);
  sort(π.begin(), π.end(),
       [&](int i, int j) { return i_.p[i] * i_.w[j] > i_.p[j] * i_.w[i]; });

  for (auto i = 0u; i != s_.size(); ++i)
    fmt::print("{}", s_[π[i]] ? "■" : ".");
  fmt::print("\n");
}
