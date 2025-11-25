#ifndef JOGADOR_H
#define JOGADOR_H

// Enum para o nível de dificuldade
typedef enum {
    FACIL,
    MEDIO,
    DIFICIL
} NivelDificuldade;

// Struct para os dados do jogador
typedef struct {
    char* nome;
    int pontuacao;
    NivelDificuldade nivel;
} Jogador;

// Struct para salvar o ranking
typedef struct {
    char nome[50];
    int pontuacao;
} RegistroRank;

// Protótipos das funções
Jogador* criarJogador(char* nome, NivelDificuldade nivel);
void destruirJogador(Jogador* jogador);

// --- Protótipos de Ranking ---
// Lê o arquivo e preenche o array, retorna quantos leu
int carregarRanking(RegistroRank ranking[5]); 
// Adiciona pontos, ordena e salva no arquivo
void salvarPontuacao(const char* nome, int pontuacao);

#endif