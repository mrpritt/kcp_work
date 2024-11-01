#include "model.hpp"

#include <ilcplex/ilocplex.h>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

void KPModel::build(const KnapsackData& data) {
    // Create the CPLEX environment
    env_ = std::make_unique<IloEnv>();
    model_ = std::make_unique<IloModel>(*env_);

    // Variables
    x_ = IloNumVarArray(*env_, data.n, 0.0, 1.0, ILOINT);

    // Constraints
    IloExpr expr(*env_);
    for (int i = 0; i < data.n; ++i)
        expr += data.w[i] * x_[i];
    model_->add(expr <= data.W);

    // Objective
    expr.clear();
    for (int i = 0; i < data.n; ++i)
        expr += data.p[i] * x_[i];
    model_->add(IloMaximize(*env_, expr));

    solver_ = IloCplex(*model_);
}

void KPCModel::build(const KnapsackData& data) {
    // Create the CPLEX environment
    env_ = std::make_unique<IloEnv>();
    model_ = std::make_unique<IloModel>(*env_);

    // Variables
    x_ = IloNumVarArray(*env_, data.n, 0.0, 1.0, ILOINT);

    // Constraints
    IloExpr expr(*env_);
    for (int i = 0; i < data.n; ++i)
        expr += data.w[i] * x_[i];
    model_->add(expr <= data.W);

    for (const auto& pair : data.pairs) {
      model_->add(x_[pair.first-1] + x_[pair.second-1] <= 1);
    }

    // Objective
    expr.clear();
    for (int i = 0; i < data.n; ++i)
        expr += data.p[i] * x_[i];
    model_->add(IloMaximize(*env_, expr));

    solver_ = IloCplex(*model_);
}

void LPModel::build(const KnapsackData& data) {
    // Create the CPLEX environment
    env_ = std::make_unique<IloEnv>();
    model_ = std::make_unique<IloModel>(*env_);

    // Variables
    x_ = IloNumVarArray(*env_, data.n, 0.0, 1.0, ILOFLOAT);

    // Constraints
    IloExpr expr(*env_);
    for (int i = 0; i < data.n; ++i)
        expr += data.w[i] * x_[i];
    model_->add(expr <= data.W);

    for (const auto& pair : data.pairs) {
      model_->add(x_[pair.first-1] + x_[pair.second-1] <= 1);
    }

    // Objective
    expr.clear();
    for (int i = 0; i < data.n; ++i)
        expr += data.p[i] * x_[i];
    model_->add(IloMaximize(*env_, expr));

    solver_ = IloCplex(*model_);
}

void WMISModel::build(const KnapsackData& data) {
    // Create the CPLEX environment
    env_ = std::make_unique<IloEnv>();
    model_ = std::make_unique<IloModel>(*env_);

    // Variables
    x_ = IloNumVarArray(*env_, data.n, 0.0, 1.0, ILOINT);

    // Constraints
    for (const auto& pair : data.pairs) {
      model_->add(x_[pair.first-1] + x_[pair.second-1] <= 1);
    }

    // Objective
    IloExpr expr(*env_);
    for (int i = 0; i < data.n; ++i)
        expr += (data.p[i]) * x_[i];
    model_->add(IloMaximize(*env_, expr));

    solver_ = IloCplex(*model_);
}

pair<vector<bool>, float> KPModel::getSolution(vector<float> solution) {
  const unsigned n = x_.getSize();
  vector<bool> S(n, false);
  for (auto i = 0u; i < n; ++i)
    S[i] = (solution[i] > 0.5);
  return {S, solver_.getObjValue()};
}

pair<vector<bool>, float> KPCModel::getSolution(vector<float> solution) {
  const unsigned n = x_.getSize();
  vector<bool> S(n, false);
  for (auto i = 0u; i < n; ++i)
    S[i] = (solution[i] > 0.5);
  return {S, solver_.getObjValue()};
}
pair<vector<float>, float> LPModel::getSolution(vector<float> solution) {
  return {solution, solver_.getObjValue()};
}

pair<vector<bool>, float> WMISModel::getSolution(vector<float> solution) {
  const unsigned n = x_.getSize();
  vector<bool> S(n, false);
  for (auto i = 0u; i < n; ++i)
    S[i] = (solution[i] > 0.5);
  return {S, solver_.getObjValue()};
}

pair<vector<float>, IloAlgorithm::Status> Model::solve() {
    const unsigned n = x_.getSize();
    solver_.exportModel("x.lp");

    solver_.solve();

    vector<float> S(n, 0.0);
    IloAlgorithm::Status status = solver_.getStatus();

    if (status == IloAlgorithm::Feasible || status == IloAlgorithm::Optimal) {
      IloNumArray xvalue(*env_);
      solver_.getValues(x_, xvalue);
      for (auto i = 0u; i < n; ++i)
        S[i] = xvalue[i];
    }
    else
      fmt::print("No feasible solution.\n");
    return {S, status};
}

Model::~Model() {
    solver_.end();
}
