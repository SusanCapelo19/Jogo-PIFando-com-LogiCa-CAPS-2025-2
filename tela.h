#ifndef TELA_H
#define TELA_H

#include <stdbool.h>
#include "screen.h"
#include "logica.h"

// Funções base
void telaInit();
void telaFinish();
void telaClear();
void telaRefresh();
void telaDrawText(int x, int y, char* texto);
int  telaGetKey();

// Funções de Cor
void telaSetColor(screenColor fg, screenColor bg);

// Funções de Desenho
void telaDesenharJanela(int x1, int y1, int x2, int y2, const char* titulo);

void telaDesenharMenuInicial(const char* nomeDigitado, int dificuldadeSelecionada);

void telaDesenharTextoQuebrado(int x, int y, const char* texto, int larguraMaxima);

void telaDesenharCutsceneInicial(void);

void telaDesenharCutsceneForca(void);

void telaDesenharFaseForca(int erros, const char* letrasDescobertas, const char* letrasTentadas, const char* nome, int pontuacao);

void telaDesenharVitoriaFase1(int pontuacao);

void telaDesenharCutsceneBomba(void);

void telaDesenharFaseBomba(bool alvo[9], struct Proposicao* matriz[9], bool atomos[3], int selecionado, int tempoRestante, bool ajudaAtiva, const char* textoAjuda, const char* nome, int pontuacao);

void telaDesenharFaseGalaga(const char* nome, int pontuacao);

void telaDesenharGameOver(bool venceu, int pontuacaoFinal);

#endif