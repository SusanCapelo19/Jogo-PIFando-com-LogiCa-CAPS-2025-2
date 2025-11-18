#include "tela.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "screen.h"
#include "keyboard.h"

// --- ARTE ASCII DA FORCA  ---
const char* GALLOWS_ART[] = {
    "   +-----+", 
    "   |     |",  
    "         |", 
    "         |", 
    "         |",
    "         |", 
    "         |", 
    "=========="  
};

// --- ARTE DO CORPO ---
const char* BODY_PARTS[] = {
    "O",   // Erro 1: Cabeça
    "|",   // Erro 2: Tronco
    "/|",  // Erro 3: Braço Esquerdo
    "/|\\", // Erro 4: Braço Direito
    "/",   // Erro 5: Perna Esquerda
    "/ \\"  // Erro 6: Perna Direita
};

// --- FUNÇÕES BASE ---

void telaInit() {
    screenInit(0); 
    screenSetColor(WHITE, BLACK); 
    keyboardInit();
    screenHideCursor();
}

void telaFinish() {
    keyboardDestroy();
    screenDestroy();
    screenShowCursor();
    screenSetNormal();
}

void telaClear() {
    telaSetColor(BLACK, BLACK); 

    // Buffer de espaços
    char buffer[MAXX]; 
    memset(buffer, ' ', MAXX - MINX); 
    buffer[MAXX - MINX] = '\0';

    // Deixa a última linha do terminal vazia para evitar rolagem
    for (int y = MINY; y < MAXY; y++) { 
        telaDrawText(MINX, y, buffer); 
    }
    
    telaSetColor(WHITE, BLACK);
    screenHomeCursor();
}

void telaRefresh() {
    screenUpdate();
}

void telaDrawText(int x, int y, char* texto) {
    screenGotoxy(x, y);
    printf("%s", texto);
}

int telaGetKey() {
    while (!keyhit()) {
    }
    return readch();
}

// --- FUNÇÕES DE COR ---
void telaSetColor(screenColor fg, screenColor bg) {
    screenSetColor(fg, bg);
}

// --- FUNÇÕES DE DESENHO ---

void telaDesenharJanela(int x1, int y1, int x2, int y2, const char* titulo) {
    int i;
    char buffer[100];
    screenBoxEnable();
    screenGotoxy(x1, y1); printf("%c", BOX_UPLEFT);
    screenGotoxy(x2, y1); printf("%c", BOX_UPRIGHT);
    screenGotoxy(x1, y2); printf("%c", BOX_DWNLEFT);
    screenGotoxy(x2, y2); printf("%c", BOX_DWNRIGHT);
    for (i = x1 + 1; i < x2; i++) {
        screenGotoxy(i, y1); printf("%c", BOX_HLINE);
        screenGotoxy(i, y2); printf("%c", BOX_HLINE);
    }
    for (i = y1 + 1; i < y2; i++) {
        screenGotoxy(x1, i); printf("%c", BOX_VLINE);
        screenGotoxy(x2, i); printf("%c", BOX_VLINE);
    }
    if (titulo != NULL) {
        int tamTitulo = strlen(titulo);
        int xTitulo = x1 + (x2 - x1 - tamTitulo) / 2;
        sprintf(buffer, " %s ", titulo);
        screenGotoxy(xTitulo - 1, y1);
        printf("%s", buffer);
    }
    screenBoxDisable();
}


void telaDesenharMenuInicial(const char* nomeDigitado, int dificuldadeSelecionada) {
        
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL);
    telaSetColor(WHITE, BLACK);
    const char* linhaTitulo = "===================================";
    const char* nomeJogo = " SOBREVIVENDO A PIF COM LOGICA";
    
    int xTitulo = (MAXX - strlen(linhaTitulo)) / 2;
    
    telaDrawText(xTitulo, 4, (char*)linhaTitulo);
    telaDrawText(xTitulo, 5, (char*)nomeJogo);
    telaDrawText(xTitulo, 6, (char*)linhaTitulo);

    int xPrompt = 20;
    int yPrompt = 10;
    telaDrawText(xPrompt, yPrompt, "Digite seu nome: ");
    telaDrawText(xPrompt + 17, yPrompt, (char*)nomeDigitado);

    yPrompt += 3; 
    telaDrawText(xPrompt, yPrompt, "Nivel de dificuldade:");
    
    int xOpcoes = xPrompt + 10;
    
    if (dificuldadeSelecionada == 0) {
        telaSetColor(GREEN, BLACK);
        telaDrawText(xOpcoes, yPrompt + 2, "> FACIL <");
    } else {
        telaSetColor(GREEN, BLACK);
        telaDrawText(xOpcoes, yPrompt + 2, "  FACIL  ");
    }
    if (dificuldadeSelecionada == 1) {
        telaSetColor(YELLOW, BLACK); 
        telaDrawText(xOpcoes, yPrompt + 3, "> MEDIO <");
    } else {
        telaSetColor(YELLOW, BLACK);
        telaDrawText(xOpcoes, yPrompt + 3, "  MEDIO  ");
    }
    if (dificuldadeSelecionada == 2) {
        telaSetColor(RED, BLACK); 
        telaDrawText(xOpcoes, yPrompt + 4, "> DIFICIL <");
    } else {
        telaSetColor(RED, BLACK);
        telaDrawText(xOpcoes, yPrompt + 4, "  DIFICIL  ");
    }

    telaSetColor(WHITE, BLACK);
    const char* instrucao = "Apertar Enter pra confirmar. Use as setas cima/baixo para escolher.";
    int xInstrucao = (MAXX - strlen(instrucao)) / 2;
    
    telaDrawText(xInstrucao, MAXY - 3, (char*)instrucao);
}

// --- CUTSCENES ---

void telaDesenharCutsceneInicial(void) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY, NULL); 
    telaSetColor(WHITE, BLACK);
    
    const char* linha1 = "VOCE É UM ESTUDANTE DE PIF QUE FICOU NA FINAL E FOI FAZER";
    const char* linha2 = "A PROVA, JÁ SABENDO QUE SERIA DIFÍCIL, SÓ NÃO ESPERAVA QUE";
    const char* linha3 = "ELA FOSSE TÃO DIFÍCIL...";
    const char* linha4 = "AO RODAR UM CODIGO QUE O PROFESSOR DEIXOU";
    const char* linha5 = "PRONTO NA SUA MÁQUINA, VOCÊ FOI PUXADO PARA UMA";
    const char* linha6 = "VIAGEM NO TEMPO, ONDE DEVERÁ ENFRENTAR 3 DESAFIOS E";
    const char* linha7 = "VENCÊ-LOS, SE VOCÊ QUISER PASSAR EM PIF E TAMBÉM VIVER...";
    const char* linha8 = "MAS NÃO SE PREOCUPE, VOCE NÃO ESTARÁ SOZINHO, A LÓGICA";
    const char* linha9 = "VAI SER SUA ALIADA NESSA VIAGEM E LHE AJUDAR A TRILHAR";
    const char* linha10 = "SEU CAMINHO!";
    const char* prompt = "Pressione ENTER para continuar.";

    telaDrawText((MAXX - strlen(linha1)) / 2, 4, (char*)linha1);
    telaDrawText((MAXX - strlen(linha2)) / 2, 5, (char*)linha2);
    telaDrawText((MAXX - strlen(linha3)) / 2, 6, (char*)linha3);
    telaDrawText((MAXX - strlen(linha4)) / 2, 8, (char*)linha4);
    telaDrawText((MAXX - strlen(linha5)) / 2, 9, (char*)linha5);
    telaDrawText((MAXX - strlen(linha6)) / 2, 10, (char*)linha6);
    telaDrawText((MAXX - strlen(linha7)) / 2, 11, (char*)linha7);
    telaDrawText((MAXX - strlen(linha8)) / 2, 13, (char*)linha8);
    telaDrawText((MAXX - strlen(linha9)) / 2, 14, (char*)linha9);
    telaDrawText((MAXX - strlen(linha10)) / 2, 15, (char*)linha10);
    telaDrawText((MAXX - strlen(prompt)) / 2, MAXY - 2, (char*)prompt);

    telaSetColor(WHITE, BLACK);     telaDrawText(2, 3, "x"); 
    telaSetColor(LIGHTBLUE, BLACK); telaDrawText(5, 6, "x"); 
    telaSetColor(YELLOW, BLACK);    telaDrawText(2, 9, "x"); 
    telaSetColor(MAGENTA, BLACK);   telaDrawText(5, 12, "x"); 
    telaSetColor(WHITE, BLACK);     telaDrawText(2, 15, "x");
    telaSetColor(LIGHTBLUE, BLACK); telaDrawText(5, 18, "x");
    telaSetColor(WHITE, BLACK);     telaDrawText(MAXX - 3, 3, "x"); 
    telaSetColor(LIGHTBLUE, BLACK); telaDrawText(MAXX - 6, 6, "x"); 
    telaSetColor(YELLOW, BLACK);    telaDrawText(MAXX - 3, 9, "x"); 
    telaSetColor(MAGENTA, BLACK);   telaDrawText(MAXX - 6, 12, "x"); 
    telaSetColor(WHITE, BLACK);     telaDrawText(MAXX - 3, 15, "x");
    telaSetColor(LIGHTBLUE, BLACK); telaDrawText(MAXX - 6, 18, "x");
    telaSetColor(WHITE, BLACK);
}

void telaDesenharCutsceneForca(void) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY, NULL); 
    telaSetColor(WHITE, BLACK);
    
    const char* linha1 = "INGLATERRA, SÉCULO XVIII, O REI JORGE II LHE SENTENCIOU À FORCA, VOCÊ DEVE";
    const char* linha2 = "DESVENDAR O ENIGMA PARA PASSAR DE FASE E NÃO SER ENFORCADO. THOMAS BAYES";
    const char* linha3 = "VAI LHE AJUDAR NESSA JORNADA, VOCÊ PODE PEDIR AJUDA A ELE 3X, MAS NÃO ACHE";
    const char* linha4 = "QUE SERÁ DE GRAÇA: VOCÊ DEVERÁ DAR UMA RESPOSTA CERTA PARA CADA AJUDA...";
    const char* linha5 = "BOA SORTE!";

    telaDrawText(3, 3, (char*)linha1);
    telaDrawText(3, 4, (char*)linha2);
    telaDrawText(3, 5, (char*)linha3);
    telaDrawText(3, 6, (char*)linha4);
    telaDrawText(3, 8, (char*)linha5); 

    telaSetColor(RED, BLACK); // Cor da Forca
    int xForca = 35;
    int yForca = 10; 

    for (int i = 0; i < 8; i++) { 
        telaDrawText(xForca, yForca + i, (char*)GALLOWS_ART[i]);
    }

    telaSetColor(WHITE, BLACK);
    telaDrawText(xForca + 2, yForca + 8, "-------"); 

    const char* prompt = "Pressione ENTER para continuar.";
    telaSetColor(YELLOW, BLACK); 
    telaDrawText((MAXX - strlen(prompt)) / 2, MAXY - 2, (char*)prompt);
    telaSetColor(WHITE, BLACK);
}

// --- FUNÇÃO AUXILIAR ---
void desenharPlacar(const char* nome, int pontuacao) {
    telaSetColor(LIGHTRED, BLACK); // Cor de destaque para o placar
    // Posiciona no canto superior direito
    int xPos = MAXX - 12; 
    // Desenha Nome
    telaDrawText(xPos, 2, (char*)nome);
    // Desenha Pontuação 
    char buffer[20];
    sprintf(buffer, "%04d", pontuacao);
    telaDrawText(xPos, 3, buffer);
    
    telaSetColor(WHITE, BLACK); 
}
// --- TELA DA FORCA ---
void telaDesenharFaseForca(int erros, const char* letrasDescobertas, const char* letrasTentadas, const char* nome, int pontuacao) {
    
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY, NULL); 
    telaSetColor(WHITE, BLACK);
    
    // Desenha o placar 
    desenharPlacar(nome, pontuacao);

    // Desenha o Sol
    telaSetColor(YELLOW, BLACK);
    telaDrawText(5, 3, " \\ | /");
    telaDrawText(5, 4, "-- O --");
    telaDrawText(5, 5, " / | \\");
    telaSetColor(WHITE, BLACK);

    // Desenha as Nuvens
    telaDrawText(20, 2, " oooo"); telaDrawText(20, 3, "OOOOOO");
    telaDrawText(40, 2, " ooooo"); telaDrawText(40, 3, "OOOOOO");
    telaDrawText(55, 2, " oooo"); telaDrawText(55, 3, "OOOOOO"); 

    // Forca
    telaSetColor(RED, BLACK); 
    int xForca = 35; 
    int yForca = 6;  
    for (int i = 0; i < 8; i++) {
        telaDrawText(xForca, yForca + i, (char*)GALLOWS_ART[i]);
    }

    // Boneco
    telaSetColor(WHITE, BLACK); 
    int xBoneco = xForca + 3; 
    int yBoneco = yForca + 2; 
    if (erros > 0) telaDrawText(xBoneco, yBoneco, "O");      
    if (erros > 1) telaDrawText(xBoneco, yBoneco + 1, "|");  
    if (erros > 2) telaDrawText(xBoneco - 1, yBoneco + 1, "/"); 
    if (erros > 3) telaDrawText(xBoneco + 1, yBoneco + 1, "\\"); 
    if (erros > 4) telaDrawText(xBoneco - 1, yBoneco + 2, "/"); 
    if (erros > 5) telaDrawText(xBoneco + 1, yBoneco + 2, "\\"); 

    // Layout de Texto
    telaSetColor(WHITE, BLACK);
    telaDrawText(30, 15, "Digite uma letra:"); 

    int xPalavra = (MAXX - 1 - strlen(letrasDescobertas)) / 2; 
    telaDrawText(xPalavra, 17, (char*)letrasDescobertas);

    char buffer[100];
    telaDrawText(5, 20, "Erros:");
    sprintf(buffer, "%d/6", erros);
    telaDrawText(5, 21, buffer);

    telaDrawText(MAXX - 20, 20, "Letras usadas:");
    telaDrawText(MAXX - 20, 21, (char*)letrasTentadas); 

    telaDrawText((MAXX - 1 - strlen("Para pedir ajuda ao Bayes digite 0")) / 2, MAXY - 2, "Para pedir ajuda ao Bayes digite 0");
}

// --- TELA DE VITÓRIA DA FASE 1 ---
void telaDesenharVitoriaFase1(int pontuacao) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY, NULL); 
    telaSetColor(WHITE, BLACK);

    const char* msg1 = "VOCE SOBREVIVEU... POR ENQUANTO!";
    const char* msg2 = "PREPARE-SE PARA O PROXIMO DESAFIO.";
    
    telaDrawText((MAXX - strlen(msg1)) / 2, 9, (char*)msg1);
    telaDrawText((MAXX - strlen(msg2)) / 2, 11, (char*)msg2);

    // Mostra pontuação acumulada
    char buffer[50];
    sprintf(buffer, "Pontuacao Atual: %04d", pontuacao);
    telaSetColor(CYAN, BLACK);
    telaDrawText((MAXX - strlen(buffer)) / 2, 14, buffer);
    
    telaSetColor(YELLOW, BLACK);
    const char* prompt = "Pressione ENTER para continuar.";
    telaDrawText((MAXX - strlen(prompt)) / 2, MAXY - 4, (char*)prompt);
    telaSetColor(WHITE, BLACK);
}

// --- FASES AINDA NÃO IMPLEMENTADAS ---
void telaDesenharFaseBomba(const char* nome, int pontuacao) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY, NULL); 
    desenharPlacar(nome, pontuacao);
    telaSetColor(WHITE, BLACK);
    telaDrawText(10, 5, "FASE 2 (Ainda nao implementado)");
}

void telaDesenharFaseGalaga(const char* nome, int pontuacao) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY, NULL); 
    desenharPlacar(nome, pontuacao);
    telaSetColor(WHITE, BLACK);
    telaDrawText(10, 5, "FASE 3 (Ainda nao implementado)");
}

// --- TELAS DE GAME OVER E VITÓRIA ---

void telaDesenharGameOver(bool venceu, int pontuacaoFinal) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY, NULL); 
    telaSetColor(WHITE, BLACK);
    
    if (venceu) {
        telaDrawText(31, 5, "VOCÊ PASSOU EM PIF"); 
        telaDrawText(34, 6, "e é um herói!");
    } else {
        const char* msg1 = "VOCÊ NÃO PASSOU EM PIF";
        const char* msg2 = "e morreu";
        telaSetColor(RED, BLACK); 
        telaDrawText((MAXX - strlen(msg1)) / 2, 10, (char*)msg1);
        telaSetColor(WHITE, BLACK); 
        telaDrawText((MAXX - strlen(msg2)) / 2, 12, (char*)msg2);
    }

    // Mostra pontuação final
    char buffer[50];
    sprintf(buffer, "Pontuacao Final: %04d", pontuacaoFinal);
    telaSetColor(CYAN, BLACK);
    telaDrawText((MAXX - strlen(buffer)) / 2, 15, buffer);
    telaSetColor(WHITE, BLACK);
}