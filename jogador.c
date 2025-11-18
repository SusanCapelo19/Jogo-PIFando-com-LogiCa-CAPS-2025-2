#include "jogador.h"
#include <stdlib.h>
#include <string.h> 

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