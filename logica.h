#ifndef LOGICA_H
#define LOGICA_H

#include <stdbool.h>

// Enum para os tipos de nós na árvore lógica
typedef enum {
    ATOMO,          // P, Q, R, S
    NAO,            // ~
    E,              // ^
    OU,             // v
    CONDICIONAL,        // ->
    XOU,            // (+) Disjunção Exclusiva
    BICONDICIONAL   // <-> Bicondicional
} TipoOperador;

// Struct RECURSIVA
typedef struct Proposicao {
    TipoOperador tipo;
    char nomeAtomo;  
    bool valorAtomo; 
    struct Proposicao* esq;
    struct Proposicao* dir;
} Proposicao;

// --- Funções de Criação (Alocação Dinâmica) ---
Proposicao* criarAtomo(char nome, bool valorInicial);
Proposicao* criarOperador(TipoOperador tipo, Proposicao* esq, Proposicao* dir);
Proposicao* criarNegacao(Proposicao* filho);

// --- Função Principal (Recursividade) ---
bool avaliar(Proposicao* p);

// --- Funções Utilitárias ---
void destruirProposicao(Proposicao* p); // Limpa a memória
void atualizarValorAtomo(Proposicao* p, char nome, bool novoValor); // Atualiza P, Q, R, S na árvore
void imprimirExpressao(Proposicao* p, char* buffer); // Transforma a árvore em texto para desenhar na tela

#endif