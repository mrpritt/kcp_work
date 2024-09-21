#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#define FMT_HEADER_ONLY
#include "fmt/format.h"
#include "model.hpp"
using namespace std;

#include "kcp.hpp"

using namespace std;

KnapsackData read_knapcak_data(const string& filename) {
    KnapsackData data;
    ifstream file(filename);
    string line;

    if (file.is_open()) {
        getline(file, line);
        stringstream ss(line);
        ss >> data.n >> data.I >> data.W;

        getline(file, line);
        ss = stringstream(line);
        data.p.resize(data.n);
        for (int i = 0; i < data.n; ++i) {
            ss >> data.p[i];
        }

        getline(file, line);
        ss = stringstream(line);
        data.w.resize(data.n);
        for (int i = 0; i < data.n; ++i) {
            ss >> data.w[i];
        }

        data.pairs.resize(data.I);
        for (int i = 0; i < data.I; ++i) {
            getline(file, line);
            ss = stringstream(line);
            ss >> data.pairs[i].first >> data.pairs[i].second;
        }

        file.close();
    } else {
        cerr << "Unable to open file: " << filename << endl;
    }

    return data;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Error: filename is required\n";
        cerr << "Usage: kcp <filename>\n";
        return 1;
    }

    string filename = argv[1];
    KnapsackData data = read_knapcak_data(filename);

    try {
      Model model;
      model.build(data);
      model.solve();
    } catch (IloCplex::Exception e) {
    }

    fmt::print("Objective function value: {}\n", model.getObjectiveValue());

    return 0;
}
