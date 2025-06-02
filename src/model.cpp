#include "model.hpp"

#include <ilcplex/ilocplex.h>
#include <optional>

#include "kpc.hpp"
#include "utils.hpp"

using namespace std;

Model::Model(std::optional<double> time_limit) {
  env_ = make_unique<IloEnv>();
  model_ = make_unique<IloModel>(*env_);
  solver_ = IloCplex(*model_);
  solver_.setParam(IloCplex::Param::Threads, 1);
  if (time_limit.has_value()) {
    solver_.setParam(IloCplex::Param::TimeLimit, *time_limit);
  }
  solver_.setOut(env_->getNullStream());
  solver_.setWarning(env_->getNullStream());
}

Model::~Model() { solver_.end(); }

void Model::build(const KnapsackData &data) {
  defineVariables(data);
  defineConstraints(data);
  defineObjective(data);
}

tuple<double, vector<float>, IloAlgorithm::Status> Model::solve() {
  const unsigned n = x_.getSize();

  solver_.solve();

  vector<float> S(n, 0.0);
  IloAlgorithm::Status status = solver_.getStatus();

  if (status == IloAlgorithm::Feasible || status == IloAlgorithm::Optimal) {
    IloNumArray xvalue(*env_);
    solver_.getValues(x_, xvalue);
    for (auto i = 0u; i < n; ++i)
      S[i] = xvalue[i];
  } else
    cout << "No feasible solution." << endl;
  return {solver_.getObjValue(), S, status};
}

void Model::exportModel(const string &filename) {
  solver_.exportModel(filename.c_str());
}

/*
 * KPModel
 */
void KPModel::defineVariables(const KnapsackData &data) {
  x_ = IloNumVarArray(*env_, data.n, 0.0, 1.0, ILOINT);
}

void KPModel::defineConstraints(const KnapsackData &data) {
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += data.w[i] * x_[i];
  model_->add(expr <= data.W);
}

void KPModel::defineObjective(const KnapsackData &data) {
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += data.p[i] * x_[i];
  model_->add(IloMaximize(*env_, expr));
}

/*
 * KPBModel
 */
void KPBModel::defineVariables(const KnapsackData &data) {
  x_ = IloNumVarArray(*env_, data.n, 0.0, 1.0, ILOINT);
}

void KPBModel::defineConstraints(const KnapsackData &data) {
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += data.w[i] * x_[i];
  model_->add(expr >= data.W);
}

void KPBModel::defineObjective(const KnapsackData &data) {
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += data.p[i] * x_[i];
  model_->add(IloMinimize(*env_, expr));
}

/*
 * KPCModel
 */
void KPCModel::defineVariables(const KnapsackData &data) {
  x_ = IloNumVarArray(*env_, data.n, 0.0, 1.0, ILOINT);
}

void KPCModel::defineConstraints(const KnapsackData &data) {
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += data.w[i] * x_[i];
  model_->add(expr <= data.W);

  for (const auto &pair : data.pairs) {
    model_->add(x_[pair.first - 1] + x_[pair.second - 1] <= 1);
  }
}

void KPCModel::defineObjective(const KnapsackData &data) {
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += data.p[i] * x_[i];
  model_->add(IloMaximize(*env_, expr));
}

/*
 * ILP2
 */
void ILP2::defineVariables(const KnapsackData &data) {
  x_ = IloNumVarArray(*env_, data.n, 0.0, 1.0, ILOINT);
}

void ILP2::defineConstraints(const KnapsackData &data) {
  vector<vector<int>> adj = pairs2vv(data.n, data.pairs);
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += data.w[i] * x_[i];
  model_->add(expr <= data.W);
  expr.end();

#if 1
  IloExpr conflictExpr(*env_);
  for (int i = 0; i < data.n; ++i) {
    conflictExpr.clear();
    for (int j : adj[i]) conflictExpr += x_[j];
    model_->add(conflictExpr <= adj[i].size() * (1 - x_[i]));
  }
  conflictExpr.end();
#endif
}

void ILP2::defineObjective(const KnapsackData &data) {
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += data.p[i] * x_[i];
  model_->add(IloMaximize(*env_, expr));
}

/*
 * MinCut Cliques
 */
void MCC::defineVariables(const KnapsackData &data) {
  x_ = IloNumVarArray(*env_, data.pairs.size(), 0.0, 1.0, ILOINT);
  model_->add(x_);
}

void MCC::defineConstraints(const KnapsackData &data) {
  auto adj = pairs2vb(data.n, data.pairs);

  IloExpr conflictExpr(*env_);
  for (int i = 0; i < data.n - 2; ++i) {
    for (int j = i + 1; j < data.n - 1; ++j) {
      for (int k = j + 1; k < data.n; ++k) {
        conflictExpr.clear();
        if (adj[i][j]) conflictExpr += x_[adj[i][j]-1];
        if (adj[j][k]) conflictExpr += x_[adj[j][k]-1];
        if (adj[i][k]) conflictExpr -= x_[adj[i][k]-1];
        model_->add(conflictExpr <= 1);
        conflictExpr.clear();
        if (adj[i][j]) conflictExpr += x_[adj[i][j]-1];
        if (adj[j][k]) conflictExpr -= x_[adj[j][k]-1];
        if (adj[i][k]) conflictExpr += x_[adj[i][k]-1];
        model_->add(conflictExpr <= 1);
        conflictExpr.clear();
        if (adj[i][j]) conflictExpr -= x_[adj[i][j]-1];
        if (adj[j][k]) conflictExpr += x_[adj[j][k]-1];
        if (adj[i][k]) conflictExpr += x_[adj[i][k]-1];
        model_->add(conflictExpr <= 1);
      }
    }
  }
  conflictExpr.end();
}

void MCC::defineObjective(const KnapsackData &data) {
  IloExpr expr(*env_);
  for (int i = 0; i < data.pairs.size(); ++i)
      expr += x_[i];
  model_->add(IloMaximize(*env_, expr));
}

/*
 * MWISModel
 */
void MWISModel::defineVariables(const KnapsackData &data) {
  x_ = IloNumVarArray(*env_, data.n, 0.0, 1.0, ILOINT);
}

void MWISModel::defineConstraints(const KnapsackData &data) {
  for (const auto &pair : data.pairs) {
    model_->add(x_[pair.first - 1] + x_[pair.second - 1] <= 1);
  }
}

void MWISModel::defineObjective(const KnapsackData &data) {
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += (data.p[i]) * x_[i];
  model_->add(IloMaximize(*env_, expr));
}

/*
 * LPKPCModel
 */
void LPKPCModel::defineVariables(const KnapsackData &data) {
  x_ = IloNumVarArray(*env_, data.n, 0.0, 1.0, ILOFLOAT);
}

void LPKPCModel::defineConstraints(const KnapsackData &data) {
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += data.w[i] * x_[i];
  model_->add(expr <= data.W);

  for (const auto &pair : data.pairs) {
    model_->add(x_[pair.first - 1] + x_[pair.second - 1] <= 1);
  }
}

void LPKPCModel::defineObjective(const KnapsackData &data) {
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += data.p[i] * x_[i];
  model_->add(IloMaximize(*env_, expr));
}

/*
 * LP2
 */
void LP2::defineVariables(const KnapsackData &data) {
  x_ = IloNumVarArray(*env_, data.n, 0.0, 1.0, ILOFLOAT);
}

void LP2::defineConstraints(const KnapsackData &data) {
  vector<vector<int>> adj = pairs2vv(data.n, data.pairs);
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += data.w[i] * x_[i];
  model_->add(expr <= data.W);
  expr.end();

  IloExpr conflictExpr(*env_);
  for (int i = 0; i < data.n; ++i) {
    conflictExpr.clear();
    for (int j : adj[i]) conflictExpr += x_[j];
    model_->add(conflictExpr <= adj[i].size() * (1 - x_[i]));
  }
  conflictExpr.end();
}

void LP2::defineObjective(const KnapsackData &data) {
  IloExpr expr(*env_);
  for (int i = 0; i < data.n; ++i)
    expr += data.p[i] * x_[i];
  model_->add(IloMaximize(*env_, expr));
}
