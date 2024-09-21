#include "model.hpp"

#include <ilcplex/ilocplex.h>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

void Model::build(const KnapsackData& data) {
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
}

vector<bool> Model::getSolution(IloCplex solver) {
  const unsigned n = x_.getSize();

  IloNumArray xvalue(*env_);
  solver.getValues(x_, xvalue);
  vector<bool> S(n, false);
  for (auto i = 0u; i < n; ++i)
    S[i] = (xvalue[i] > 0.5);
  return S;
}

pair<vector<bool>, IloAlgorithm::Status> Model::solve() {
    // Create the CPLEX solver
    IloCplex solver(*model_);
    solver.exportModel("x.lp");
    // Solve the problem
    solver.solve();

    vector<bool> S;
    IloAlgorithm::Status status = solver.getStatus();
    if (status == IloAlgorithm::Feasible || status == IloAlgorithm::Optimal)
      S = getSolution(solver);
    else
      fmt::print("No feasible solution.\n");
    solver.end();
    return {S, status};
}
