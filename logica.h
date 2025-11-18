#ifndef LOGICA_H
#define LOGICA_H

#include <stdbool.h> 

// Enum para os operadores lógicos
typedef enum {
    ATOMO,  // p, q, r
    NAO,    // ~
    E,      // ∧
    OU,     // ∨
    IMPLICA // ->
} TipoOperador;

// Struct RECURSIVA para a árvore de expressão
typedef struct Proposicao {
    TipoOperador tipo;

    char* nomeAtomo;
    bool valorAtomo; // O valor V/F que o jogador define

    // Se for OPERADOR (NAO, E, OU, IMPLICA)
    struct Proposicao* esq; 
    struct Proposicao* dir; 
} Proposicao;

// Protótipos das funções
Proposicao* criarAtomo(char* nome);
Proposicao* criarOperador(TipoOperador tipo, Proposicao* esq, Proposicao* dir);
void destruirProposicao(Proposicao* prop); // Importante para liberar memória!

// A função RECURSIVA principal
bool avaliar(Proposicao* prop);

#endif