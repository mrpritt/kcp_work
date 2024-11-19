#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <boost/functional/hash.hpp>

#include "kpc_dp.hpp"
#include "kpc.hpp"
#include "logging.hpp"

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

int nup;

pair<int, vector<bool>> knapsackWithConflicts(KnapsackData& data) {
  nup = 0; timer t;
  
    int n = data.n;
    int W = data.W;
    const vector<int>& weights = data.w;
    const vector<int>& values = data.p;
    vector<vector<int>> conflicts(data.I);
    for (auto pair : data.pairs) {
        conflicts[pair.first - 1].push_back(pair.second - 1);
        conflicts[pair.second - 1].push_back(pair.first - 1);
    }

    // Initialize the previous DP map
    unordered_map<pair<vector<bool>, int>, int, PairHash> previousDP;
    vector<bool> initialSubset(n, false);
    previousDP[{initialSubset, W}] = 0; // Base case: no items, no weight, no value

    // Fill the DP table iteratively
    for (int i = 0; i < n; ++i) { // For each item
        unordered_map<pair<vector<bool>, int>, int, PairHash> currentDP;

        for (const auto& entry : previousDP) { // Iterate through previous states
            auto [subset, weight] = entry.first;
            int value = entry.second;

            // Case 1: Do not include item i
            currentDP[{subset, weight}] = value; nup++;

            // Check if we can include item i
            if (weights[i] <= weight) {
                // Determine if we can include item i
                bool canInclude = true;
                for (int j : conflicts[i]) { // Check conflicts with item i
                    if (subset[j]) { // If item j is included in subset
                        canInclude = false;
                        break;
                    }
                }

                // Case 2: Include item i, if no conflicts
                if (canInclude) {
                    int newWeight = weight - weights[i];
                    vector<bool> newSubset = subset; // Copy the existing subset
                    newSubset[i] = true; // Include item i in the subset
                    int newValue = values[i] + value;

                    // Update current DP if this combination is better
                    currentDP[{newSubset, newWeight}] = newValue; nup++;
                }
            }
        }

        // Update previousDP for the next iteration
        previousDP = move(currentDP);
    }

    // Find the maximum value achievable with all items and weight limit W
    int maxValue = 0;
    vector<bool> bestSubset(n, false); // Track best subset
    for (const auto& entry : previousDP) {
        auto [subset, weight] = entry.first;
        if (weight <= W) {
            if (entry.second > maxValue) {
                maxValue = entry.second;
                bestSubset = subset; // Set bestSubset to the corresponding one
            }
        }
    }

    fmt::print("Full DP, states {}, value {}, time {}\n", nup, maxValue, t.elapsed());

    return {maxValue, bestSubset};
}

