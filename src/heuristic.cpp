#include "heuristic.hpp"

#include "kpc.hpp"
#include "model.hpp"
#include "solution.hpp"
#include "utils.hpp"

Solution heuristic(const KnapsackData &i_) {
  if (i_.n == 0)
    return Solution(i_);
  // solve the KP
  KPModel kp_model(i_);
  auto [kp_v, kp_x, kp_stat] = kp_model.solve();
  Solution s(kp_x, i_);
  // solve the MWIS of the selected items
  auto selectedItems = s.selectedItems();
  KnapsackData mwis_i_ = extractSubproblem(selectedItems, i_.W, i_);
  MWISModel mwis_model(mwis_i_);
  auto [mwis_v, mwis_x, mwis_stat] = mwis_model.solve();
  Solution sl(mwis_x, mwis_i_);
  // merge solutions
  int j = 0;
  for (auto item : selectedItems) {
    if (!sl[j]) {
      s.set(item, false);
    }
    ++j;
  }
  // use heuristic to solve the remaining compatible items
  auto compatibleItems = s.compatibleItems();
  KnapsackData ri_ = extractSubproblem(compatibleItems, i_.W - s.weight(), i_);
  Solution sll = heuristic(ri_);
  // merge solutions
  j = 0;
  for (auto item : compatibleItems) {
    if (sll[j]) {
      s.set(item, true);
    }
    ++j;
  }
  return s;
}
