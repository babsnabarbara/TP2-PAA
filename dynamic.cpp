#include "dynamic.hpp"
#include <algorithm>
#include <vector>

using namespace std;

Dynamic::Dynamic(int W, const vector<pair<float, int>> &items) {
  this->W = W;
  this->items.resize(items.size());
  for (size_t i = 0; i < items.size(); ++i) {
    this->items[i] = {static_cast<int>(i), static_cast<int>(items[i].first),
                      items[i].second};
  }
  this->maxProfit = 0;
  this->takenItems.resize(items.size(), false);
}

int Dynamic::knapsack() {
  int n = items.size();
  vector<vector<int>> dp(n + 1, vector<int>(W + 1, 0));

  for (int i = 1; i <= n; ++i) {
    for (int w = 1; w <= W; ++w) {
      if (items[i - 1].weight <= w) {
        dp[i][w] = max(dp[i - 1][w],
                       items[i - 1].value + dp[i - 1][w - items[i - 1].weight]);
      } else {
        dp[i][w] = dp[i - 1][w];
      }
    }
  }

  this->maxProfit = dp[n][W];

  int w_rem = W;
  for (int i = n; i > 0 && dp[i][w_rem] > 0; --i) {
    if (dp[i][w_rem] != dp[i - 1][w_rem]) {
      takenItems[items[i - 1].id] = true;
      w_rem -= items[i - 1].weight;
    }
  }

  return this->maxProfit;
}

int Dynamic::getMaxProfit() const { return this->maxProfit; }

const vector<bool> &Dynamic::getTakenItems() const { return this->takenItems; }
