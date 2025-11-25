#ifndef FASE3_GALAGA_H
#define FASE3_GALAGA_H

#include <stdbool.h>
#include "jogador.h"

// Configurações da Fase
#define MAX_TIROS 100       
#define MAX_TIROS_INIMIGOS 50 
#define MAX_INIMIGOS 30      
#define MAX_ESTRELAS 80

// Tipos de Tiro da Nave
typedef enum {
    TIRO_SIMPLES, 
    TIRO_DUPLO,   
    TIRO_TRIPLO,  
    TIRO_MASTER   
} TipoArma;

// Estrutura da Nave do Jogador
typedef struct {
    int x, y;
    int hp;
    bool escudoAtivo;
    TipoArma armaAtual;
} NaveJogador;

// Estrutura do Inimigo
typedef struct {
    int x, y;
    bool vivo;
    int tipo;    // 0 = Comum, 1 = Boss
    int direcao; // 1 (Direita) ou -1 (Esquerda)
    int linha;   // 0, 1, 2... usado para lógica de tiro (cima/baixo)
} Inimigo;

// Estrutura do Tiro
typedef struct {
    int x, y;
    bool ativo;
    bool ehMaster; 
} Tiro;

// Estrutura para o fundo (estrelas)
typedef struct {
    int x, y;
    int cor; 
} Estrela;

// Função Principal
bool executarFaseGalaga(Jogador* jogador);

#endif