#include "model.hpp"

#include <ortools/linear_solver/linear_solver.h>

void Model::build(const KnapsackData& data) {
    // Create the linear solver with the SCIP backend.
    solver_ = operations_research::MPSolver::CreateSolver("SCIP");
    if (!solver_) {
        std::cerr << "SCIP solver unavailable." << std::endl;
        return;
    }

    // Variables
    for (int i = 0; i < data.n; ++i) {
        x_.push_back(solver_->MakeIntVar(0.0, 1.0, "x_" + std::to_string(i)));
    }

    // Constraints
    solver_->Add(solver_->ScalProd(data.w, x_) <= data.W);

    // Objective
    solver_->Maximize(solver_->ScalProd(data.p, x_));
}

operations_research::MPSolver::ResultStatus Model::solve() {
    // Solve the problem
    return solver_->Solve();
}

double Model::getObjectiveValue() const {
    return solver_->Objective().Value();
}
