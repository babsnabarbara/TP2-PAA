#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

namespace fs = std::filesystem;

int main() {
  std::random_device rd;
  std::mt19937 gen(rd());

  int n_inicial = 100;
  int n_final = 900;
  int passo = 100;
  int instancias = 10;

  fs::create_directory("experimento2");

  for (int n = n_inicial, W = n_inicial; n <= n_final; n += passo, W += passo) {
    std::uniform_int_distribution<> valor_dist(1, 1000);
    std::uniform_int_distribution<> peso_dist(1, n / 4);

    for (int inst = 1; inst <= instancias; inst++) {
      std::string nome_arquivo = "experimento2/instancia_n" +
                                 std::to_string(n) + "_W" + std::to_string(W) +
                                 "_" + std::to_string(inst) + ".txt";

      std::ofstream arquivo(nome_arquivo);
      arquivo << W << "\n";
      for (int i = 0; i < n; i++) {
        arquivo << peso_dist(gen) << " " << valor_dist(gen) << "\n";
      }
      arquivo.close();
    }
  }

  std::cout << "Arquivos do Experimento 2 gerados na pasta 'experimento2'\n";
  return 0;
}
