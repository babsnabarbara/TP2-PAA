#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <functional>  // para std::function
#include "branchandbound.hpp"

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

bool lerInstancia(const string& caminho, int& W, vector<pair<float,int>>& itens) {
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

string itensParaJSON(const vector<int>& itens) {
    ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < itens.size(); ++i) {
        oss << itens[i];
        if (i != itens.size() - 1) oss << ",";
    }
    oss << "]";
    return oss.str();
}

// Tipo do retorno esperado da função do algoritmo: lucro máximo + itens escolhidos (bool vector)
struct AlgoritmoResultado {
    int lucroMaximo;
    vector<bool> itensTomados;
};

// Função genérica para rodar algoritmo e medir tempo
// A funçãoAlgoritmo recebe (int W, const vector<pair<float,int>>& itens) e retorna AlgoritmoResultado
AlgoritmoResultado rodarAlgoritmo(const string& nomeAlgoritmo,
                                 int W,
                                 const vector<pair<float,int>>& itens,
                                 const function<AlgoritmoResultado(int, const vector<pair<float,int>>&)>& funcAlgoritmo) {
    auto start = chrono::high_resolution_clock::now();
    AlgoritmoResultado resultado = funcAlgoritmo(W, itens);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> duracao = end - start;
    cout << "Algoritmo " << nomeAlgoritmo << " executado em " << duracao.count() << " segundos\n";

    return resultado;
}

// Exemplo de wrapper para BranchAndBound que encaixa no padrão
AlgoritmoResultado executarBranchAndBound(int W, const vector<pair<float,int>>& itens) {
    BranchAndBound bnb(W, itens);
    int lucro = bnb.getMaxProfit();
    vector<bool> taken = bnb.getTakenItems();
    return { lucro, taken };
}

int main() {
    string diretorio = "instancias";
    string arquivo_saida = "resultados.csv";

    ofstream csv(arquivo_saida);
    if (!csv.is_open()) {
        cerr << "Erro ao abrir arquivo de saída: " << arquivo_saida << endl;
        return 1;
    }
    csv << "experimento,n,W,instancia_id,algoritmo,tempo_seg,lucro_max,itens\n";

    for (const auto& entry : filesystem::directory_iterator(diretorio)) {
        string caminho_arquivo = entry.path().string();

        int n = 0, instancia_id = 0;
        size_t pos_n = caminho_arquivo.find("_n");
        size_t pos_underscore = caminho_arquivo.rfind("_");
        size_t pos_dot = caminho_arquivo.rfind(".");

        if (pos_n == string::npos || pos_underscore == string::npos || pos_dot == string::npos) {
            cerr << "Nome de arquivo não no formato esperado: " << caminho_arquivo << endl;
            continue;
        }

        try {
            n = stoi(caminho_arquivo.substr(pos_n + 2, pos_underscore - (pos_n + 2)));
            instancia_id = stoi(caminho_arquivo.substr(pos_underscore + 1, pos_dot - (pos_underscore + 1)));
        } catch (...) {
            cerr << "Erro ao extrair n ou instancia_id de: " << caminho_arquivo << endl;
            continue;
        }

        int W;
        vector<pair<float,int>> itensEntrada;
        if (!lerInstancia(caminho_arquivo, W, itensEntrada)) {
            continue;
        }

        // Rodar Branch and Bound (exemplo) usando a função genérica
        auto start_total = chrono::high_resolution_clock::now();
        AlgoritmoResultado resultado = rodarAlgoritmo("BranchAndBound", W, itensEntrada, executarBranchAndBound);
        auto end_total = chrono::high_resolution_clock::now();

        chrono::duration<double> duracao = end_total - start_total;
        double tempo_seg = duracao.count();

        // Converter vetor<bool> para vetor<int> com índices dos itens escolhidos
        vector<int> itensEscolhidos;
        for (size_t i = 0; i < resultado.itensTomados.size(); i++) {
            if (resultado.itensTomados[i]) itensEscolhidos.push_back((int)i);
        }

        int experimento = 2;
        string algoritmo = "BB";

        csv << experimento << "," 
            << n << "," 
            << W << "," 
            << instancia_id << "," 
            << algoritmo << ","
            << tempo_seg << ","
            << resultado.lucroMaximo << ","
            << '"' << itensParaJSON(itensEscolhidos) << '"'
            << "\n";

        cout << "Processado: " << caminho_arquivo << endl;
    }

    csv.close();
    cout << "Resultados salvos em " << arquivo_saida << endl;

    return 0;
}
