#ifndef BRANCH_AND_BOUND_HPP
#define BRANCH_AND_BOUND_HPP

#include <vector>
#include <algorithm>
#include <memory>

using namespace std;

class BranchAndBound {
public:
    BranchAndBound(int capacidade, const vector<pair<float, int>>& itensEntrada)
        : capacidade(capacidade) {
            transform(itensEntrada.begin(), itensEntrada.end(), back_inserter(items),
                [idx = 0] (const pair<float, int>& p) mutable {
                    return Item{static_cast<float>(p.first), p.second, idx++};
                });
                
            maxProfit = knapsack();
                
        }
    ~BranchAndBound() = default; // Use default destructor since we're using smart pointers
    const vector<bool>& getTakenItems() const { return takenItems; }
    int getMaxProfit() const { return maxProfit; }

private:
    int capacidade;
    // Structure for Item which store weight and corresponding
    // value of Item

    struct Item {
        float weight;
        int value;
        int index;
    };

    struct Node{
        int level, profit, bound, index;
        float weight;
        vector<bool> path; // Track the path to this node
        bool TookItem = false;
        
        // Constructor to initialize members
        Node() : level(0), profit(0), bound(0), index(0), weight(0), TookItem(false) {}
    };

    vector<Item> items;

    static bool cmp(Item a, Item b); 

    int bound(Node u, int n, int W, Item arr[]);

    int knapsack();

    vector<bool> takenItems;

    int maxProfit = 0;

    // Remove allocatedNodes vector and cleanup function - no longer needed
};
#endif // BRANCH_AND_BOUND_HPP
