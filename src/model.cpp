#include "model.hpp"

#include <ilcplex/ilocplex.h>

void Model::build(const KnapsackData& data) {
    // Create the CPLEX environment
    env_ = std::make_unique<IloEnv>();
    model_ = std::make_unique<IloModel>(*env_);

    // Variables
    x_.resize(data.n);
    for (int i = 0; i < data.n; ++i) {
        x_[i] = IloBoolVar(*env_);
        model_->add(x_[i]);
    }

    // Constraints
    IloExpr expr(*env_);
    for (int i = 0; i < data.n; ++i) {
        expr += data.w[i] * x_[i];
    }
    model_->add(expr <= data.W);

    // Objective
    expr.clear();
    for (int i = 0; i < data.n; ++i) {
        expr += data.p[i] * x_[i];
    }
    model_->add(IloMaximize(*env_, expr));
}

bool Model::solve() {
    // Create the CPLEX solver
    IloCplex cplex(*model_);

    // Solve the problem
    return cplex.solve();
}

double Model::getObjectiveValue() const {
    // Create the CPLEX solver
    IloCplex cplex(*model_);
    return cplex.getObjValue();
}
