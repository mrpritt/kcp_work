#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <unordered_map>
#include <boost/functional/hash.hpp>

#include "kpc_dp2.hpp"
#include "kcp.hpp"
#include "utils.hpp"

using namespace std;

// Custom hash function for pair of vector<bool> and int using Boost
struct PairHash {
    template <class T1>
    size_t operator () (const pair<vector<bool>, T1>& pair) const {
        size_t seed = 0;
        boost::hash_range(seed, pair.first.begin(), pair.first.end()); // Hash the vector<bool>
        boost::hash_combine(seed, pair.second); // Combine with the integer
        return seed;
    }
};

pair<int, vector<bool>> KPC_DP(KnapsackData& data, int J) {
    // Unpack KnapsackData
    int n = data.n;
    int W = data.W;
    const vector<int>& weights = data.w;
    const vector<int>& values = data.p;
    auto conflicts = pairs2vv(n, data.pairs);

    // Initialize the previous DP map
    unordered_map<pair<vector<bool>, int>, int, PairHash> previousDP;
    vector<bool> initialSubset(J);
    previousDP[{initialSubset, W}] = 0; // Base case: no items, full weight, no value

    // Fill the DP table iteratively
    for (int i = 0; i < n; ++i) { // For each item
        unordered_map<pair<vector<bool>, int>, int, PairHash> currentDP;

        for (const auto& entry : previousDP) { // Iterate through previous states
            auto [subset, weight] = entry.first;
            auto value = entry.second;

            vector<bool> newSubset = subset; // Copy the existing subset
            newSubset[i%J] = false;

            // Case 1: Do not include item i
            currentDP[{newSubset, weight}] = value;

            // Check if we can include item i
            if (weights[i] <= weight) {
                // Determine if we can include item i
                bool canInclude = true;
                for (int j : conflicts[i]) { // Check conflicts with item i
                    if ((i - J) < j && j < i && subset[j%J]) { // If item j is included in subset
                        canInclude = false;
                        break;
                    }
                }

                // Case 2: Include item i, if no conflicts
                if (canInclude) {
                    int newWeight = weight - weights[i];
                    int newValue = values[i] + value;
                    newSubset[i%J] = true; // Include item i in the subset

                    // Update current DP if this combination is better
                    currentDP[{newSubset, newWeight}] = newValue;
                }
            }
        }

        // Update previousDP for the next iteration
        previousDP = move(currentDP);
    }

    // Find the maximum value achievable with all items and weight limit W
    int maxValue = 0;
    for (const auto& entry : previousDP) {
        auto [subset, weight] = entry.first;
        auto value = entry.second;
        if (value > maxValue) {
            maxValue = value;
        }
    }

    // Isolate conflicts
    // for (int i = 0; i < n; ++i) {
    //     if (bestSolution[i]) {
    //         for (auto j : conflicts[i]) {
    //             if (bestSolution[j]) {
    //                 cout << "Conflict: " << i << " " << j << endl;
    //             }
    //         }
    //     }
    // }


    // Solve Conflicts


    // Improve solution


    return {maxValue, vector<bool>(n)};
}

