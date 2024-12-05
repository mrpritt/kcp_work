#include "instance.hpp"

#include "kpc.hpp"
#include "utils.hpp"
#include <string>

Instance::Instance(const std::string &filename)
    : Instance(read_knapsack_data(filename)) {}

Instance::Instance(const KnapsackData& data)
: n(data.n), W(data.W), p(data.p), w(data.w), c(pairs2vv(n, data.pairs)) {}
