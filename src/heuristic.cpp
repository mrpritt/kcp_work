#include "heuristic.hpp"

#include "kpc.hpp"
#include "model.hpp"
#include "solution.hpp"
#include "utils.hpp"

Solution left_heuristic(const KnapsackData &i_) {
  // solve the KP
  KPModel kp_model(i_);
  auto [kp_v, kp_x, kp_stat] = kp_model.solve();
  Solution s(kp_x, i_);
  auto selectedItems = s.selectedItems();
  if (selectedItems.size() == 0) {
    return Solution(i_);
  }
  // solve the MWIS of the selected items
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
  Solution sll = left_heuristic(ri_);
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

Solution right_heuristic(const KnapsackData &i_) {
  // solve the MWIS
  MWISModel mwis_model(i_);
  auto [mwis_v, mwis_x, mwis_stat] = mwis_model.solve();
  Solution s(mwis_x, i_);
  auto selectedItems = s.selectedItems();
  // solve the \KP: remove the least amount of profit so that it fits.
  KnapsackData kpb_i_ = extractSubproblem(selectedItems, s.weight() - i_.W, i_);
  KPBModel kpb_model(kpb_i_);
  auto [kpb_v, kpb_x, kpb_stat] = kpb_model.solve();
  Solution sl(kpb_x, kpb_i_);
  if (sl.selectedItems().size() == kpb_i_.n) {
    return Solution(i_);
  }
  // merge solutions
  int j = 0;
  for (auto item : selectedItems) {
    if (sl[j]) {
      s.set(item, false);
    }
    ++j;
  }
  // use heuristic to solve the remaining compatible items
  auto compatibleItems = s.compatibleItems();
  KnapsackData ri_ = extractSubproblem(compatibleItems, i_.W - s.weight(), i_);
  Solution sll = right_heuristic(ri_);
  // merge solutions
  j = 0;
  for (auto item : compatibleItems) {
    if (sll[j]) {
      s.set(item, true);
    }
    ++j;
  }
  return s;

  return Solution(i_);
}
