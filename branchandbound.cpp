#include "branchandbound.hpp"
#include <queue>
#include <climits>

// Comparison function for sorting items by value/weight ratio (descending)
bool BranchAndBound::cmp(Item a, Item b) {
    return (a.value / a.weight) > (b.value / b.weight);
}

// Calculate upper bound using fractional knapsack approach
int BranchAndBound::bound(Node u, int n, int W, Item arr[]) {
    if (u.weight >= W) {
        return 0;
    }
    
    int bound = u.profit;
    int j = u.level + 1;
    int totweight = u.weight;
    
    // Add items that fit completely
    while (j < n && totweight + arr[j].weight <= W) {
        totweight += arr[j].weight;
        bound += arr[j].value;
        j++;
    }
    
    // Add fractional part of the next item if it doesn't fit completely
    if (j < n) {
        bound += (W - totweight) * (arr[j].value / arr[j].weight);
    }
    
    return bound;
}

int BranchAndBound::knapsack() {
  int n = this->items.size();
  int W = this->capacidade;
  Item *items = this->items.data();

  // Sort items by value/weight ratio
  sort(items, items + n, cmp);

  // Use a queue of smart pointers to manage node memory automatically  
  queue<unique_ptr<Node>> Q;

  // The root node of our decision tree
  auto root = make_unique<Node>();
  root->level = -1;
  root->profit = 0;
  root->weight = 0;
  root->path.resize(n, false); // Initialize path with all false
  Q.push(std::move(root));

  // This will hold the final node of the best solution found
  vector<bool> bestPath(n, false);
  this->maxProfit = 0;

  // Memory management: limit queue size to prevent memory explosion
  const int MAX_QUEUE_SIZE = 1000000; // Adjust based on available memory
  int nodesProcessed = 0;

  while (!Q.empty() && nodesProcessed < MAX_QUEUE_SIZE) {
    nodesProcessed++;
    
    // Move ownership of the node from the queue to the local variable 'u'
    unique_ptr<Node> u = std::move(Q.front());
    Q.pop();

    // If at the last item, we can't branch further
    if (u->level == n - 1) {
      continue;
    }

    // --- Branch 1: Include the next item ---
    int nextLevel = u->level + 1;
    auto include = make_unique<Node>();
    include->level = nextLevel;
    include->weight = u->weight + items[nextLevel].weight;
    include->profit = u->profit + items[nextLevel].value;
    include->path = u->path; // Copy the path from parent
    include->path[nextLevel] = true; // Mark this item as taken
    include->index = items[nextLevel].index;
    include->TookItem = true;

    if (include->weight <= W && include->profit > this->maxProfit) {
      this->maxProfit = include->profit;
      bestPath = include->path; // Save the best path found
    }

    include->bound = bound(*include, n, W, items);

    if (include->bound > this->maxProfit && Q.size() < MAX_QUEUE_SIZE) {
      Q.push(std::move(include)); // Give ownership to the queue
    }

    // --- Branch 2: Exclude the next item ---
    auto exclude = make_unique<Node>();
    exclude->level = nextLevel;
    exclude->weight = u->weight;
    exclude->profit = u->profit;
    exclude->path = u->path; // Copy the path from parent
    exclude->path[nextLevel] = false; // Mark this item as not taken
    exclude->index = items[nextLevel].index;
    exclude->TookItem = false;

    exclude->bound = bound(*exclude, n, W, items);

    if (exclude->bound > this->maxProfit && Q.size() < MAX_QUEUE_SIZE) {
      Q.push(std::move(exclude)); // Give ownership to the queue
    }
  }

  // --- Set the taken items based on the best path found ---
  takenItems.assign(n, false);
  
  // Convert the best path to the original item indices
  for (int i = 0; i < n; i++) {
    if (bestPath[i]) {
      int originalIndex = items[i].index;
      takenItems[originalIndex] = true;
    }
  }

  return this->maxProfit;
}
