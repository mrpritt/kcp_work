#ifndef KPC_DP2_HPP
#define KPC_DP2_HPP
#include <vector>
#include <utility>

#include "kpc.hpp"

std::tuple<int, std::vector<bool>, unsigned, double> KPC_DP(KnapsackData& data, int J);

#endif
