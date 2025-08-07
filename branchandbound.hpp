#ifndef BRANCH_AND_BOUND_HPP
#define BRANCH_AND_BOUND_HPP

#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>

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
        Node *parent;
        bool TookItem = false;
    };

    vector<Item> items;

    static bool cmp(Item a, Item b); 

    int bound(Node u, int n, int W, Item arr[]);

    int knapsack();

    vector<bool> takenItems;

    int maxProfit = 0;

    vector<Node*> allocatedNodes; 

    void cleanup();
};  
#endif // BRANCH_AND_BOUND_HPP