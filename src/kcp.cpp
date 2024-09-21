#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

struct KnapsackData {
    int n;
    int I;
    int W;
    vector<int> p;
    vector<int> w;
    vector<pair<int, int>> pairs;
};

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

int main() {
    cout << "Hello from KCP\n";
    return 0;
}
