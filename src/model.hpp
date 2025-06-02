#ifndef MODEL_HPP
#define MODEL_HPP

#include "kpc.hpp"
#include <ilcplex/ilocplex.h>
#include <memory>
#include <optional>
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
  Model(std::optional<double> time_limit = std::nullopt);
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

class ILP2 : public Model {
protected:
  void defineVariables(const KnapsackData &data) override;
  void defineConstraints(const KnapsackData &data) override;
  void defineObjective(const KnapsackData &data) override;

public:
  ILP2(const KnapsackData &data) : Model() { build(data); };
};

class MCC : public Model {
protected:
  void defineVariables(const KnapsackData &data) override;
  void defineConstraints(const KnapsackData &data) override;
  void defineObjective(const KnapsackData &data) override;

public:
  MCC(const KnapsackData &data, std::optional<double> time_limit = std::nullopt)
      : Model(time_limit) {
    build(data);
  };
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

class LP2 : public Model {
protected:
  void defineVariables(const KnapsackData &data) override;
  void defineConstraints(const KnapsackData &data) override;
  void defineObjective(const KnapsackData &data) override;

public:
  LP2(const KnapsackData &data) : Model() { build(data); };
};

#endif
