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
    RANK,
    CUTSCENE_INICIAL,
    CUTSCENE_FORCA,
    FASE_1,
    CUTSCENE_BOMBA,
    FASE_2,
    CUTSCENE_GALAGA,
    INTRO_LOGITECH,
    FASE_3,
    CUTSCENE_FINAL,
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
    int dificuldadeIdx = 0; 
    NivelDificuldade nivel;
    
    int estadoMenu = 0; 

    // --- Lista para carregar o ranking ---
    RegistroRank top5[5]; 

    // --- LOOP PRINCIPAL DO JOGO ---
    while (rodando) {
        
        telaClear(); // Limpa a tela no início de cada frame

        switch (estadoAtual) {
            
            // --- LÓGICA DO MENU INICIAL ---
            case MENU:
                
                telaDesenharMenuInicial(nomeJogador, dificuldadeIdx);
                
                if (estadoMenu == 0) { 
                    telaDrawText(20 + 17 + nomeIndex, 10, "_"); 
                }

                telaRefresh(); 
                int key = telaGetKey(); 

                if (estadoMenu == 0) { 
                    if (key == 13 || key == 10) { 
                        if (nomeIndex > 0) { 
                            estadoMenu = 1; 
                        }
                    } 
                    else if (key == 127 || key == 8) { 
                        if (nomeIndex > 0) {
                            nomeIndex--;
                            nomeJogador[nomeIndex] = '\0'; 
                        }
                    } 
                    else if (key >= 32 && key <= 126 && nomeIndex < 48) { 
                        nomeJogador[nomeIndex] = (char)key;
                        nomeIndex++;
                        nomeJogador[nomeIndex] = '\0'; 
                    }
                } 
                else if (estadoMenu == 1) { 
                    if (key == 13 || key == 10) { 
                        if (dificuldadeIdx == 3) {
                             estadoAtual = RANK;
                        } 
                        else {
                            if (dificuldadeIdx == 0) nivel = FACIL;
                            else if (dificuldadeIdx == 1) nivel = MEDIO;
                            else nivel = DIFICIL;

                            if (jogador != NULL) { destruirJogador(jogador); }
                            jogador = criarJogador(nomeJogador, nivel); 

                            if (jogador == NULL) {
                                telaDrawText(10, 22, "Erro ao criar jogador! Saindo...");
                                telaRefresh();
                                telaGetKey();
                                rodando = false;
                            } else {
                                estadoAtual = CUTSCENE_INICIAL; 
                                estadoMenu = 0; 
                                nomeIndex = 0;
                                memset(nomeJogador, 0, 50);
                            }
                        }
                    }
                    else if (key == 'B' || key == 66 || key == 's' || key == 'S') { 
                        dificuldadeIdx++;
                        if (dificuldadeIdx > 3) dificuldadeIdx = 0; 
                    } 
                    else if (key == 'A' || key == 65 || key == 'w' || key == 'W') { 
                        dificuldadeIdx--;
                        if (dificuldadeIdx < 0) dificuldadeIdx = 3; 
                    }
                    else if (key == 127 || key == 8) { 
                         estadoMenu = 0; 
                    }
                }
                break; 

            // --- TELA DE RANK ---
            case RANK:
                carregarRanking(top5); 
                telaDesenharRank(top5); 
                telaRefresh();
                
                int kRank = telaGetKey();
                if (kRank == 13 || kRank == 10) {
                    estadoAtual = MENU;
                }
                else if (kRank == '0') {
                    estadoAtual = MENU;
                }
                else if (kRank == '1') {
                    estadoAtual = SAIR;
                }
                break;

            case CUTSCENE_INICIAL:
                telaDesenharCutsceneInicial();
                telaRefresh();
                int keyCutscene = telaGetKey();
                if (keyCutscene == 13 || keyCutscene == 10) {
                    estadoAtual = CUTSCENE_FORCA; 
                }
                break;    

            case CUTSCENE_FORCA:
                telaDesenharCutsceneForca();
                telaRefresh();
                int keyForca = telaGetKey();
                if (keyForca == 13 || keyForca == 10) {
                    estadoAtual = FASE_1; 
                }
                break;

            case FASE_1:
                if (executarFaseForca(jogador)) {
                    estadoAtual = CUTSCENE_BOMBA; 
                } else {
                    venceu = false;
                    estadoAtual = GAME_OVER; 
                }
                break;

            case CUTSCENE_BOMBA:
                telaDesenharCutsceneBomba();
                telaRefresh();
                int keyBomba = telaGetKey();
                if (keyBomba == 13 || keyBomba == 10) {
                    estadoAtual = FASE_2; 
                }
                break;

            case FASE_2:
                if (executarFaseBomba(jogador)) {
                    estadoAtual = CUTSCENE_GALAGA; 
                } else {
                    venceu = false;
                    estadoAtual = GAME_OVER; 
                }
                break;

            case CUTSCENE_GALAGA:
                telaDesenharCutsceneGalaga();
                telaRefresh();
                int keyGalaga = telaGetKey();
                if (keyGalaga == 13 || keyGalaga == 10) {
                    telaClear();
                    estadoAtual = INTRO_LOGITECH; 
                }
                break;

            // --- INTRODUÇÃO LOGITECH ---
            case INTRO_LOGITECH:
                telaDesenharIntroLogitech();
                telaRefresh();
                int keyIntro = telaGetKey();
                if (keyIntro == 13 || keyIntro == 10) {
                    telaClear();
                    estadoAtual = FASE_3; 
                }
                break;

            case FASE_3:
                if (executarFaseGalaga(jogador)) {
                    venceu = true; 
                    estadoAtual = CUTSCENE_FINAL; 
                } else {
                    venceu = false;
                    estadoAtual = GAME_OVER; 
                }
                break;

            // *** CUTSCENE FINAL ***
            case CUTSCENE_FINAL:
                // Salva a pontuação antes de mostrar a tela final
                if (jogador != NULL) {
                    salvarPontuacao(jogador->nome, jogador->pontuacao);
                    // Evita salvar de novo se o loop rodar
                }

                telaDesenharCutsceneFinal();
                telaRefresh();
                
                int kFinal = telaGetKey();
                if (kFinal == 13 || kFinal == 10) {
                    estadoAtual = RANK;
                }
                break;

            case GAME_OVER:
                if (jogador != NULL) {
                    salvarPontuacao(jogador->nome, jogador->pontuacao);
                }

                telaDesenharGameOver(venceu, jogador->pontuacao);
                
                telaSetColor(WHITE, BLACK);
                telaDrawText(30, 20, "Jogar novamente? (S/N)");
                telaRefresh();
                
                // Loop Infinito de Validação: só sai se apertar S ou N
                while(true) {
                    int keyGameover = telaGetKey(); 
                    
                    // Opção SIM (Jogar Novamente)
                    if (keyGameover == 's' || keyGameover == 'S') {
                        if (jogador != NULL) destruirJogador(jogador);
                        jogador = NULL;
                        estadoAtual = MENU; 
                        estadoMenu = 0;
                        nomeIndex = 0;
                        memset(nomeJogador, 0, 50);
                        break; // Sai do loop de validação
                    } 
                    // Opção NÃO (Sair do Jogo)
                    else if (keyGameover == 'n' || keyGameover == 'N') {
                        estadoAtual = SAIR; 
                        break; // Sai do loop de validação
                    }
                }
                break;

            case SAIR:
                rodando = false;
                break;
        }
    }

    if (jogador != NULL) {
        destruirJogador(jogador);
    }

    telaFinish(); 
    return 0;
}