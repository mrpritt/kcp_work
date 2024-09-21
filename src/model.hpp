#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <sstream>

#include "kcp.hpp"

#include <ilcplex/ilocplex.h>

using namespace std;

class Model {
private:
    std::unique_ptr<IloEnv> env_;
    std::unique_ptr<IloModel> model_;
    std::vector<IloBoolVar> x_;

public:
    void build(const KnapsackData& data);
    bool solve();
    double getObjectiveValue() const;
};
