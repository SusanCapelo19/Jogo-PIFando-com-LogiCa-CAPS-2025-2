#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>     
#include "jogador.h"
#include "tela.h"
#include "fase1_forca.h"
#include "fase2_bomba.h"
#include "fase3_galaga.h"

// Enum para controlar o estado do jogo
typedef enum {
    MENU,
    CUTSCENE_INICIAL,
    CUTSCENE_FORCA,
    FASE_1,
    FASE_2,
    FASE_3,
    GAME_OVER,
    SAIR
} EstadoJogo;

int main() {

    telaInit(); // Inicia a cli-lib

    EstadoJogo estadoAtual = MENU;
    bool rodando = true;
    bool venceu = false;
    Jogador* jogador = NULL;

    // --- Variáveis de estado para o Menu ---
    char nomeJogador[50];
    memset(nomeJogador, 0, 50); // Limpa a string
    int nomeIndex = 0;
    int dificuldadeIdx = 0; // 0=Facil, 1=Medio, 2=Dificil
    NivelDificuldade nivel;
    
    // Estado do menu: 0 = digitando nome, 1 = escolhendo dificuldade
    int estadoMenu = 0; 

    // --- LOOP PRINCIPAL DO JOGO ---
    while (rodando) {
        
        telaClear(); // Limpa a tela no início de cada frame

        switch (estadoAtual) {
            
            // --- LÓGICA DO MENU INICIAL ---
            case MENU:
                
                // Desenha o menu na tela 
                telaDesenharMenuInicial(nomeJogador, dificuldadeIdx);
                
                // Coloca o cursor no lugar certo
                if (estadoMenu == 0) { 
                    
                    telaDrawText(20 + 17 + nomeIndex, 10, "_"); 
                }

                telaRefresh(); // Mostra o desenho
                int key = telaGetKey(); // Espera uma tecla

                // Processa o Input
                if (estadoMenu == 0) { 
                    // --- Estado 0: Digitandor Nome ---
                    
                    if (key == 13 || key == 10) { // ENTER
                        if (nomeIndex > 0) { 
                            estadoMenu = 1; // Vai para a seleção de dificuldade
                        }
                    } 
                    else if (key == 127 || key == 8) { // BACKSPACE
                        if (nomeIndex > 0) {
                            nomeIndex--;
                            nomeJogador[nomeIndex] = '\0'; // Apaga o último caractere
                        }
                    } 
                    else if (key >= 32 && key <= 126 && nomeIndex < 48) { // Letra/Número/Símbolo
                        nomeJogador[nomeIndex] = (char)key;
                        nomeIndex++;
                        nomeJogador[nomeIndex] = '\0'; // Mantém a string terminada
                    }
                } 
                else if (estadoMenu == 1) { 
                    // --- Estado 1: Escolhendo Dificuldade ---
                    
                    if (key == 13 || key == 10) { // ENTER - Seleção Final!
                        // Converter índice para o Enum
                        if (dificuldadeIdx == 0) nivel = FACIL;
                        else if (dificuldadeIdx == 1) nivel = MEDIO;
                        else nivel = DIFICIL;

                        // Criar o Jogador
                        if (jogador != NULL) { 
                            destruirJogador(jogador); 
                        }
                        jogador = criarJogador(nomeJogador, nivel); 

                        if (jogador == NULL) {
                            telaDrawText(10, 22, "Erro ao criar jogador! Saindo...");
                            telaRefresh();
                            telaGetKey();
                            rodando = false;
                        } else {
                            // Passa para a custscene da Fase 1
                            estadoAtual = CUTSCENE_INICIAL; 
                            
                            // Reseta o menu para a próxima vez
                            estadoMenu = 0; 
                            nomeIndex = 0;
                            memset(nomeJogador, 0, 50);
                        }
                    }
                    else if (key == 'B' || key == 66) { // Seta para Baixo
                        dificuldadeIdx++;
                        if (dificuldadeIdx > 2) dificuldadeIdx = 0; 
                    } 
                    else if (key == 'A' || key == 65) { // Seta para Cima
                        dificuldadeIdx--;
                        if (dificuldadeIdx < 0) dificuldadeIdx = 2;
                    }
                    else if (key == 127 || key == 8) { // BACKSPACE
                         estadoMenu = 0; // Permite voltar e corrigir o nome
                    }
                }
                break; // Fim do case MENU

            // --- LÓGICA DA CUTSCENE INICIAL ---
        case CUTSCENE_INICIAL:
            // Desenha a tela da história
            telaDesenharCutsceneInicial();
            telaRefresh();

            // Espera o jogador pressionar ENTER
            int keyCutscene = telaGetKey();
            if (keyCutscene == 13 || keyCutscene == 10) {
                estadoAtual = CUTSCENE_FORCA; 
            
            }
            break;    

            // --- LÓGICA DA CUTSCENE DA FORCA ---
            case CUTSCENE_FORCA:
                // Desenha a tela
                telaDesenharCutsceneForca();
                telaRefresh();

                // Espera o jogador pressionar ENTER
                int keyForca = telaGetKey();
                if (keyForca == 13 || keyForca == 10) {
                    estadoAtual = FASE_1; // Vai para a Fase 1
                }
                break;


            // --- LÓGICA DA FASE 1 ---
            case FASE_1:
                if (executarFaseForca(jogador)) {
                    estadoAtual = FASE_2; // Passa para Fase 2 se vencer
                } else {
                    venceu = false;
                    estadoAtual = GAME_OVER; // Se perder
                }
                break;

            // --- LÓGICA DA FASE 2 ---
            case FASE_2:
                if (executarFaseBomba(jogador)) {
                    estadoAtual = FASE_3; // Passa para Fase 3 se vencer
                } else {
                    venceu = false;
                    estadoAtual = GAME_OVER; // Se perder
                }
                break;

            // --- LÓGICA DA FASE 3 ---
            case FASE_3:
                if (executarFaseGalaga(jogador)) {
                    venceu = true; // Vencendo o jogo!
                    estadoAtual = GAME_OVER; // Vai para a tela de vitória
                } else {
                    venceu = false;
                    estadoAtual = GAME_OVER; // Se perder
                }
                break;

            // --- LÓGICA DE GAME OVER ---
            case GAME_OVER:
                telaDesenharGameOver(venceu, jogador->pontuacao);
                
                // Pergunta se quer jogar de novo
                telaSetColor(WHITE, BLACK);
                telaDrawText(30, 20, "Jogar novamente? (S/N)");
                telaRefresh();
                int keyGameover = telaGetKey(); 
                
                if (keyGameover == 's' || keyGameover == 'S') {
                    estadoAtual = MENU; // Volta ao menu
                } else {
                    estadoAtual = SAIR; // Sai
                }
                break;

            // --- SAIR ---
            case SAIR:
                rodando = false;
                break;
        }
    }
    // --- FIM DO LOOP PRINCIPAL ---

    // Limpa a memória do jogador antes de sair
    if (jogador != NULL) {
        destruirJogador(jogador);
    }

    telaFinish(); // Finaliza a cli-lib
    return 0;
}