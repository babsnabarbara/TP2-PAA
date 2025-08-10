#ifndef BACKTRACKING_HPP
#define BACKTRACKING_HPP

#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>

class Backtracking {
public:
  Backtracking(int W, const std::vector<std::pair<float, int>> &items);
  int solve();
  int getMaxProfit() const;
  const std::vector<bool> &getTakenItems() const;

private:
  void backtrack(int i, int current_weight, int current_value,
                 std::vector<bool> &current_selection);
  double upperBound(int i, int current_weight, int current_value) const;

  int W;
  int n;
  std::vector<int> weights;
  std::vector<int> values;
  std::vector<double> valuePerWeight;
  int maxProfit;
  std::vector<bool> takenItems;
  std::vector<int> originalIndices;
};

#endif // BACKTRACKING_HPP
