#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;

// Estrutura para armazenar os resultados de um algoritmo
struct AlgoritmoResultado {
    string nome;
    vector<double> tempos;
    vector<int> lucros;
    double media;
    double desvioPadrao;
    double erroPadrao;
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

// Função para calcular o valor p aproximado usando distribuição t
double calcularValorP(double estatisticaT, int grausLiberdade) {
    // Aproximação usando distribuição normal para n > 30
    if (grausLiberdade > 30) {
        // Para distribuição normal, P(|Z| > |t|) ≈ 2 * (1 - Φ(|t|))
        // Usando aproximação: Φ(x) ≈ 1 - 0.5 * exp(-x²/2)
        double x = abs(estatisticaT);
        if (x > 3.5) return 0.0005; // Muito significativo
        if (x > 3.0) return 0.003;  // Altamente significativo
        if (x > 2.5) return 0.012;  // Muito significativo
        if (x > 2.0) return 0.046;  // Significativo
        if (x > 1.96) return 0.05;  // Limite de 5%
        return 0.1; // Não significativo
    }
    
    // Para amostras pequenas, usar aproximação mais conservadora
    if (abs(estatisticaT) > 2.0) return 0.05;
    if (abs(estatisticaT) > 1.5) return 0.1;
    return 0.2;
}

// Função para calcular o teste t pareado
struct ResultadoTesteT {
    double estatisticaT;
    double valorP;
    double intervaloConfianca;
    bool rejeitaH0;
    string conclusao;
    double mediaDiferencas;
    double desvioDiferencas;
    int grausLiberdade;
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
    resultado.grausLiberdade = n - 1;
    
    // Calcular as diferenças
    vector<double> diferencas;
    for (int i = 0; i < n; i++) {
        diferencas.push_back(algoritmo1[i] - algoritmo2[i]);
    }
    
    // Calcular média e desvio padrão das diferenças
    resultado.mediaDiferencas = calcularMedia(diferencas);
    resultado.desvioDiferencas = calcularDesvioPadrao(diferencas, resultado.mediaDiferencas);
    
    // Calcular estatística t
    double erroPadrao = resultado.desvioDiferencas / sqrt(n);
    resultado.estatisticaT = resultado.mediaDiferencas / erroPadrao;
    
    // Calcular valor p
    resultado.valorP = calcularValorP(resultado.estatisticaT, resultado.grausLiberdade);
    
    // Valor crítico t para 95% de confiança
    double tCritico = (n > 30) ? 1.96 : 2.0;
    
    // Calcular intervalo de confiança
    resultado.intervaloConfianca = tCritico * erroPadrao;
    
    // Decisão: rejeitar H0 se valor p < 0.05
    resultado.rejeitaH0 = resultado.valorP < 0.05;
    
    // Determinar qual algoritmo é melhor
    if (resultado.rejeitaH0) {
        if (resultado.mediaDiferencas > 0) {
            resultado.conclusao = "Algoritmo 2 é estatisticamente melhor (p < 0.05)";
        } else {
            resultado.conclusao = "Algoritmo 1 é estatisticamente melhor (p < 0.05)";
        }
    } else {
        resultado.conclusao = "EMPATE ESTATÍSTICO - Não há diferença significativa (p >= 0.05)";
    }
    
    return resultado;
}

// Função para ler resultados dos arquivos CSV
vector<AlgoritmoResultado> lerResultadosCSV(const string& diretorio) {
    vector<AlgoritmoResultado> resultados;
    
    // Nomes dos arquivos
    vector<string> arquivos = {
        diretorio + "/resultados_branchandbound.csv",
        diretorio + "/resultados_dynamic.csv", 
        diretorio + "/resultados_backtracking.csv"
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
        
        // Calcular estatísticas
        resultado.media = calcularMedia(resultado.tempos);
        resultado.desvioPadrao = calcularDesvioPadrao(resultado.tempos, resultado.media);
        resultado.erroPadrao = calcularErroPadrao(resultado.tempos, resultado.desvioPadrao);
        
        resultados.push_back(resultado);
    }
    
    return resultados;
}

// Função para gerar relatório em CSV
void gerarRelatorioCSV(const vector<AlgoritmoResultado>& resultados, 
                       const vector<vector<ResultadoTesteT>>& comparacoes) {
    ofstream relatorio("relatorio_estatistico.csv");
    if (!relatorio.is_open()) {
        cerr << "Erro ao criar relatório CSV" << endl;
        return;
    }
    
    // Cabeçalho
    relatorio << "Algoritmo,Média (s),Desvio Padrão (s),Erro Padrão (s),Número de Instâncias\n";
    
    // Estatísticas descritivas
    for (const auto& resultado : resultados) {
        relatorio << resultado.nome << "," 
                 << fixed << setprecision(6) << resultado.media << ","
                 << resultado.desvioPadrao << ","
                 << resultado.erroPadrao << ","
                 << resultado.tempos.size() << "\n";
    }
    
    relatorio << "\nComparações,Estatística t,Valor p,Intervalo Confiança (±s),Rejeita H0,Conclusão\n";
    
    // Comparações
    vector<string> nomes = {"BranchAndBound", "Dynamic", "Backtracking"};
    int idx = 0;
    for (size_t i = 0; i < nomes.size(); i++) {
        for (size_t j = i + 1; j < nomes.size(); j++) {
            string comparacao = nomes[i] + " vs " + nomes[j];
            const auto& teste = comparacoes[i][j-i-1];
            
            relatorio << comparacao << ","
                     << fixed << setprecision(4) << teste.estatisticaT << ","
                     << setprecision(3) << teste.valorP << ","
                     << setprecision(6) << teste.intervaloConfianca << ","
                     << (teste.rejeitaH0 ? "SIM" : "NÃO") << ","
                     << teste.conclusao << "\n";
        }
    }
    
    relatorio.close();
    cout << "Relatório CSV salvo em: relatorio_estatistico.csv\n";
}

// Função principal para análise estatística
void realizarAnaliseEstatistica() {
    cout << "=== ANÁLISE ESTATÍSTICA AVANÇADA - TESTE T PARADO ===\n";
    cout << "Nível de Confiança: 95% (α = 0.05)\n\n";
    
    // Ler resultados
    vector<AlgoritmoResultado> resultados = lerResultadosCSV(".");
    
    if (resultados.size() < 2) {
        cout << "Erro: Não foi possível ler resultados suficientes para análise.\n";
        return;
    }
    
    // Armazenar todas as comparações
    vector<vector<ResultadoTesteT>> comparacoes;
    
    // Comparar cada par de algoritmos
    for (size_t i = 0; i < resultados.size(); i++) {
        vector<ResultadoTesteT> comparacoesAlgoritmo;
        
        for (size_t j = i + 1; j < resultados.size(); j++) {
            cout << "COMPARAÇÃO: " << resultados[i].nome << " vs " << resultados[j].nome << "\n";
            cout << string(60, '-') << "\n";
            
            // Estatísticas descritivas
            cout << resultados[i].nome << ":\n";
            cout << "  Média: " << fixed << setprecision(6) << resultados[i].media << " segundos\n";
            cout << "  Desvio Padrão: " << setprecision(6) << resultados[i].desvioPadrao << " segundos\n";
            cout << "  Erro Padrão: " << setprecision(6) << resultados[i].erroPadrao << " segundos\n";
            cout << "  Número de instâncias: " << resultados[i].tempos.size() << "\n\n";
            
            cout << resultados[j].nome << ":\n";
            cout << "  Média: " << setprecision(6) << resultados[j].media << " segundos\n";
            cout << "  Desvio Padrão: " << setprecision(6) << resultados[j].desvioPadrao << " segundos\n";
            cout << "  Erro Padrão: " << setprecision(6) << resultados[j].erroPadrao << " segundos\n";
            cout << "  Número de instâncias: " << resultados[j].tempos.size() << "\n\n";
            
            // Teste t pareado
            ResultadoTesteT teste = testeTPareado(resultados[i].tempos, resultados[j].tempos);
            comparacoesAlgoritmo.push_back(teste);
            
            cout << "RESULTADO DO TESTE T PARADO:\n";
            cout << "  Estatística t: " << setprecision(4) << teste.estatisticaT << "\n";
            cout << "  Graus de Liberdade: " << teste.grausLiberdade << "\n";
            cout << "  Valor p: " << setprecision(3) << teste.valorP << "\n";
            cout << "  Intervalo de Confiança: ±" << setprecision(6) << teste.intervaloConfianca << " segundos\n";
            cout << "  Rejeita H0: " << (teste.rejeitaH0 ? "SIM" : "NÃO") << "\n";
            cout << "  CONCLUSÃO: " << teste.conclusao << "\n";
            cout << "\n";
        }
        
        comparacoes.push_back(comparacoesAlgoritmo);
    }
    
    // Resumo final
    cout << "=== RESUMO FINAL ===\n";
    cout << "Hipóteses:\n";
    cout << "  H0: Não há diferença significativa entre os algoritmos\n";
    cout << "  H1: Há diferença significativa entre os algoritmos\n";
    cout << "  α = 0.05 (5% de significância)\n\n";
    
    cout << "Interpretação:\n";
    cout << "  - Se p < 0.05: Rejeita H0 (diferença significativa)\n";
    cout << "  - Se p >= 0.05: Não rejeita H0 (empate estatístico)\n\n";
    
    cout << "Ranking de Desempenho (por tempo médio):\n";
    vector<pair<string, double>> ranking;
    for (const auto& resultado : resultados) {
        ranking.push_back({resultado.nome, resultado.media});
    }
    
    sort(ranking.begin(), ranking.end(), 
         [](const auto& a, const auto& b) { return a.second < b.second; });
    
    for (size_t i = 0; i < ranking.size(); i++) {
        cout << "  " << (i+1) << "º lugar: " << ranking[i].first 
             << " (" << fixed << setprecision(6) << ranking[i].second << "s)\n";
    }
    
    // Gerar relatório CSV
    gerarRelatorioCSV(resultados, comparacoes);
}

int main() {
    realizarAnaliseEstatistica();
    return 0;
}
