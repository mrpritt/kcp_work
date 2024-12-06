#ifndef MODEL_HPP
#define MODEL_HPP

#include "kpc.hpp"
#include <ilcplex/ilocplex.h>
#include <memory>
#include <vector>

class Model {
protected:
  std::unique_ptr<IloEnv> env_;
  std::unique_ptr<IloModel> model_;
  IloNumVarArray x_;
  IloCplex solver_;

  void build(const KnapsackData &data);
  virtual void defineVariables(const KnapsackData &data) = 0;
  virtual void defineConstraints(const KnapsackData &data) = 0;
  virtual void defineObjective(const KnapsackData &data) = 0;

public:
  Model();
  std::tuple<double, std::vector<float>, IloAlgorithm::Status> solve();
  void exportModel(const std::string &filename);
  ~Model();
};

class KPModel : public Model {
protected:
  void defineVariables(const KnapsackData &data) override;
  void defineConstraints(const KnapsackData &data) override;
  void defineObjective(const KnapsackData &data) override;

public:
  KPModel(const KnapsackData &data) : Model() { build(data); };
};

class KPBModel : public Model {
protected:
  void defineVariables(const KnapsackData &data) override;
  void defineConstraints(const KnapsackData &data) override;
  void defineObjective(const KnapsackData &data) override;

public:
  KPBModel(const KnapsackData &data) : Model() { build(data); };
};

class KPCModel : public Model {
protected:
  void defineVariables(const KnapsackData &data) override;
  void defineConstraints(const KnapsackData &data) override;
  void defineObjective(const KnapsackData &data) override;

public:
  KPCModel(const KnapsackData &data) : Model() { build(data); };
};

class MWISModel : public Model {
protected:
  void defineVariables(const KnapsackData &data) override;
  void defineConstraints(const KnapsackData &data) override;
  void defineObjective(const KnapsackData &data) override;

public:
  MWISModel(const KnapsackData &data) : Model() { build(data); };
};

class LPKPCModel : public Model {
protected:
  void defineVariables(const KnapsackData &data) override;
  void defineConstraints(const KnapsackData &data) override;
  void defineObjective(const KnapsackData &data) override;

public:
  LPKPCModel(const KnapsackData &data) : Model() { build(data); };
};

#endif
