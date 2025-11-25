#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "jogador.h"

#define ARQ_RANKING "ranking.dat"
#define MAX_RANK 5

Jogador* criarJogador(char* nome, NivelDificuldade nivel) {
    Jogador* novoJogador = (Jogador*) malloc(sizeof(Jogador));
    if (novoJogador == NULL) return NULL; // Falha de alocação

    // Aloca memória para o nome e copia
    novoJogador->nome = (char*) malloc(strlen(nome) + 1);
    if (novoJogador->nome == NULL) {
        free(novoJogador);
        return NULL;
    }
    strcpy(novoJogador->nome, nome);
    
    novoJogador->pontuacao = 0;
    novoJogador->nivel = nivel;
    
    return novoJogador;
}

void destruirJogador(Jogador* jogador) {
    if (jogador != NULL) {
        free(jogador->nome); // Libera a string do nome
        free(jogador);       // Libera a struct
    }
}

int carregarRanking(RegistroRank ranking[MAX_RANK]) {
    FILE *f = fopen(ARQ_RANKING, "rb");
    int total = 0;

    if (f == NULL) {
        // Se não existir, zera o ranking
        for (int i = 0; i < MAX_RANK; i++) {
            strcpy(ranking[i].nome, "---");
            ranking[i].pontuacao = 0;
        }
        return 0;
    }

    total = fread(ranking, sizeof(RegistroRank), MAX_RANK, f);
    fclose(f);
    return total;
}

// Salva nova pontuação, mantendo top 5
void salvarPontuacao(const char* nome, int pontuacao) {
    RegistroRank ranking[MAX_RANK];
    carregarRanking(ranking);

    // Cria novo registro
    RegistroRank novo;
    strncpy(novo.nome, nome, 49);
    novo.nome[49] = '\0';
    novo.pontuacao = pontuacao;

    // Insere na posição correta
    for (int i = 0; i < MAX_RANK; i++) {
        if (pontuacao > ranking[i].pontuacao) {
            // Empurra os piores para baixo
            for (int j = MAX_RANK - 1; j > i; j--) {
                ranking[j] = ranking[j - 1];
            }
            ranking[i] = novo;
            break;
        }
    }

    // Salva no arquivo
    FILE *f = fopen(ARQ_RANKING, "wb");
    if (f != NULL) {
        fwrite(ranking, sizeof(RegistroRank), MAX_RANK, f);
        fclose(f);
    }
}
