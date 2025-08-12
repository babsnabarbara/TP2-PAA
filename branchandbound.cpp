#include "branchandbound.hpp"
#include <stack>
#include <climits>

// Comparison function for sorting items by value/weight ratio (descending)
bool BranchAndBound::cmp(Item a, Item b) {
    return (a.value / a.weight) > (b.value / a.weight);
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

    // Usando stack para DFS
    stack<unique_ptr<Node>> S;

    // Nó raiz
    auto root = make_unique<Node>();
    root->level = -1;
    root->profit = 0;
    root->weight = 0;
    root->path.resize(n, false);
    S.push(std::move(root));

    vector<bool> bestPath(n, false);
    this->maxProfit = 0;

    const int MAX_STACK_SIZE = 1000000;
    int nodesProcessed = 0;

    while (!S.empty() && nodesProcessed < MAX_STACK_SIZE) {
        nodesProcessed++;

        unique_ptr<Node> u = std::move(S.top());
        S.pop();

        if (u->level == n - 1) {
            continue;
        }

        int nextLevel = u->level + 1;

        // --- Branch 1: Include the item ---
        auto include = make_unique<Node>();
        include->level = nextLevel;
        include->weight = u->weight + items[nextLevel].weight;
        include->profit = u->profit + items[nextLevel].value;
        include->path = u->path;
        include->path[nextLevel] = true;
        include->index = items[nextLevel].index;
        include->TookItem = true;

        if (include->weight <= W && include->profit > this->maxProfit) {
            this->maxProfit = include->profit;
            bestPath = include->path;
        }

        include->bound = bound(*include, n, W, items);

        if (include->bound > this->maxProfit && S.size() < MAX_STACK_SIZE) {
            S.push(std::move(include)); // DFS: empilha para explorar já
        }

        // --- Branch 2: Exclude the item ---
        auto exclude = make_unique<Node>();
        exclude->level = nextLevel;
        exclude->weight = u->weight;
        exclude->profit = u->profit;
        exclude->path = u->path;
        exclude->path[nextLevel] = false;
        exclude->index = items[nextLevel].index;
        exclude->TookItem = false;

        exclude->bound = bound(*exclude, n, W, items);

        if (exclude->bound > this->maxProfit && S.size() < MAX_STACK_SIZE) {
            S.push(std::move(exclude));
        }
    }

    // Reconstruindo itens escolhidos
    takenItems.assign(n, false);
    for (int i = 0; i < n; i++) {
        if (bestPath[i]) {
            int originalIndex = items[i].index;
            takenItems[originalIndex] = true;
        }
    }

    return this->maxProfit;
}
