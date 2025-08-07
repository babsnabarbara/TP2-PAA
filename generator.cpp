#include <iostream>
#include <fstream>
#include <random>
#include <filesystem>

using namespace std;

int main() {
    const int W = 100; // Capacidade da mochila
    const int num_instancias = 10;
    const int inicio = 100;
    const int fim = 1100;
    const int passo = 100;

    // Criar diretório se não existir (C++17)
    filesystem::create_directory("instancias");

    // Inicializa gerador de números aleatórios
    random_device rd;
    mt19937 gen(rd());

    for (int n = inicio; n <= fim; n += passo) {
        uniform_int_distribution<> valor_dist(1, 1000);
        uniform_int_distribution<> peso_dist(1, n / 4);

        for (int i = 1; i <= num_instancias; i++) {
            string nome_arquivo = "instancias/instancia_n" + to_string(n) + "_" + to_string(i) + ".txt";
            ofstream arquivo(nome_arquivo);

            if (!arquivo.is_open()) {
                cerr << "Erro ao criar o arquivo: " << nome_arquivo << endl;
                continue;
            }

            arquivo << W << "\n";

            for (int j = 0; j < n; j++) {
                int peso = peso_dist(gen);
                int valor = valor_dist(gen);
                arquivo << peso << "\t" << valor << "\n";
            }

            arquivo.close();
            cout << "Gerado: " << nome_arquivo << endl;
        }
    }

    return 0;
}


