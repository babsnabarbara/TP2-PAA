#include "backtracking.hpp"
#include "branchandbound.hpp"
#include "dynamic.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional> // para std::function
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Resultado {
  int experimento;
  int n;
  int W;
  int instancia_id;
  string algoritmo;
  double tempo_seg;
  int lucro_max;
  vector<int> itens;
};

bool lerInstancia(const string &caminho, int &W,
                  vector<pair<float, int>> &itens) {
  ifstream arquivo(caminho);
  if (!arquivo.is_open()) {
    cerr << "Erro ao abrir arquivo: " << caminho << endl;
    return false;
  }

  arquivo >> W;
  itens.clear();

  float peso;
  int valor;
  while (arquivo >> peso >> valor) {
    itens.emplace_back(peso, valor);
  }

  arquivo.close();
  return true;
}

string itensParaJSON(const vector<int> &itens) {
  ostringstream oss;
  oss << "[";
  for (size_t i = 0; i < itens.size(); ++i) {
    oss << itens[i];
    if (i != itens.size() - 1)
      oss << ",";
  }
  oss << "]";
  return oss.str();
}

// Tipo do retorno esperado da função do algoritmo: lucro máximo + itens
// escolhidos (bool vector)
struct AlgoritmoResultado {
  int lucroMaximo;
  vector<bool> itensTomados;
};

// Função genérica para rodar algoritmo e medir tempo
// A funçãoAlgoritmo recebe (int W, const vector<pair<float,int>>& itens) e
// retorna AlgoritmoResultado
AlgoritmoResultado rodarAlgoritmo(
    const string &nomeAlgoritmo, int W, const vector<pair<float, int>> &itens,
    const function<AlgoritmoResultado(int, const vector<pair<float, int>> &)>
        &funcAlgoritmo) {
  auto start = chrono::high_resolution_clock::now();
  AlgoritmoResultado resultado = funcAlgoritmo(W, itens);
  auto end = chrono::high_resolution_clock::now();

  chrono::duration<double> duracao = end - start;
  cout << "Algoritmo " << nomeAlgoritmo << " executado em " << duracao.count()
       << " segundos\n";

  return resultado;
}

// Exemplo de wrapper para BranchAndBound que encaixa no padrão
AlgoritmoResultado
executarBranchAndBound(int W, const vector<pair<float, int>> &itens) {
  BranchAndBound bnb(W, itens);
  int lucro = bnb.getMaxProfit();
  vector<bool> taken = bnb.getTakenItems();
  return {lucro, taken};
}

AlgoritmoResultado executarDinamico(int W,
                                    const vector<pair<float, int>> &itens) {
  Dynamic dyn(W, itens);
  int lucro = dyn.knapsack();
  vector<bool> taken = dyn.getTakenItems();
  return {lucro, taken};
}

AlgoritmoResultado executarBacktracking(int W,
                                        const vector<pair<float, int>> &itens) {
  Backtracking bkt(W, itens);
  int lucro = bkt.solve();
  vector<bool> taken = bkt.getTakenItems();
  return {lucro, taken};
}

int main() {
  string diretorio = "experimento2";
  string arquivo_saida_bb = "resultados_branchandbound_2.csv";
  string arquivo_saida_dp = "resultados_dynamic_2.csv";
  string arquivo_saida_bt = "resultados_backtracking_2.csv";

  ofstream csv_bb(arquivo_saida_bb);
  if (!csv_bb.is_open()) {
    cerr << "Erro ao abrir arquivo de saída: " << arquivo_saida_bb << endl;
    return 1;
  }
  csv_bb
      << "experimento,n,W,instancia_id,algoritmo,tempo_seg,lucro_max,itens\n";

  ofstream csv_dp(arquivo_saida_dp);
  if (!csv_dp.is_open()) {
    cerr << "Erro ao abrir arquivo de saída: " << arquivo_saida_dp << endl;
    return 1;
  }
  csv_dp
      << "experimento,n,W,instancia_id,algoritmo,tempo_seg,lucro_max,itens\n";

  ofstream csv_bt(arquivo_saida_bt);
  if (!csv_bt.is_open()) {
    cerr << "Erro ao abrir arquivo de saída: " << arquivo_saida_bt << endl;
    return 1;
  }
  csv_bt
      << "experimento,n,W,instancia_id,algoritmo,tempo_seg,lucro_max,itens\n";

  for (const auto &entry : filesystem::directory_iterator(diretorio)) {
    string caminho_arquivo = entry.path().string();

    int n = 0, instancia_id = 0;
    size_t pos_n = caminho_arquivo.find("_n");
    size_t pos_underscore = caminho_arquivo.rfind("_");
    size_t pos_dot = caminho_arquivo.rfind(".");

    if (pos_n == string::npos || pos_underscore == string::npos ||
        pos_dot == string::npos) {
      cerr << "Nome de arquivo não no formato esperado: " << caminho_arquivo
           << endl;
      continue;
    }

    try {
      n = stoi(caminho_arquivo.substr(pos_n + 2, pos_underscore - (pos_n + 2)));
      instancia_id = stoi(caminho_arquivo.substr(
          pos_underscore + 1, pos_dot - (pos_underscore + 1)));
    } catch (...) {
      cerr << "Erro ao extrair n ou instancia_id de: " << caminho_arquivo
           << endl;
      continue;
    }

    int W;
    vector<pair<float, int>> itensEntrada;
    if (!lerInstancia(caminho_arquivo, W, itensEntrada)) {
      continue;
    }

    // --- Rodar Branch and Bound ---
    auto start_bb = chrono::high_resolution_clock::now();
    AlgoritmoResultado resultado_bb = rodarAlgoritmo(
        "BranchAndBound", W, itensEntrada, executarBranchAndBound);
    auto end_bb = chrono::high_resolution_clock::now();

    chrono::duration<double> duracao_bb = end_bb - start_bb;
    double tempo_seg_bb = duracao_bb.count();

    vector<int> itensEscolhidos_bb;
    for (size_t i = 0; i < resultado_bb.itensTomados.size(); i++) {
      if (resultado_bb.itensTomados[i])
        itensEscolhidos_bb.push_back((int)i);
    }

    csv_bb << 1 << "," << n << "," << W << "," << instancia_id << ","
           << "BranchAndBound" << "," << tempo_seg_bb << ","
           << resultado_bb.lucroMaximo << "," << '"'
           << itensParaJSON(itensEscolhidos_bb) << '"' << "\n";

    // --- Rodar Dinamico ---
    auto start_dp = chrono::high_resolution_clock::now();
    AlgoritmoResultado resultado_dp =
        rodarAlgoritmo("Dynamic", W, itensEntrada, executarDinamico);
    auto end_dp = chrono::high_resolution_clock::now();

    chrono::duration<double> duracao_dp = end_dp - start_dp;
    double tempo_seg_dp = duracao_dp.count();

    vector<int> itensEscolhidos_dp;
    for (size_t i = 0; i < resultado_dp.itensTomados.size(); i++) {
      if (resultado_dp.itensTomados[i])
        itensEscolhidos_dp.push_back((int)i);
    }

    csv_dp << 2 << "," << n << "," << W << "," << instancia_id << ","
           << "Dynamic" << "," << tempo_seg_dp << ","
           << resultado_dp.lucroMaximo << "," << '"'
           << itensParaJSON(itensEscolhidos_dp) << '"' << "\n";

    // --- Rodar Backtracking ---
    auto start_bt = chrono::high_resolution_clock::now();
    AlgoritmoResultado resultado_bt =
        rodarAlgoritmo("Backtracking", W, itensEntrada, executarBacktracking);
    auto end_bt = chrono::high_resolution_clock::now();

    chrono::duration<double> duracao_bt = end_bt - start_bt;
    double tempo_seg_bt = duracao_bt.count();

    vector<int> itensEscolhidos_bt;
    for (size_t i = 0; i < resultado_bt.itensTomados.size(); i++) {
      if (resultado_bt.itensTomados[i])
        itensEscolhidos_bt.push_back((int)i);
    }

    csv_bt << 3 << "," << n << "," << W << "," << instancia_id << ","
           << "Backtracking" << "," << tempo_seg_bt << ","
           << resultado_bt.lucroMaximo << "," << '"'
           << itensParaJSON(itensEscolhidos_bt) << '"' << "\n";

    cout << "Processado: " << caminho_arquivo << endl;
  }

  csv_bb.close();
  csv_dp.close();
  csv_bt.close();
  cout << "Resultados salvos em " << arquivo_saida_bb << ", "
       << arquivo_saida_dp << " e " << arquivo_saida_bt << endl;

  return 0;
}
