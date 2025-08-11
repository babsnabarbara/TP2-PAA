#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

// Estrutura para armazenar os resultados de um algoritmo
struct AlgoritmoResultado {
    string nome;
    vector<double> tempos;
    vector<int> lucros;
};

// Função para calcular a média
double calcularMedia(const vector<double>& valores) {
    if (valores.empty()) return 0.0;
    double soma = 0.0;
    for (double valor : valores) {
        soma += valor;
    }
    return soma / valores.size();
}

// Função para calcular o desvio padrão
double calcularDesvioPadrao(const vector<double>& valores, double media) {
    if (valores.size() <= 1) return 0.0;
    double somaQuadrados = 0.0;
    for (double valor : valores) {
        somaQuadrados += pow(valor - media, 2);
    }
    return sqrt(somaQuadrados / (valores.size() - 1));
}

// Função para calcular o erro padrão da média
double calcularErroPadrao(const vector<double>& valores, double desvioPadrao) {
    if (valores.empty()) return 0.0;
    return desvioPadrao / sqrt(valores.size());
}

// Função para calcular o teste t pareado
struct ResultadoTesteT {
    double estatisticaT;
    double valorP;
    double intervaloConfianca;
    bool rejeitaH0;
    string conclusao;
};

ResultadoTesteT testeTPareado(const vector<double>& algoritmo1, 
                              const vector<double>& algoritmo2, 
                              double nivelConfianca = 0.95) {
    ResultadoTesteT resultado;
    
    if (algoritmo1.size() != algoritmo2.size() || algoritmo1.size() < 2) {
        resultado.conclusao = "Dados insuficientes para o teste";
        return resultado;
    }
    
    int n = algoritmo1.size();
    
    // Calcular as diferenças
    vector<double> diferencas;
    for (int i = 0; i < n; i++) {
        diferencas.push_back(algoritmo1[i] - algoritmo2[i]);
    }
    
    // Calcular média e desvio padrão das diferenças
    double mediaDiferencas = calcularMedia(diferencas);
    double desvioDiferencas = calcularDesvioPadrao(diferencas, mediaDiferencas);
    
    // Calcular estatística t
    double erroPadrao = desvioDiferencas / sqrt(n);
    resultado.estatisticaT = mediaDiferencas / erroPadrao;
    
    // Graus de liberdade
    int grausLiberdade = n - 1;
    
    // Valor crítico t para 95% de confiança (aproximação)
    // Para n > 30, podemos usar aproximadamente 1.96
    double tCritico = (n > 30) ? 1.96 : 2.0; // Aproximação simplificada
    
    // Calcular intervalo de confiança
    resultado.intervaloConfianca = tCritico * erroPadrao;
    
    // Decisão: rejeitar H0 se |t| > t_crítico
    resultado.rejeitaH0 = abs(resultado.estatisticaT) > tCritico;
    
    // Determinar qual algoritmo é melhor
    if (resultado.rejeitaH0) {
        if (mediaDiferencas > 0) {
            resultado.conclusao = "Algoritmo 2 é estatisticamente melhor (95% confiança)";
        } else {
            resultado.conclusao = "Algoritmo 1 é estatisticamente melhor (95% confiança)";
        }
    } else {
        resultado.conclusao = "EMPATE ESTATÍSTICO - Não há diferença significativa (95% confiança)";
    }
    
    return resultado;
}

// Função para ler resultados dos arquivos CSV
vector<AlgoritmoResultado> lerResultadosCSV(const string& diretorio) {
    vector<AlgoritmoResultado> resultados;
    
    // Nomes dos arquivos
    vector<string> arquivos = {
        diretorio + "/resultados_branchandbound_2.csv",
        diretorio + "/resultados_dynamic_2.csv", 
        diretorio + "/resultados_backtracking_2.csv"
    };
    
    vector<string> nomes = {"BranchAndBound", "Dynamic", "Backtracking"};
    
    for (size_t i = 0; i < arquivos.size(); i++) {
        AlgoritmoResultado resultado;
        resultado.nome = nomes[i];
        
        ifstream arquivo(arquivos[i]);
        if (!arquivo.is_open()) {
            cerr << "Erro ao abrir: " << arquivos[i] << endl;
            continue;
        }
        
        string linha;
        getline(arquivo, linha); // Pular cabeçalho
        
        while (getline(arquivo, linha)) {
            stringstream ss(linha);
            string token;
            
            // Pular até a coluna do tempo
            for (int j = 0; j < 5; j++) {
                getline(ss, token, ',');
            }
            
            // Ler tempo
            getline(ss, token, ',');
            try {
                double tempo = stod(token);
                resultado.tempos.push_back(tempo);
            } catch (...) {
                // Ignorar linhas com erro
            }
        }
        
        arquivo.close();
        resultados.push_back(resultado);
    }
    
    return resultados;
}

// Função principal para análise estatística
void realizarAnaliseEstatistica() {
    cout << "=== ANÁLISE ESTATÍSTICA - TESTE T PARADO (95% CONFIANÇA) ===\n\n";
    
    // Ler resultados
    vector<AlgoritmoResultado> resultados = lerResultadosCSV(".");
    
    if (resultados.size() < 2) {
        cout << "Erro: Não foi possível ler resultados suficientes para análise.\n";
        return;
    }
    
    // Comparar cada par de algoritmos
    for (size_t i = 0; i < resultados.size(); i++) {
        for (size_t j = i + 1; j < resultados.size(); j++) {
            cout << "COMPARAÇÃO: " << resultados[i].nome << " vs " << resultados[j].nome << "\n";
            cout << "--------------------------------------------------------\n";
            
            // Estatísticas descritivas
            double media1 = calcularMedia(resultados[i].tempos);
            double media2 = calcularMedia(resultados[j].tempos);
            double desvio1 = calcularDesvioPadrao(resultados[i].tempos, media1);
            double desvio2 = calcularDesvioPadrao(resultados[j].tempos, media2);
            
            cout << resultados[i].nome << ":\n";
            cout << "  Média: " << media1 << " segundos\n";
            cout << "  Desvio Padrão: " << desvio1 << " segundos\n";
            cout << "  Número de instâncias: " << resultados[i].tempos.size() << "\n\n";
            
            cout << resultados[j].nome << ":\n";
            cout << "  Média: " << media2 << " segundos\n";
            cout << "  Desvio Padrão: " << desvio2 << " segundos\n";
            cout << "  Número de instâncias: " << resultados[j].tempos.size() << "\n\n";
            
            // Teste t pareado
            ResultadoTesteT teste = testeTPareado(resultados[i].tempos, resultados[j].tempos);
            
            cout << "RESULTADO DO TESTE T PARADO:\n";
            cout << "  Estatística t: " << teste.estatisticaT << "\n";
            cout << "  Intervalo de Confiança: ±" << teste.intervaloConfianca << " segundos\n";
            cout << "  Rejeita H0: " << (teste.rejeitaH0 ? "SIM" : "NÃO") << "\n";
            cout << "  CONCLUSÃO: " << teste.conclusao << "\n";
            cout << "\n";
        }
    }
    
    // Resumo final
    cout << "=== RESUMO FINAL ===\n";
    cout << "Para determinar qual algoritmo tem melhor desempenho:\n";
    cout << "1. Se rejeita H0: O algoritmo com menor tempo médio é estatisticamente melhor\n";
    cout << "2. Se não rejeita H0: Há EMPATE ESTATÍSTICO entre os algoritmos\n";
    cout << "3. Nível de confiança: 95%\n";
    cout << "4. H0: Não há diferença significativa entre os algoritmos\n";
    cout << "5. H1: Há diferença significativa entre os algoritmos\n";
}

int main() {
    realizarAnaliseEstatistica();
    return 0;
}
