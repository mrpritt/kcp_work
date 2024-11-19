/**
 * \file kcp.hpp
 *   \author Marcus Ritt <marcus.ritt@inf.ufrgs.br>
 *   \date Time-stamp: <2024-09-21 11:57:47 ritt>
 */
#pragma once

#include <vector>
#include <utility>

struct KnapsackData {
    int n;
    int I;
    int W;
    std::vector<int> p;
    std::vector<int> w;
    std::vector<std::pair<int, int>> pairs;
};
