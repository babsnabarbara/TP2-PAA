#include "backtracking.hpp"
#include <algorithm>
#include <numeric>

Backtracking::Backtracking(int W, const std::vector<std::pair<float, int>>& items) {
    this->W = W;
    this->n = items.size();
    
    std::vector<int> p(n);
    std::iota(p.begin(), p.end(), 0);

    std::sort(p.begin(), p.end(), [&](int i, int j) {
        return (double)items[i].second / items[i].first > (double)items[j].second / items[j].first;
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

double Backtracking::upperBound(int i, int current_weight, int current_value) const {
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

void Backtracking::backtrack(int i, int current_weight, int current_value, std::vector<bool>& current_selection) {
    if (upperBound(i, current_weight, current_value) <= maxProfit) {
        return;
    }

    if (i == n) {
        if (current_value > maxProfit) {
            maxProfit = current_value;
            takenItems = current_selection;
        }
        return;
    }

    if (current_weight + weights[i] <= W) {
        current_selection[i] = true;
        backtrack(i + 1, current_weight + weights[i], current_value + values[i], current_selection);
    }

    current_selection[i] = false;
    backtrack(i + 1, current_weight, current_value, current_selection);
}

int Backtracking::solve() {
    std::vector<bool> current_selection(n, false);
    backtrack(0, 0, 0, current_selection);

    std::vector<bool> original_order_taken_items(n, false);
    for(int i=0; i<n; ++i) {
        if(takenItems[i]) {
            original_order_taken_items[originalIndices[i]] = true;
        }
    }
    takenItems = original_order_taken_items;

    return maxProfit;
}

int Backtracking::getMaxProfit() const {
    return maxProfit;
}

const std::vector<bool>& Backtracking::getTakenItems() const {
    return takenItems;
}
