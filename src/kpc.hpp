/**
 * \file kcp.hpp
 *   \author Marcus Ritt <marcus.ritt@inf.ufrgs.br>
 *   \date Time-stamp: <2024-11-27 15:12:48 ritt>
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

    KnapsackData() : n(0), I(0), W(0) {}
};
