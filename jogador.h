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

// Protótipos das funções
Jogador* criarJogador(char* nome, NivelDificuldade nivel);
void destruirJogador(Jogador* jogador);

#endif