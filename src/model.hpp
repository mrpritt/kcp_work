#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <sstream>

#include "kpc.hpp"

#include <ilcplex/ilocplex.h>

using namespace std;

class Model {
protected:
    std::unique_ptr<IloEnv> env_;
    std::unique_ptr<IloModel> model_;
    IloNumVarArray x_;
    IloCplex solver_;

public:
    virtual void build(const KnapsackData& data)=0;
    std::pair<std::vector<float>, IloAlgorithm::Status> solve();
    ~Model();
};

class KPModel: public Model {
public:
    void build(const KnapsackData& data);
    pair<std::vector<bool>, float> getSolution(vector<float> solution);
};

class KPCModel: public Model {
public:
    void build(const KnapsackData& data);
    pair<std::vector<bool>, float> getSolution(vector<float> solution);
};

class LPModel: public Model {
public:
    void build(const KnapsackData& data);
    pair<std::vector<float>, float> getSolution(vector<float> solution);
};

class WMISModel: public Model {
public:
    void build(const KnapsackData& data);
    pair<std::vector<bool>, float> getSolution(vector<float> solution);
};
