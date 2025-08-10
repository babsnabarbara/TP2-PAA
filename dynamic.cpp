#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

using namespace std;
using namespace std::chrono;

// Structure to represent an item
struct Item {
  int id;
  int weight;
  int value;
};

// Function to solve the 0/1 knapsack problem
void knapsack(int W, const vector<Item> &items) {
  int n = items.size();
  vector<vector<int>> dp(n + 1, vector<int>(W + 1, 0));

  auto start = high_resolution_clock::now();

  // Build the DP table
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

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);

  int maxValue = dp[n][W];
  vector<Item> selectedItems;

  // Backtrack to find the selected items
  int w = W;
  for (int i = n; i > 0 && maxValue > 0; --i) {
    if (maxValue != dp[i - 1][w]) {
      selectedItems.push_back(items[i - 1]);
      maxValue -= items[i - 1].value;
      w -= items[i - 1].weight;
    }
  }
  reverse(selectedItems.begin(), selectedItems.end());

  cout << "Maximum value: " << dp[n][W] << endl;
  cout << "Selected items (Weight, Value):" << endl;
  for (const auto &item : selectedItems) {
    cout << "Item " << item.id << ": " << item.weight << ", " << item.value
         << endl;
  }
  cout << "Execution time: " << duration.count() << " microseconds" << endl;
}
