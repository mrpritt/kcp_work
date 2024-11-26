#ifndef KPC_DP2_HPP
#define KPC_DP2_HPP
#include <vector>
#include <utility>

#include "kpc.hpp"
#include "utils.hpp"

std::tuple<int, std::vector<bool>, stats> KPC_DP(KnapsackData& data, int J);

#endif
