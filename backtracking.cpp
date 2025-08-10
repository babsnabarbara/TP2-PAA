#include "backtracking.hpp"
#include <algorithm>
#include <numeric>
#include <stack> // Required for the iterative approach
#include <tuple> // Required for storing state

// The constructor remains identical to the original version.
Backtracking::Backtracking(int W,
                           const std::vector<std::pair<float, int>> &items) {
  this->W = W;
  this->n = items.size();

  std::vector<int> p(n);
  std::iota(p.begin(), p.end(), 0);

  // Sort items by value-to-weight ratio in descending order
  std::sort(p.begin(), p.end(), [&](int i, int j) {
    return (double)items[i].second / items[i].first >
           (double)items[j].second / items[j].first;
  });

  this->weights.resize(n);
  this->values.resize(n);
  this->originalIndices.resize(n);
  for (int i = 0; i < n; ++i) {
    this->weights[i] = items[p[i]].first;
    this->values[i] = items[p[i]].second;
    this->originalIndices[i] = p[i];
  }

  this->maxProfit = 0;
  this->takenItems.resize(n, false);
}

// The upperBound function remains identical to the original version.
double Backtracking::upperBound(int i, int current_weight,
                                int current_value) const {
  double bound = current_value;
  int weight = current_weight;
  int k = i;
  while (k < n && weight + weights[k] <= W) {
    weight += weights[k];
    bound += values[k];
    k++;
  }
  if (k < n) {
    bound += (W - weight) * ((double)values[k] / weights[k]);
  }
  return bound;
}

// The recursive `backtrack` function is replaced by the iterative `solve`
// method.
int Backtracking::solve() {
  // Use a tuple to store the state: {index, weight, value, selection}
  using State = std::tuple<int, int, int, std::vector<bool>>;
  std::stack<State> s;

  // Push the initial state onto the stack to start the process
  s.push({0, 0, 0, std::vector<bool>(n, false)});

  while (!s.empty()) {
    // Pop the current state to explore
    auto [i, current_weight, current_value, current_selection] = s.top();
    s.pop();

    // Base Case: If we have considered all items
    if (i == n) {
      if (current_value > maxProfit) {
        maxProfit = current_value;
        takenItems = current_selection;
      }
      continue; // End of this path
    }

    // Pruning: If the upper bound of this path is not better than the max
    // profit found so far, we don't need to explore it further.
    if (upperBound(i, current_weight, current_value) <= maxProfit) {
      continue;
    }

    // --- Branching ---
    // To simulate the depth-first search of recursion, we push the states
    // in the reverse order of how they would be called recursively.

    // Branch 1: Exclude the current item `i`.
    // We push this state first so it gets processed after the "include" branch.
    s.push({i + 1, current_weight, current_value, current_selection});

    // Branch 2: Include the current item `i` (if it fits in the knapsack).
    if (current_weight + weights[i] <= W) {
      // Create a new selection vector for this new path.
      std::vector<bool> next_selection = current_selection;
      next_selection[i] = true;
      s.push({i + 1, current_weight + weights[i], current_value + values[i],
              next_selection});
    }
  }

  // Reorder the `takenItems` vector to match the original item indices.
  std::vector<bool> original_order_taken_items(n, false);
  for (int i = 0; i < n; ++i) {
    if (takenItems[i]) {
      original_order_taken_items[originalIndices[i]] = true;
    }
  }
  takenItems = original_order_taken_items;

  return maxProfit;
}

// The accessor methods remain identical.
int Backtracking::getMaxProfit() const { return maxProfit; }

const std::vector<bool> &Backtracking::getTakenItems() const {
  return takenItems;
}
