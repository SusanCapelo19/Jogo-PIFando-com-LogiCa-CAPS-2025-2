#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "screen.h"
#include "keyboard.h"
#include "jogador.h"
#include "logica.h"
#include "fase3_galaga.h"
#include "tela.h"

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

// --- ARTE DO BOSS GALAGA ---
const char* BOSS_ART[] = {
    "      /---\\  /---\\      ",
    "     |  O  | |  O  |     ",
    "   __|-----|_|-----|__   ",
    "  /  |   | |_|   | |  \\  ",
    " |   \\__|_______|__/   | ",
    " |       |   V   |      | ",
    " |       |_______|      | ",
    "  \\     /       \\     /  ",
    "   \\   /         \\   /   ",
    "    \\_/           \\_/    "
};
const int BOSS_HEIGHT = 10; 
const int BOSS_WIDTH = 29;  

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

    char buffer[MAXX]; 
    memset(buffer, ' ', MAXX - MINX); 
    buffer[MAXX - MINX] = '\0';

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
    const char* nomeJogo = "   SOBREVIVENDO A PIF COM LOGICA";
    int xTitulo = (MAXX - 1 - strlen(linhaTitulo)) / 2 + MINX;
    telaDrawText(xTitulo, 4, (char*)linhaTitulo);
    telaDrawText(xTitulo, 5, (char*)nomeJogo);
    telaDrawText(xTitulo, 6, (char*)linhaTitulo);

    int xPrompt = 20; int yPrompt = 10;
    telaDrawText(xPrompt, yPrompt, "Digite seu nome: ");
    telaDrawText(xPrompt + 17, yPrompt, (char*)nomeDigitado);

    yPrompt += 3; 
    telaDrawText(xPrompt, yPrompt, "Nivel de dificuldade:");
    int xOpcoes = xPrompt + 10;
    
    if (dificuldadeSelecionada == 0) { telaSetColor(GREEN, BLACK); telaDrawText(xOpcoes, yPrompt + 2, "> FACIL <"); } 
    else { telaSetColor(GREEN, BLACK); telaDrawText(xOpcoes, yPrompt + 2, "  FACIL  "); }
    
    if (dificuldadeSelecionada == 1) { telaSetColor(YELLOW, BLACK); telaDrawText(xOpcoes, yPrompt + 3, "> MEDIO <"); } 
    else { telaSetColor(YELLOW, BLACK); telaDrawText(xOpcoes, yPrompt + 3, "  MEDIO  "); }
    
    if (dificuldadeSelecionada == 2) { telaSetColor(LIGHTRED, BLACK); telaDrawText(xOpcoes, yPrompt + 4, "> DIFICIL <"); } 
    else { telaSetColor(LIGHTRED, BLACK); telaDrawText(xOpcoes, yPrompt + 4, "  DIFICIL  "); }

    if (dificuldadeSelecionada == 3) { 
        telaSetColor(DARKGRAY, BLACK); 
        telaDrawText(xOpcoes, yPrompt + 5, "> RANK <"); 
    } else { 
        telaSetColor(DARKGRAY, BLACK); 
        telaDrawText(xOpcoes, yPrompt + 5, "  RANK  "); 
    }

    telaSetColor(YELLOW, BLACK); 
    const char* instrucao = "W/S: Escolher | ENTER: Confirmar";
    int xInstrucao = (MAXX - 1 - strlen(instrucao)) / 2 + MINX;
    telaDrawText(xInstrucao, MAXY - 2, (char*)instrucao);
    telaSetColor(WHITE, BLACK);
}

void telaDesenharRank(RegistroRank ranking[5]) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
    
    telaSetColor(WHITE, BLACK);
    const char* titulo = "RANKING DE SOBREVIVENTES";
    telaDrawText((MAXX - strlen(titulo))/2, 4, (char*)titulo);
    
    int xBase = 20; 
    int yBase = 8;
    telaSetColor(LIGHTGRAY, BLACK);
    telaDrawText(xBase, yBase, "POS.   NOME                PONTUAÇÃO");
    telaDrawText(xBase, yBase+1, "-----------------------------------");

    char bufferNome[50];
    char bufferPontos[20];
    char bufferPos[10];

    for (int i = 0; i < 5; i++) {
        int yLinha = yBase + 3 + i;
        
        sprintf(bufferPos, "%d.", i + 1);
        sprintf(bufferNome, "%-20s", ranking[i].nome); 
        sprintf(bufferPontos, "%04d", ranking[i].pontuacao);

        telaSetColor(WHITE, BLACK); 
        telaDrawText(xBase, yLinha, bufferPos);

        telaSetColor(WHITE, BLACK); 
        telaDrawText(xBase + 6, yLinha, bufferNome);

        telaSetColor(CYAN, BLACK);  
        telaDrawText(xBase + 26, yLinha, bufferPontos);
    }

    telaSetColor(YELLOW, BLACK);
    const char* rodape = "MENU: 0 | SAIR: 1";
    telaDrawText((MAXX - strlen(rodape))/2, MAXY - 2, (char*)rodape);
    telaSetColor(WHITE, BLACK);
}

void telaDesenharTextoQuebrado(int x, int y, const char* texto, int larguraMaxima) {
    char buffer[256];
    int len = strlen(texto);
    int inicio = 0;
    int linha = 0;

    while (inicio < len) {
        int restante = len - inicio;
        
        if (restante <= larguraMaxima) {
            telaDrawText(x, y + linha, (char*)(texto + inicio));
            break;
        }

        int pontoCorte = inicio + larguraMaxima;
        int temp = pontoCorte;
        int achouEspaco = 0;
        
        while (temp > inicio) {
            if (texto[temp] == ' ') {
                pontoCorte = temp;
                achouEspaco = 1;
                break;
            }
            temp--;
        }

        if (!achouEspaco) {
            pontoCorte = inicio + larguraMaxima;
        }

        int tamLinha = pontoCorte - inicio;
        strncpy(buffer, texto + inicio, tamLinha);
        buffer[tamLinha] = '\0'; 

        telaDrawText(x, y + linha, buffer);

        linha++;
        inicio = pontoCorte;
        
        if (texto[inicio] == ' ') {
            inicio++;
        }
    }
}
// --- CUTSCENES ---

void telaDesenharCutsceneInicial(void) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
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
    
    const char* prompt = "Pressione ENTER para continuar.";
    telaSetColor(YELLOW, BLACK); 
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
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
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

    telaSetColor(RED, BLACK); 
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
    telaSetColor(LIGHTRED, BLACK); 
    
    int xPos = MAXX - 8; 
    
    telaDrawText(xPos, 2, (char*)nome);
    
    char buffer[20];
    sprintf(buffer, "%04d", pontuacao); 
    telaDrawText(xPos, 3, buffer);
    
    telaSetColor(WHITE, BLACK); 
}

// --- TELA DA FORCA ---
void telaDesenharFaseForca(int erros, const char* letrasDescobertas, const char* letrasTentadas, const char* nome, int pontuacao) {
    
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
    telaSetColor(WHITE, BLACK);
    
    desenharPlacar(nome, pontuacao);

    telaSetColor(YELLOW, BLACK);
    telaDrawText(5, 3, " \\ | /");
    telaDrawText(5, 4, "-- O --");
    telaDrawText(5, 5, " / | \\");
    telaSetColor(WHITE, BLACK);

    telaDrawText(20, 2, " oooo"); telaDrawText(20, 3, "OOOOOO");
    telaDrawText(40, 2, " oooo"); telaDrawText(40, 3, "OOOOOO");
    telaDrawText(55, 2, " oooo"); telaDrawText(55, 3, "OOOOOO"); 

    telaSetColor(RED, BLACK); 
    int xForca = 35; 
    int yForca = 6;  
    for (int i = 0; i < 8; i++) {
        telaDrawText(xForca, yForca + i, (char*)GALLOWS_ART[i]);
    }

    telaSetColor(WHITE, BLACK); 
    int xBoneco = xForca + 3; 
    int yBoneco = yForca + 2; 
    if (erros > 0) telaDrawText(xBoneco, yBoneco, "O");      
    if (erros > 1) telaDrawText(xBoneco, yBoneco + 1, "|");  
    if (erros > 2) telaDrawText(xBoneco - 1, yBoneco + 1, "/"); 
    if (erros > 3) telaDrawText(xBoneco + 1, yBoneco + 1, "\\"); 
    if (erros > 4) telaDrawText(xBoneco - 1, yBoneco + 2, "/"); 
    if (erros > 5) telaDrawText(xBoneco + 1, yBoneco + 2, "\\"); 

    telaSetColor(WHITE, BLACK);
    telaDrawText(30, 15, "Digite uma letra:"); 


    int xPalavra = (MAXX - 1 - strlen(letrasDescobertas)) / 2; 
    telaDrawText(xPalavra, 17, (char*)letrasDescobertas);

    char buffer[100];
    telaSetColor(BLUE, BLACK);
    telaDrawText(5, 20, "Erros:");
    sprintf(buffer, "%d/6", erros);
    telaDrawText(5, 21, buffer);

    telaDrawText(MAXX - 20, 20, "Letras usadas:");
    telaDrawText(MAXX - 20, 21, (char*)letrasTentadas); 

    telaSetColor(YELLOW,BLACK);
    telaDrawText((MAXX - 1 - strlen("Para pedir ajuda ao Bayes digite 0")) / 2, MAXY - 2, "Para pedir ajuda ao Bayes digite 0");
}

// --- TELA DE VITÓRIA DA FASE 1 ---
void telaDesenharVitoriaFase1(int pontuacao) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
    telaSetColor(WHITE, BLACK);

    const char* msg1 = "VOCÊ SOBREVIVEU... POR ENQUANTO!";
    const char* msg2 = "PREPARE-SE PARA O PRÓXIMO DESAFIO.";
    
    telaDrawText((MAXX - strlen(msg1)) / 2, 9, (char*)msg1);
    telaDrawText((MAXX - strlen(msg2)) / 2, 11, (char*)msg2);

    char buffer[50];
    sprintf(buffer, "Pontuação Atual: %04d", pontuacao);
    telaSetColor(CYAN, BLACK);
    telaDrawText((MAXX - strlen(buffer)) / 2, 14, buffer);
    
    telaSetColor(YELLOW, BLACK);
    const char* prompt = "Pressione ENTER para continuar.";
    telaDrawText((MAXX - strlen(prompt)) / 2, MAXY - 4, (char*)prompt);
    telaSetColor(WHITE, BLACK);
}

void telaDesenharCutsceneBomba(void) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
    telaSetColor(WHITE, BLACK);

    const char* linha1 = "PARABÉNS! VOCÊ SOBREVIVEU À FORCA.";
    const char* linha2 = "MAS AINDA NÃO É O MOMENTO PARA COMEMORAR...";
    const char* linha3 = "A VITÓRIA REATIVOU A VIAGEM NO TEMPO E AGORA TE TROUXE DE";
    const char* linha4 = "VOLTA PARA O PRESENTE, PARA DESARMAR UMA BOMBA LÓGICA, QUE FOI";
    const char* linha5 = "COLOCADA EM UM CAMINHÃO QUE ESTÁ BLOQUEANDO O RODOANEL";
    const char* linha6 = "MÁRIO COVAS. VOCÊ TEM POUCOS MINUTOS PARA DESARMÁ-LA!";
    const char* linha7 = "MAS VOCÊ PODE CONTAR COM AS DICAS DO";
    const char* linha8 = "PROFESSOR GUILHERME PARA LHE AJUDAR.";
        
    const char* prompt = "Pressione ENTER para aceitar a missão.";

    int y = 2;
    telaDrawText((MAXX - strlen(linha1)) / 2, y++, (char*)linha1);
    telaDrawText((MAXX - strlen(linha2)) / 2, y++, (char*)linha2);
    telaDrawText((MAXX - strlen(linha3)) / 2, y++, (char*)linha3);
    telaDrawText((MAXX - strlen(linha4)) / 2, y++, (char*)linha4);
    telaDrawText((MAXX - strlen(linha5)) / 2, y++, (char*)linha5);
    telaDrawText((MAXX - strlen(linha6)) / 2, y++, (char*)linha6);
    telaDrawText((MAXX - strlen(linha7)) / 2, y++, (char*)linha7);
    telaDrawText((MAXX - strlen(linha8)) / 2, y++, (char*)linha8);

    telaSetColor(WHITE, BLACK);
        
    int xTruck = 22; 
    int yTruck = 11; 

    telaSetColor(RED, BLACK); 
    telaDrawText(xTruck, yTruck++, "      ____________________");
    telaDrawText(xTruck, yTruck++, "     /                    |");
    
    telaDrawText(xTruck, yTruck,   "    |   ");
    telaSetColor(RED, BLACK); 
    telaDrawText(xTruck + 8, yTruck, "CESAR  SCHOOL"); 
    telaSetColor(BLUE, BLACK);
    telaDrawText(xTruck + 22, yTruck++, "   |");
    
    telaDrawText(xTruck, yTruck++, "    |____________________ |__");
    telaDrawText(xTruck, yTruck++, "    |  ___   ___   ___  |  __|");
    telaDrawText(xTruck, yTruck++, "    | |   | |   | |   | | |  |"); 
    telaDrawText(xTruck, yTruck++, "    |_|___|_|___|_|___|_|_|__|");
    
    telaSetColor(DARKGRAY, BLACK);
    telaDrawText(xTruck, yTruck++, "     (O) (O) (O)       (O) (O)");

    telaSetColor(YELLOW, BLACK);
    telaDrawText((MAXX - strlen(prompt)) / 2, MAXY - 2, (char*)prompt);
    telaSetColor(WHITE, BLACK);
}

void telaDesenharFaseBomba(bool alvo[9], Proposicao* matriz[9], bool atomos[3], int selecionado, int tempoRestante, bool ajudaAtiva, const char* textoAjuda, const char* nome, int pontuacao) {
    
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 

    desenharPlacar(nome, pontuacao);
    
    char buffer[50];
    int minutos = tempoRestante / 60;
    int segundos = tempoRestante % 60;
    
    if (tempoRestante < 60) telaSetColor(RED, BLACK); 
    else telaSetColor(CYAN, BLACK);
    
    sprintf(buffer, "TEMPO: %02d:%02d", minutos, segundos);
    telaDrawText(3, 2, buffer);
    
    telaSetColor(WHITE, BLACK);
    const char* tituloAlvo = "PADRAO ALVO:";
    telaDrawText((MAXX - strlen(tituloAlvo))/2, 3, (char*)tituloAlvo);
    
    int xAlvoBase = (MAXX - (3 * 4)) / 2; 
    int yAlvoBase = 4;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int idx = i * 3 + j;
            if (alvo[idx]) {
                telaSetColor(GREEN, BLACK);
                telaDrawText(xAlvoBase + (j * 4), yAlvoBase + i, "[V]");
            } else {
                telaSetColor(RED, BLACK);
                telaDrawText(xAlvoBase + (j * 4), yAlvoBase + i, "[F]");
            }
        }
    }

    int larguraCelula = 22;
    int xInicioGrade = 6;
    int yInicioGrade = 8; 
    
    telaSetColor(WHITE, BLACK);
    telaDrawText(xInicioGrade, yInicioGrade, " ______________________ ______________________ ______________________ ");

    for (int i = 0; i < 3; i++) { 
        int yLinhaAtual = yInicioGrade + 1 + (i * 2); 
        
        for (int j = 0; j < 3; j++) { 
            int idx = i * 3 + j;
            int xColunaAtual = xInicioGrade + (j * (larguraCelula + 1)); 

            telaSetColor(WHITE, BLACK);
            telaDrawText(xColunaAtual, yLinhaAtual, "|");
            
            if (j == 2) telaDrawText(xColunaAtual + larguraCelula + 1, yLinhaAtual, "|");

            bool valorAtual = avaliar(matriz[idx]);
            
            if (valorAtual) telaSetColor(GREEN, BLACK); 
            else telaSetColor(RED, BLACK);             

            char textoExpr[100] = "";
            imprimirExpressao(matriz[idx], textoExpr);
            
            int lenExpr = strlen(textoExpr);
            int padding = (larguraCelula - lenExpr) / 2;
            if (padding < 0) padding = 0; 

            telaDrawText(xColunaAtual + 1 + padding, yLinhaAtual, textoExpr);
        }

        telaSetColor(WHITE, BLACK);
        char linhaFundo[100] = "";
        sprintf(linhaFundo, "|______________________|______________________|______________________|");
        telaDrawText(xInicioGrade, yLinhaAtual + 1, linhaFundo);
    }

    telaSetColor(WHITE, BLACK);
    int xVars = 20;
    int yVars = 18;
    const char* nomes[3] = {"P", "Q", "R"};
    
    for(int i=0; i<3; i++) {
        int xPos = xVars + (i * 15);
        
        if (i == selecionado) {
            telaSetColor(YELLOW, BLACK);
            telaDrawText(xPos, yVars - 1, "  v  ");
        } else {
            telaDrawText(xPos, yVars - 1, "     ");
        }
        
        telaSetColor(WHITE, BLACK);
        sprintf(buffer, "%s = ", nomes[i]);
        telaDrawText(xPos, yVars, buffer);
        
        if (atomos[i]) {
            telaSetColor(GREEN, BLACK);
            telaDrawText(xPos + 4, yVars, "V");
        } else {
            telaSetColor(RED, BLACK);
            telaDrawText(xPos + 4, yVars, "F");
        }
    }

    telaSetColor(YELLOW, BLACK);
    const char* instrucoes = "ESPAÇO: Mudar Valor | A/D: Mover | '0': Dica Prof. Guilherme";
    telaDrawText((MAXX - strlen(instrucoes))/2, MAXY - 3, (char*)instrucoes);

    if (ajudaAtiva) {
        telaSetColor(WHITE, BLUE); 
        
        int yAjuda = 12;
        int xCaixa = 10;
        int larguraCaixa = 60; 
        
        char fundo[100];
        memset(fundo, ' ', larguraCaixa);
        fundo[larguraCaixa] = '\0';

        for(int k = 0; k < 7; k++) {
            telaDrawText(xCaixa, yAjuda + k, fundo);
        }

        const char* titulo = "DICA DO PROF. GUILHERME";
        int xTitulo = xCaixa + (larguraCaixa - strlen(titulo)) / 2;
        telaDrawText(xTitulo, yAjuda + 1, (char*)titulo);
        
        char linhaSep[100];
        memset(linhaSep, '=', larguraCaixa - 4);
        linhaSep[larguraCaixa - 4] = '\0';
        telaDrawText(xCaixa + 2, yAjuda + 2, linhaSep);

        telaDesenharTextoQuebrado(xCaixa + 4, yAjuda + 3, textoAjuda, larguraCaixa - 8);
        
        telaSetColor(WHITE, BLACK); 
    }
}

void telaDesenharVitoriaFase2(int pontuacao) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
    telaSetColor(WHITE, BLACK);

    const char* msg1 = "VOCÊ CONSEGUIU DE NOVO...";
    const char* msg2 = "MAS NÃO SABE O QUE LHE ESPERA A SEGUIR!";
    
    telaDrawText((MAXX - strlen(msg1)) / 2, 9, (char*)msg1);
    telaDrawText((MAXX - strlen(msg2)) / 2, 11, (char*)msg2);

    char buffer[50];
    sprintf(buffer, "Pontuação Atual: %04d", pontuacao);
    telaSetColor(CYAN, BLACK);
    telaDrawText((MAXX - strlen(buffer)) / 2, 14, buffer);
    
    telaSetColor(YELLOW, BLACK);
    const char* prompt = "Pressione ENTER para continuar.";
    telaDrawText((MAXX - strlen(prompt)) / 2, MAXY - 4, (char*)prompt);
    telaSetColor(WHITE, BLACK);
}

void telaDesenharCutsceneGalaga(void) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
    telaSetColor(WHITE, BLACK);
    
    const char* l1 = "PARABÉNS, VOCÊ CONSEGUIU PASSAR PARA A PRÓXIMA FASE DA PROVA DE PIF,";
    const char* l2 = "SALVOU CENTENAS DE VIDAS, INCLUSIVE A SUA, NO PROCESSO.";
    const char* l3 = "MAS A VITÓRIA ACIONOU MAIS UMA VEZ A VIAGEM NO TEMPO";
    const char* l4 = "E AGORA VOCÊ FOI LEVADO PARA O FUTURO, ONDE DEVERÁ DEFENDER A TERRA";
    const char* l5 = "DE UMA INVASÃO ALIENIGENA, PARA PASSAR EM PIF E SALVAR O MUNDO.";

    int y = 4; 
    telaDrawText((MAXX - strlen(l1)) / 2, y++, (char*)l1);
    telaDrawText((MAXX - strlen(l2)) / 2, y++, (char*)l2);
    telaDrawText((MAXX - strlen(l3)) / 2, y++, (char*)l3);
    telaDrawText((MAXX - strlen(l4)) / 2, y++, (char*)l4);
    telaDrawText((MAXX - strlen(l5)) / 2, y++, (char*)l5);

    telaSetColor(CYAN, BLACK);     telaDrawText(10, 12, "."); telaDrawText(70, 15, ".");
    telaSetColor(MAGENTA, BLACK);  telaDrawText(20, 18, "."); telaDrawText(60, 13, ".");
    telaSetColor(YELLOW, BLACK);   telaDrawText(30, 14, "."); telaDrawText(50, 19, ".");
    telaSetColor(RED, BLACK);      telaDrawText(15, 20, "."); telaDrawText(75, 17, ".");
    telaSetColor(WHITE, BLACK);    telaDrawText(5, 15, ".");  telaDrawText(35, 11, ".");
    telaSetColor(CYAN, BLACK);     telaDrawText(45, 16, "."); telaDrawText(65, 12, ".");
    
    int x = MAXX / 2; 
    int yn = 18;
    
    telaSetColor(RED, BLACK); 
    telaDrawText(x, yn, "^"); 
    telaDrawText(x - 1, yn + 1, "/");
    
    telaSetColor(WHITE, BLACK); 
    telaDrawText(x, yn + 1, "C"); 
    
    telaSetColor(RED, BLACK); 
    telaDrawText(x + 1, yn + 1, "\\");
    
    telaSetColor(RED, BLACK); 
    telaDrawText(x - 3, yn + 2, "*");
    
    telaSetColor(WHITE, BLACK); 
    telaDrawText(x - 2, yn + 2, "-----");
    
    telaSetColor(RED, BLACK); 
    telaDrawText(x + 3, yn + 2, "*");

    telaSetColor(YELLOW, BLACK);
    const char* prompt = "Pressione ENTER para continuar.";
    telaDrawText((MAXX - strlen(prompt))/2, MAXY - 2, (char*)prompt);
    
    telaSetColor(WHITE, BLACK);
}

// --- APRESENTAÇÃO LOGITECH ---
void telaDesenharIntroLogitech(void) {
    // Limpa fundo
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
    
    // Configuração do Pop-up 
    int width = 64;
    int height = 14; 
    int x = (MAXX - width) / 2;
    int y = (MAXY - height) / 2;

    telaSetColor(WHITE, BLUE);

    char linhaFundo[width + 1];
    memset(linhaFundo, ' ', width);
    linhaFundo[width] = '\0';

    for(int i=0; i<height; i++) {
        telaDrawText(x, y+i, linhaFundo);
    }

    const char* titulo = "LOGITECH";
    telaDrawText(x + (width - strlen(titulo))/2, y + 1, (char*)titulo);

    char div[width - 3];
    memset(div, '-', width - 4);
    div[width - 4] = '\0';
    telaDrawText(x + 2, y + 2, div);

    const char* msg = "Olá, eu sou o supercomputador LogiteCh e irei lhe ajudar na sua jornada. Para " 
    "ativar power ups ao passar em cada rodada você deve me passar um comando em lógica proposicional. "
    "Para MOVER a nave para ESQUERDA e DIREITA você deve usar as teclas A e D, respectivamente. E para ATIRAR "
    "usar a tecla BACKSPACE. Vamos começar e boa sorte na sua missão!";
    
    // Usa a função de quebrar texto automático dentro da caixa azul
    telaDesenharTextoQuebrado(x + 2, y + 4, msg, width - 4);

    
    telaSetColor(YELLOW, BLACK);
    const char* prompt = "Pressione ENTER para iniciar a missão.";
    telaDrawText((MAXX - strlen(prompt))/2, MAXY - 3, (char*)prompt);
    
    telaSetColor(WHITE, BLACK); // Reset final
}

// --- DESENHAR O BOSS ---
void desenharBoss(int xCentro, int yTopo) {
    int xIni = xCentro - (BOSS_WIDTH / 2);
    int yIni = yTopo;

    for (int i = 0; i < BOSS_HEIGHT; i++) {
        int y = yIni + i;
        if (y >= MAXY - 1) break; 

        int len = strlen(BOSS_ART[i]);
        for (int j = 0; j < len; j++) {
            int x = xIni + j;
            if (x <= MINX || x >= MAXX - 1) continue; 

            char c = BOSS_ART[i][j];
            if (c == ' ') continue; 

            screenColor cor = WHITE;
            
            if (c == 'O') cor = RED; 
            else if (c == 'V') cor = YELLOW; 
            else if (c == '_' || c == '-') cor = CYAN;
            else cor = WHITE;
            
            telaSetColor(cor, BLACK);
            char str[2] = {c, '\0'};
            telaDrawText(x, y, str);
        }
    }
}

void telaDesenharFaseGalaga(void* navePtr, void* inimigosPtr, void* tirosPtr, void* tirosInimigosPtr, void* estrelasPtr, const char* nome, int pontuacao, int rodada) {
    
    NaveJogador* nave = (NaveJogador*)navePtr;
    Inimigo* inimigos = (Inimigo*)inimigosPtr;
    Tiro* tiros = (Tiro*)tirosPtr;
    Tiro* tirosInimigos = (Tiro*)tirosInimigosPtr;
    Estrela* estrelas = (Estrela*)estrelasPtr;

    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 

    for (int i = 0; i < MAX_ESTRELAS; i++) {
        telaSetColor((screenColor)estrelas[i].cor, BLACK);
        char ponto[2] = {'.', '\0'};
        if (estrelas[i].x > MINX && estrelas[i].x < MAXX-1 && 
            estrelas[i].y > MINY && estrelas[i].y < MAXY-1) {
            telaDrawText(estrelas[i].x, estrelas[i].y, ponto);
        }
    }
    
    desenharPlacar(nome, pontuacao);
    telaSetColor(CYAN, BLACK);
    char bufRodada[20];
    sprintf(bufRodada, "RODADA %d/5", rodada);
    telaDrawText(3, 2, bufRodada);

    int x = nave->x;
    int y = nave->y;
    
    telaSetColor(RED, BLACK); 
    telaDrawText(x, y, "^");
    telaDrawText(x - 1, y + 1, "/");
    telaSetColor(WHITE, BLACK);
    telaDrawText(x, y + 1, "C"); 
    telaSetColor(RED, BLACK);
    telaDrawText(x + 1, y + 1, "\\");
    
    telaSetColor(RED, BLACK); 
    telaDrawText(x - 3, y + 2, "*");
    telaSetColor(WHITE, BLACK); 
    telaDrawText(x - 2, y + 2, "-----");
    telaSetColor(RED, BLACK);
    telaDrawText(x + 3, y + 2, "*");

    if (nave->escudoAtivo) {
        telaSetColor(CYAN, BLACK);
        telaDrawText(x - 4, y - 1, "/"); 
        telaDrawText(x + 4, y - 1, "\\");
        telaDrawText(x - 3, y - 2, "-------");
    }

    // --- DESENHO DOS INIMIGOS (COM O BOSS) ---
    for (int i = 0; i < MAX_INIMIGOS; i++) {
        if (inimigos[i].vivo) {
            int ex = inimigos[i].x;
            int ey = inimigos[i].y;
            
            if (inimigos[i].tipo == 1 && rodada == 5) {
                desenharBoss(ex, ey);
            } 
            else {
                if (i % 2 == 0) telaSetColor(MAGENTA, BLACK); 
                else telaSetColor(GREEN, BLACK);

                telaDrawText(ex - 2, ey, "* - *");
                telaDrawText(ex - 1, ey + 1, "\\ /");
                telaDrawText(ex, ey + 2, "V");
            }
        }
    }

    for (int i = 0; i < MAX_TIROS; i++) {
        if (tiros[i].ativo) {
            if (tiros[i].ehMaster) { telaSetColor(YELLOW, BLACK); telaDrawText(tiros[i].x, tiros[i].y, "*"); } 
            else { telaSetColor(CYAN, BLACK); telaDrawText(tiros[i].x, tiros[i].y, "|"); }
        }
    }

    telaSetColor(YELLOW, BLACK);
    for (int i = 0; i < MAX_TIROS_INIMIGOS; i++) {
        if (tirosInimigos[i].ativo) {
            telaDrawText(tirosInimigos[i].x, tirosInimigos[i].y, "v"); 
        }
    }

    telaSetColor(WHITE, BLACK);
}

void telaDesenharVitoriaFase(int pontuacao) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
    telaSetColor(WHITE, BLACK);

    const char* msg1 = "VOCÊ SOBREVIVEU... POR ENQUANTO!";
    const char* msg2 = "PREPARE-SE PARA O PRÓXIMO DESAFIO.";
    
    telaDrawText((MAXX - strlen(msg1)) / 2, 9, (char*)msg1);
    telaDrawText((MAXX - strlen(msg2)) / 2, 11, (char*)msg2);

    char buffer[50];
    sprintf(buffer, "Pontuação Atual: %04d", pontuacao);
    telaSetColor(CYAN, BLACK);
    telaDrawText((MAXX - strlen(buffer)) / 2, 14, buffer);
    
    telaSetColor(YELLOW, BLACK);
    const char* prompt = "Pressione ENTER para continuar.";
    telaDrawText((MAXX - strlen(prompt)) / 2, MAXY - 4, (char*)prompt);
    telaSetColor(WHITE, BLACK);
}

// --- CUTSCENE FINAL ---
void telaDesenharCutsceneFinal(void) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
    telaSetColor(WHITE, BLACK);

    const char* l1 = "ESSA ÚLTIMA VITÓRIA FOI CAPAZ DE TRAZER VOCÊ DE VOLTA PARA O";
    const char* l2 = "PRESENTE E FAZER VOCÊ ALCANCAR SEU OBJETIVO PRINCIPAL:";
    const char* l3 = "PASSAR EM PIF! AGORA VOCÊ PODE SEGUIR PARA O TERCEIRO";
    const char* l4 = "PERíODO TRANQUILO, OUTROS DESAFIOS LHE AGuARDAM POR LÁ.";
    const char* l5 = "Ah, não podemos esquecer que no processo você salvou ";
    const char* l6 = "o mundo, você é um herói, parabéns!";

    int y = 3;
    telaDrawText((MAXX - strlen(l1))/2, y++, (char*)l1);
    telaDrawText((MAXX - strlen(l2))/2, y++, (char*)l2);
    telaDrawText((MAXX - strlen(l3))/2, y++, (char*)l3);
    telaDrawText((MAXX - strlen(l4))/2, y++, (char*)l4);
    telaDrawText((MAXX - strlen(l5))/2, y++, (char*)l5);
    telaDrawText((MAXX - strlen(l6))/2, y++, (char*)l6);

    // Fachada CESAR School ASCII
    int baseY = 11;
    int baseX = 10;
    
    telaSetColor(RED, BLACK); 
    
    const char* predio[] = {
        "   ___________________________________________________________   ",
        "  |                       |                       |           |  ",
        "  |                       |                       |   (C)     |  ",
        "  |                       |                       |  CESAR    |  ",
        "  |      _______          |      _______          |           |  ",
        "  |     |       |         |     |       |         |           |  ",
        "  |     |       |         |     |       |         |     *     |  ",
        "  |     |       |    *    |     |       |         |    /|\\    |  ",
        "  |     |       |   /|\\   |     |       |         |   / | \\  |  ",
        "  |_____|_______|____|____|_____|_______|_________|_____|_____|  ",
        "    // // // // // // // // // // // // // // // // // // //     "
    };

    for(int i=0; i<11; i++) {
        telaDrawText(baseX, baseY + i, (char*)predio[i]);
    }

    
    telaSetColor(YELLOW, BLACK);
    const char* prompt = "Pressione ENTER para ir para o rank.";
    telaDrawText((MAXX - strlen(prompt))/2, MAXY - 2, (char*)prompt);
    telaSetColor(WHITE, BLACK);
}

// --- TELAS DE GAME OVER E VITÓRIA ---

void telaDesenharGameOver(bool venceu, int pontuacaoFinal) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
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

    char buffer[50];
    sprintf(buffer, "Pontuação Final: %04d", pontuacaoFinal);
    telaSetColor(CYAN, BLACK);
    telaDrawText((MAXX - strlen(buffer)) / 2, 15, buffer);
    telaSetColor(WHITE, BLACK);
}