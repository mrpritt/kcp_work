#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <set>
#include <string>
#include <vector>
#include "kpc.hpp"

class Instance {
public:
  const int n;
  const int W;
  const std::vector<int> p;
  const std::vector<int> w;
  const std::vector<std::vector<int>> c;

  Instance(const std::string &);
  Instance(const KnapsackData &);
  Instance extractSubproblem(std::set<int>);
};

#endif
