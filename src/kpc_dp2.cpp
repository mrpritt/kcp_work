#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <boost/dynamic_bitset.hpp>


#include "kpc_dp2.hpp"
#include "kcp.hpp"

using namespace std;

// Custom hash function for pair of vector<bool> and int using Boost
// struct PairHash {
//     template <class T1>
//     size_t operator () (const pair<boost::dynamic_bitset, T1>& pair) const {
//         size_t seed = 0;
//         boost::hash_range(seed, pair.first.begin(), pair.first.end()); // Hash the vector<bool>
//         boost::hash_combine(seed, pair.second); // Combine with the integer
//         return seed;
//     }
// };
struct dbs_hash {
    template <typename Block, typename Alloc>
    size_t operator()(boost::dynamic_bitset<Block, Alloc> const& bs) const {
        size_t seed = boost::hash_value(bs.size());

        std::vector<Block> blocks(bs.num_blocks());
        boost::hash_range(seed, blocks.begin(), blocks.end());

        return seed;
    }
};
// struct boost_dynamic_bitset_hash {
//     template <typename Block, typename Alloc>
//     size_t operator()(const boost::dynamic_bitset<Block, Alloc>& bs) const {
//         // Initial seed for the hash
//         size_t seed = 0;
//         output_iterator<Block> bs_iterator;
//         bs.to_block_range(bs, bs_iterator);
//         boost::hash_range(seed, bs_iterator.begin(), bs_iterator.end());
//         return seed;
//     }
// };

// Specialization of hash for std::pair<boost::dynamic_bitset, int>
struct pair_dynamic_bitset_int_hash {
    size_t operator()(const std::pair<boost::dynamic_bitset<>, int>& p) const {
        // Hash the boost::dynamic_bitset part
        size_t seed = dbs_hash()(p.first);
        boost::hash_combine(seed, p.second); // Combine with the integer
        return seed;
    }
};

pair<vector<int>, int> linear_arrangement(vector<vector<int>> conflicts) {
    // TODO: implement some sort of linear arrangement
    // this just calculates the max distance in the default arrangement
    vector<int> arr(conflicts.size(), 0);
    iota(arr.begin(), arr.end(), 0);
    int max_d = 0;
    for (int i : arr) {
        for (int j : conflicts[i]) {
            int diff = abs((i+1) - j);
            max_d = (max_d < diff)? diff : max_d;
        }
    }
    return {arr, max_d};
}

pair<int, vector<bool>> KPC_DP(KnapsackData& data) {
    int n = data.n;
    int W = data.W;
    const vector<int>& weights = data.w;
    const vector<int>& values = data.p;
    vector<vector<int>> conflicts(data.n);
    for (auto pair : data.pairs) {
        conflicts[pair.first - 1].push_back(pair.second - 1);
        conflicts[pair.second - 1].push_back(pair.first - 1);
    }
    // Calculate the linear arrangement of items
    auto [linarr, J] = linear_arrangement(conflicts);
    J = 20;
    cout << "J = " << J << endl;
    for (int i : linarr) cout << i << " ";
    cout << endl;

    // Initialize the previous DP map
    unordered_map<pair<boost::dynamic_bitset<>, int>, int, pair_dynamic_bitset_int_hash> previousDP;
    boost::dynamic_bitset initialSubset(J);
    previousDP[make_pair(initialSubset, W)] = 0; // Base case: no items, full weight, no value


    // Fill the DP table iteratively
    for (int k = 0; k < n; ++k) { // For each item
        int i = linarr[k];
        unordered_map<pair<boost::dynamic_bitset<>, int>, int, pair_dynamic_bitset_int_hash> currentDP;

        for (const auto& entry : previousDP) { // Iterate through previous states
            auto [subset, weight] = entry.first;
            int value = entry.second;

            boost::dynamic_bitset newSubset = subset << 1; // Copy the existing subset

            // Case 1: Do not include item i
            currentDP[make_pair(newSubset, weight)] = value;

            // Check if we can include item i
            if (weights[i] <= weight) {
                // Determine if we can include item i
                bool canInclude = true;
                for (int j : conflicts[i]) { // Check conflicts with item i
                    if (j < i && subset[J - (i - j)]) { // If item j is included in subset
                        canInclude = false;
                        break;
                    }
                }

                // Case 2: Include item i, if no conflicts
                if (canInclude) {
                    int newWeight = weight - weights[i];
                    newSubset[J-1] = 1; // Include item i in the subset
                    int newValue = values[i] + value;

                    // Update current DP if this combination is better
                    currentDP[make_pair(newSubset, newWeight)] = newValue;
                }
            }
        }

        // Update previousDP for the next iteration
        previousDP = std::move(currentDP);
    }

    // NOTE: it might be necessary to check for conflicts. But it seems to me
    // that if the linear arrangement is done correctly, and the last J items
    // are remembered, then we always have a feasible solution at any step `i`.

    // Find the maximum value achievable with all items and weight limit W
    // NOTE: tracing back the bestSubset might be tricky
    int maxValue = 0;
    vector<bool> bestSubset(n, false); // Track best subset
    for (const auto& entry : previousDP) {
        auto [subset, weight] = entry.first;
        if (weight <= W) {
            if (entry.second > maxValue) {
                maxValue = entry.second;
            }
        }
    }

    return {maxValue, bestSubset};
}

