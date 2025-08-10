#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ITENS 2000 // limite máximo de itens para segurança

int W;                      // capacidade da mochila
int n;                      // número de itens
int pesos[MAX_ITENS];       // pesos dos itens
int valores[MAX_ITENS];     // valores dos itens
int melhor_valor = 0;       // melhor valor encontrado
int melhor_comb[MAX_ITENS]; // itens escolhidos na melhor solução

void backtrack(int i, int peso_atual, int valor_atual,
               int itens_selecionados[]) {
  if (i == n) { // caso base: testamos todos os itens
    if (valor_atual > melhor_valor && peso_atual <= W) {
      melhor_valor = valor_atual;
      for (int k = 0; k < n; k++)
        melhor_comb[k] = itens_selecionados[k];
    }
    return;
  }

  // Podar se já passou do peso
  if (peso_atual > W)
    return;

  // Escolher item i
  itens_selecionados[i] = 1;
  backtrack(i + 1, peso_atual + pesos[i], valor_atual + valores[i],
            itens_selecionados);

  // Não escolher item i
  itens_selecionados[i] = 0;
  backtrack(i + 1, peso_atual, valor_atual, itens_selecionados);
}

void ler_instancia(const char *nome_arquivo) {
  FILE *f = fopen(nome_arquivo, "r");
  if (!f) {
    printf("Erro ao abrir o arquivo %s\n", nome_arquivo);
    exit(1);
  }

  fscanf(f, "%d", &W);
  n = 0;
  while (fscanf(f, "%d %d", &pesos[n], &valores[n]) == 2) {
    n++;
    if (n >= MAX_ITENS)
      break;
  }
  fclose(f);
}

int main() {
  char arquivo[] =
      "instancia.txt"; // troque pelo nome do arquivo que deseja ler
  ler_instancia(arquivo);

  int itens_selecionados[MAX_ITENS] = {0};

  clock_t inicio = clock();
  backtrack(0, 0, 0, itens_selecionados);
  clock_t fim = clock();

  double tempo_execucao = (double)(fim - inicio) / CLOCKS_PER_SEC;

  printf("Lucro máximo: %d\n", melhor_valor);
  printf("Itens escolhidos: ");
  for (int i = 0; i < n; i++) {
    if (melhor_comb[i] == 1) {
      printf("%d ", i + 1); // imprime índice do item (1-based)
    }
  }
  printf("\nTempo de execução: %.6f segundos\n", tempo_execucao);

  return 0;
}
