#include "branchandbound.hpp"

// C++ program to solve knapsack problem using
// branch and bound
#include <bits/stdc++.h>
using namespace std;


void BranchAndBound::cleanup() {
    for (Node* node : allocatedNodes) {
        delete node;
    }
    allocatedNodes.clear();
}


// Comparison function to sort Item according to
// val/weight ratio
bool BranchAndBound::cmp(Item a, Item b)
{
    double r1 = (double)a.value / a.weight;
    double r2 = (double)b.value / b.weight;
    return r1 > r2;
}

// Returns bound of profit in subtree rooted with u.
// This function mainly uses Greedy solution to find
// an upper bound on maximum profit.
int BranchAndBound::bound(Node u, int n, int W, Item arr[])
{
    // if weight overcomes the knapsack capacity, return
    // 0 as expected bound
    if (u.weight >= W)
        return 0;

    // initialize bound on profit by current profit
    int profit_bound = u.profit;

    // start including items from index 1 more to current
    // item index
    int j = u.level + 1;
    int totweight = u.weight;

    // checking index condition and knapsack capacity
    // condition
    while ((j < n) && (totweight + arr[j].weight <= W))
    {
        totweight += arr[j].weight;
        profit_bound += arr[j].value;
        j++;
    }

    // If k is not n, include last item partially for
    // upper bound on profit
    if (j < n)
        profit_bound += (W - totweight) * arr[j].value /
                                        arr[j].weight;

    return profit_bound;
}

// Returns maximum profit we can get with capacity W
int BranchAndBound::knapsack()
{
    // To keep track of allocated nodes
    int n = this->items.size();
    int W = this->capacidade;
    Item* items = this->items.data();
    
    // sorting Item on basis of value per unit weight.
    sort(items, items + n, cmp);

    // make a queue for traversing the node
    queue<Node*> Q;
    Node* u = new Node;
    allocatedNodes.push_back(u);
    Node* v = nullptr;

    // dummy node at starting
    u->level = -1;
    u->profit = u->weight = 0;
    Q.push(u);

    // One by one extract an item from decision tree
    // compute profit of all children of extracted item
    // and keep saving this->maxProfit

    Node* bestNode = nullptr;
    
    while (!Q.empty())
    {
        Node* u = Q.front();
        Q.pop();

        // If it is starting node, assign level 0
        if (u->level == -1) {
            v = new Node(*u);
            v->level = 0;
            allocatedNodes.push_back(v);
        }

        // If there is nothing on next level
        if (u->level == n-1) {
            delete u;
            continue;
        }

        // Else if not last node, then increment level,
        // and compute profit of children nodes.
        Node* include = new Node;
        include->level = u->level + 1;
        include->weight = u->weight + items[include->level].weight;
        include->profit = u->profit + items[include->level].value;
        include->index = items[include->level].index;
        include->parent = u;
        allocatedNodes.push_back(include);

        if (include->weight <= W && include->profit > this->maxProfit)
        {
            this->maxProfit = include->profit;
            if(bestNode) delete bestNode;
            bestNode = new Node (*include);
            bestNode->parent = include->parent;
        }

        include->bound = bound(*include, n, W, items);

        if (include->bound > this->maxProfit)
            Q.push(include);
     
        Node* exclude = new Node;
        exclude->level = u->level + 1;
        exclude->weight = u->weight;
        exclude->profit = u->profit;
        exclude->parent = u;
        exclude->index = items[exclude->level].index;
        exclude->bound = bound(*exclude, n, W, items);
        allocatedNodes.push_back(exclude);
        
        if (exclude->bound > this->maxProfit)
            Q.push(exclude);
     
        
     //   delete u; // Clean up the node to prevent memory leak
    }

    takenItems = vector<bool>(n, false);  // reinicializa o vetor
    Node* cur = bestNode;
    allocatedNodes.push_back(bestNode); // Ensure bestNode is also tracked for deletion
    
    while (cur && cur->level >= 0) {
        Node* p = cur->parent;
        if (p && cur->weight != p->weight) {
            int originalIndex = cur->index;
            // Mark the item as taken
            this->takenItems[originalIndex] = true;
        }
        cur = p;
    }


    return this->maxProfit;
}

