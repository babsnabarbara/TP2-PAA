#ifndef DINAMYC_HPP
#define DINAMYC_HPP

#include <vector>

// Structure to represent an item
struct Item {
  int id;
  int weight;
  int value;
};

// Function to solve the 0/1 knapsack problem
void knapsack(int W, const std::vector<Item>& items);

#endif // DINAMYC_HPP
