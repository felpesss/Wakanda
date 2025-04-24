#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cidades.h"

// Remover espaços em branco no início e no final de uma string
void trim(char *str) {
    if (str == NULL || *str == '\0') return;

    int inicio = 0;
    while (isspace((unsigned char)str[inicio])) { // Enquanto os caracteres do inicio forem em branco
        inicio++;
    }

    int fim = strlen(str) - 1; // Obteção do índice do último caractere
    while (fim >= inicio && isspace((unsigned char)str[fim])) { // Enquanto o índice do fim for maior ou igual ao início e o caractere no fim for um espaço em branco

        fim--;
    }

    str[fim + 1] = '\0';// Adiciona o terminador nulo após o último caractere não branco
    if (inicio > 0) {
        memmove(str, str + inicio, fim - inicio + 2);  // Move a parte não branca da string para o início
    }
}
// funcao para comparar e ordenar cidades com base em sua posição
int compararCidades(const void *a, const void *b) {
    const Cidade *cd = (const Cidade *)a; // Converte o ponteiro temp para um ponteiro de Cidade
    const Cidade *co = (const Cidade *)b;
    return cd->Posicao - co->Posicao; // Retorno da diferença entre as posições para a ordenar
}
// funcao para ler os dados da estrada de um arquivo
Estrada *getEstrada(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) return NULL;

    int T, N; // Declara variáveis para o tamanho total da estrada e o número de cidades descrito no header dado pelo professor

// Leitura do tamanho total da estrada (T) do arquivo
    if (fscanf(arquivo, "%d", &T) != 1 || T < 3 || T > 1000000) {
        fclose(arquivo);
        return NULL;
    }
// Leitura do número de cidades (N) do arquivo
    if (fscanf(arquivo, "%d", &N) != 1 || N < 2 || N > 10000) {
        fclose(arquivo);
        return NULL;
    }
    // Aloca memoria para a estrutura Estrada
    Estrada *estrada = (Estrada *)malloc(sizeof(Estrada));
    if (!estrada) {
        fclose(arquivo);
        return NULL;
    }

    estrada->T = T;
    estrada->N = N;
    estrada->C = (Cidade *)malloc(N * sizeof(Cidade));
    if (!estrada->C) {
        free(estrada);
        fclose(arquivo);
        return NULL;
    }

    int c;
    while ((c = fgetc(arquivo)) != '\n' && c != EOF); // Consome a linha restante apos a leitura de N

 // Loop para letira dos dados de cada cidade
    int i;
    for (i = 0; i < N; i++) {
        char linha[256];
        if (fgets(linha, sizeof(linha), arquivo) == NULL) {
            free(estrada->C);
            free(estrada);
            fclose(arquivo);
            return NULL;
        }

        int pos; // Var para armazenar a posição da cidade
        char nome[256]; 
        if (sscanf(linha, "%d %255[^\n]", &pos, nome) != 2 || pos <= 0 || pos >= T) {
            free(estrada->C);
            free(estrada);
            fclose(arquivo);
            return NULL;
        }

        trim(nome); // Remoção dos espaços em branco do início e do final do nome da cidade
        if (nome[0] == '\0') {
            free(estrada->C);
            free(estrada);
            fclose(arquivo);
            return NULL;
        }

        estrada->C[i].Posicao = pos;
        strncpy(estrada->C[i].Nome, nome, 255);
        estrada->C[i].Nome[255] = '\0';
    }

    qsort(estrada->C, N, sizeof(Cidade), compararCidades);


    for (i = 0; i < N - 1; i++) {
        if (estrada->C[i].Posicao == estrada->C[i + 1].Posicao) {
            free(estrada->C);
            free(estrada);
            fclose(arquivo);
            return NULL;
        }
    }

    fclose(arquivo);
    return estrada;
}
// funcao para calcular o tamanho da menor vizinhanca entre as cidades
double calcularMenorVizinhanca(const char *nomeArquivo) {
    Estrada *estrada = getEstrada(nomeArquivo);
    if (!estrada) return 0.0;

    double menor = -1.0;
 // Loop para calcular a vizinhanca de cada cidade
    int i;
    for (i = 0; i < estrada->N; i++) {
        double inicio, fim;
  // Define o limite esquerdo da vizinhanca
        if (i == 0) {
            inicio = 0.0;
        } else {// Para o restante das cidades
            inicio = (estrada->C[i - 1].Posicao + estrada->C[i].Posicao) / 2.0;
        }
// Define o limite direito da vizinhaca
        if (i == estrada->N - 1) { // Se for a ultima cidade
            fim = (double)estrada->T;// O fim da vizinhanca é o final da estrada
        } else {
            fim = (estrada->C[i].Posicao + estrada->C[i + 1].Posicao) / 2.0;
        }

        double tamanho = fim - inicio; // Calc o tam da vizinhanca
        if (menor < 0 || tamanho < menor) {
            menor = tamanho;
        }
    }

    free(estrada->C);
    free(estrada);
    return menor;
}
// Funcao para o nome da cidade com a menor vizinhanca
char *cidadeMenorVizinhanca(const char *nomeArquivo) {
    Estrada *estrada = getEstrada(nomeArquivo);
    if (!estrada) return NULL;

    double menor = -1.0;
    int indice = -1;
    // Loop para encontrar a cidade com a menor vizinhanca
    int i;
    for (i = 0; i < estrada->N; i++) {
        double inicio, fim;

        if (i == 0) {
            inicio = 0.0;
        } else {
            inicio = (estrada->C[i - 1].Posicao + estrada->C[i].Posicao) / 2.0; // O inicio e o ponto medio entre a cidade atual e a anterior

        }

        if (i == estrada->N - 1) {
            fim = (double)estrada->T;
        } else {
            fim = (estrada->C[i].Posicao + estrada->C[i + 1].Posicao) / 2.0;
        }

        double tamanho = fim - inicio;
        if (menor < 0 || tamanho < menor) { // Se for a primeira vizinhança calculada ou se o tamanho atual for menor que o menor encontrado
            menor = tamanho; // Atualiza o valor da menor vizinhança
            indice = i;
        }
    }
// Se nenhum indice foi encontrado
    if (indice == -1) {
        free(estrada->C);
        free(estrada);
        return NULL;
    }
 // Aloca memoria para o nome da cidade com a menor vizinhanca
    char *nome = (char *)malloc(strlen(estrada->C[indice].Nome) + 1);
    if (!nome) {
        free(estrada->C);
        free(estrada);
        return NULL;
    }

    strcpy(nome, estrada->C[indice].Nome); // Copia o nome da cidade para a memora aloc
    free(estrada->C);
    free(estrada);
    return nome; // Retorna o ponteiro para o nome da cidade com a menor vizinhanca
}