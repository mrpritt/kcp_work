#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

struct KnapsackData {
    int n;
    int I;
    int W;
    std::vector<int> p;
    std::vector<int> w;
    std::vector<std::pair<int, int>> pairs;
};

KnapsackData read_knapcak_data(const std::string& filename) {
    KnapsackData data;
    std::ifstream file(filename);
    std::string line;

    if (file.is_open()) {
        std::getline(file, line);
        std::stringstream ss(line);
        ss >> data.n >> data.I >> data.W;

        std::getline(file, line);
        ss = std::stringstream(line);
        data.p.resize(data.n);
        for (int i = 0; i < data.n; ++i) {
            ss >> data.p[i];
        }

        std::getline(file, line);
        ss = std::stringstream(line);
        data.w.resize(data.n);
        for (int i = 0; i < data.n; ++i) {
            ss >> data.w[i];
        }

        data.pairs.resize(data.I);
        for (int i = 0; i < data.I; ++i) {
            std::getline(file, line);
            ss = std::stringstream(line);
            ss >> data.pairs[i].first >> data.pairs[i].second;
        }

        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }

    return data;
}

int main() {
    std::cout << "Hello from KCP\n";
    return 0;
}
