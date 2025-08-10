#ifndef DYNAMIC_HPP
#define DYNAMIC_HPP

#include <vector>
#include <utility>

// Structure to represent an item
struct Item {
    int id;
    int weight;
    int value;
};

class Dynamic {
public:
    Dynamic(int W, const std::vector<std::pair<float, int>>& items);
    int knapsack();
    int getMaxProfit() const;
    const std::vector<bool>& getTakenItems() const;

private:
    int W;
    std::vector<Item> items;
    int maxProfit;
    std::vector<bool> takenItems;
};

#endif // DYNAMIC_HPP