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

pair<vector<int>, int> linear_arrangement(vector<vector<int>> conflicts) {
    // TODO: implement some sort of linear arrangement
    // this just calculates the max distance in the default arrangement
    vector<int> arr(conflicts.size(), 0);
    iota(arr.begin(), arr.end(), 0);
    int max_d = 0;
    for (int i : arr) {
        for (int j : conflicts[i]) {
            int diff = abs(i - j);
            max_d = (max_d < diff)? diff : max_d;
        }
    }
    return {arr, max_d + 1};
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
    // J = 50;
    cout << "J = " << J << endl;

    // Initialize the previous DP map
    unordered_map<pair<vector<bool>, int>, pair<vector<bool>, int>, PairHash> previousDP;
    vector<bool> initialSubset(J);
    vector<bool> initialSolution(n);
    previousDP[{initialSubset, W}] = {initialSolution, 0}; // Base case: no items, full weight, no value


    // Fill the DP table iteratively
    for (int k = 0; k < n; ++k) { // For each item
        int i = linarr[k];
        unordered_map<pair<vector<bool>, int>, pair<vector<bool>, int>, PairHash> currentDP;

        for (const auto& entry : previousDP) { // Iterate through previous states
            auto [subset, weight] = entry.first;
            auto [solution, value] = entry.second;

            vector<bool> newSubset = subset; // Copy the existing subset
            newSubset[i%J] = false;

            vector<bool> newSolution = solution; // Copy existing solution
            newSolution[i] = false;

            // Case 1: Do not include item i
            currentDP[{newSubset, weight}] = {newSolution, value};

            // Check if we can include item i
            if (weights[i] <= weight) {
                // Determine if we can include item i
                bool canInclude = true;
                for (int j : conflicts[i]) { // Check conflicts with item i
                    if ((i - J) < j && j < i && subset[i%J - (i - j)]) { // If item j is included in subset
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
                    currentDP[{newSubset, newWeight}] = {newSolution, newValue};
                }
            }
        }

        // Update previousDP for the next iteration
        previousDP = move(currentDP);
    }

    // Find the maximum value achievable with all items and weight limit W
    int maxValue = 0;
    vector<bool> bestSolution(n, false);
    for (const auto& entry : previousDP) {
        auto [subset, weight] = entry.first;
        auto [solution, value] = entry.second;
        if (value > maxValue) {
            maxValue = value;
            bestSolution = solution;
        }
    }

    // NOTE: it might be necessary to check for conflicts. But it seems to me
    // that if the linear arrangement is done correctly, and the last J items
    // are remembered, then we always have a feasible solution at any step `i`.

    // Isolate conflicts
    for (int i = 0; i < n; ++i) {
        if (bestSolution[i]) {
            for (auto j : conflicts[i]) {
                if (bestSolution[j]) {
                    cout << "Conflict: " << i << " " << j << endl;
                }
            }
        }
    }


    // Solve Conflicts


    // Improve solution


    return {maxValue, bestSolution};
}

