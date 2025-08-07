#include <iostream>
#include "branchandbound.hpp"

void printTakenItems(const vector<bool>& items) {
    cout << "Itens escolhidos: ";
    for (size_t i = 0; i < items.size(); i++) {
        cout << items[i] << " ";  // imprime 0 ou 1
    }
    cout << endl;
}

int main (){

    int capacidade = 10;
    vector<pair<float, int>> itensEntrada = { {2, 40}, {3.14, 50}, {1.98, 100}, {5, 95}, {3, 30} };

    BranchAndBound bnb(capacidade, itensEntrada);
    int lucroMaximo = bnb.getMaxProfit();
    bnb.getTakenItems();


    cout << "Lucro máximo: " << lucroMaximo << endl;
    cout <<"Imprimi lucro"<<endl;
    printTakenItems(bnb.getTakenItems());

    return 0;
}