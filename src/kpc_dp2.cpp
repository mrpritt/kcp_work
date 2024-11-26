#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <numeric>
#include <unordered_map>
#include <boost/functional/hash.hpp>

#include "kpc_dp2.hpp"
#include "kpc.hpp"
#include "utils.hpp"
#include "timer.hpp"

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

static unsigned int nup;

tuple<int, vector<bool>, stats> KPC_DP(KnapsackData& data, int J) {
    // Unpack KnapsackData
    int n = data.n;
    int W = data.W;
    nup = 0;
    timer t;
    const vector<int>& weights = data.w;
    const vector<int>& values = data.p;
    auto conflicts = pairs2vv(n, data.pairs);

    // Initialize the previous DP map
    unordered_map<pair<vector<bool>, int>, pair<int, vector<bool>>, PairHash> previousDP;
    vector<bool> initialSubset(J);
    vector<bool> initialSolution(n);
    previousDP[{initialSubset, W}] = {0, initialSolution}; // Base case: no items, full weight, no value

    // Fill the DP table iteratively
    for (int i = 0; i < n; ++i) { // For each item
        unordered_map<pair<vector<bool>, int>, pair<int, vector<bool>>, PairHash> currentDP;

        for (const auto& entry : previousDP) { // Iterate through previous states
            auto [subset, weight] = entry.first;
            auto [value, solution] = entry.second;

            vector<bool> newSolution = solution;
            vector<bool> newSubset = subset; // Copy the existing subset
            newSubset[i%J] = false;

            // Case 1: Do not include item i
            currentDP[{newSubset, weight}] = {value, newSolution}; nup++;

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
                    newSolution[i] = true;

                    // Update current DP if this combination is better
                    currentDP[{newSubset, newWeight}] = {newValue, newSolution}; nup++;
                }
            }
        }

        // Update previousDP for the next iteration
        previousDP = move(currentDP);
    }

    // Find the maximum value achievable with all items and weight limit W
    int maxValue = 0;
    vector<bool> bestSolution;
    for (const auto& entry : previousDP) {
        auto [subset, weight] = entry.first;
        auto [value, solution] = entry.second;
        if (value > maxValue) {
            maxValue = value;
            bestSolution = solution;
        }
    }

    stats s = {nup, t.elapsed()};

    return {maxValue, bestSolution, s};
}

