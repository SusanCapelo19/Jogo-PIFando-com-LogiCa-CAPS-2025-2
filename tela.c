#include "tela.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "screen.h"
#include "keyboard.h"
#include "logica.h"

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

void telaDesenharTextoQuebrado(int x, int y, const char* texto, int larguraMaxima) {
    char buffer[256];
    int len = strlen(texto);
    int inicio = 0;
    int linha = 0;

    while (inicio < len) {
        // Calcula quanto falta
        int restante = len - inicio;
        
        // Se cabe tudo na linha, imprime e sai
        if (restante <= larguraMaxima) {
            telaDrawText(x, y + linha, (char*)(texto + inicio));
            break;
        }

        // 3. Se não cabe, corta na largura máxima
        int pontoCorte = inicio + larguraMaxima;

        // Recua para achar um espaço para não cortar palavra no meio
        int temp = pontoCorte;
        int achouEspaco = 0;
        
        // Procura um espaço voltando para trás
        while (temp > inicio) {
            if (texto[temp] == ' ') {
                pontoCorte = temp;
                achouEspaco = 1;
                break;
            }
            temp--;
        }

        // Se não achou espaço, corta na marra onde der
        if (!achouEspaco) {
            pontoCorte = inicio + larguraMaxima;
        }

        // 6. Copia o pedaço para o buffer
        int tamLinha = pontoCorte - inicio;
        strncpy(buffer, texto + inicio, tamLinha);
        buffer[tamLinha] = '\0'; // Fecha a string

        // 7. Desenha a linha
        telaDrawText(x, y + linha, buffer);

        // 8. Prepara próxima volta
        linha++;
        inicio = pontoCorte;
        
        // Se cortaR num espaço, pula ele para não começar a próxima linha com espaço
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
    int xPos = MAXX - 15; 
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
    
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
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
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
    telaSetColor(WHITE, BLACK);

    const char* msg1 = "VOCÊ SOBREVIVEU... POR ENQUANTO!";
    const char* msg2 = "PREPARE-SE PARA O PRÓXIMO DESAFIO.";
    
    telaDrawText((MAXX - strlen(msg1)) / 2, 9, (char*)msg1);
    telaDrawText((MAXX - strlen(msg2)) / 2, 11, (char*)msg2);

    // Mostra pontuação acumulada
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
    // Borda e fundo
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
    telaSetColor(WHITE, BLACK);

    // Texto da História
    const char* linha1 = "PARABÉNS! VOCÊ SOBREVIVEU À FORCA.";
    const char* linha2 = "MAS AINDA NÃO É O MOMENTO PARA COMEMORAR...";
    const char* linha3 = "A VITÓRIA REATIVOU A VIAGEM NO TEMPO E AGORA TE TROUXE DE";
    const char* linha4 = "VOLTA PARA O PRESENTE, PARA DESARMAR UMA BOMBA LÓGICA, QUE FOI";
    const char* linha5 = "COLOCADA EM UM CAMINHÃO QUE ESTÁ BLOQUEANDO ";
    const char* linha6 = "MÁRIO COVAS. VOCÊ TEM POUCOS MINUTOS PARA DESARMÁ-LA!";
    const char* linha7 = "MAS VOCÊ PODE CONTAR COM AS DICAS DO";
    const char* linha8 = "PROFESSOR GUILHERME PARA LHE AJUDAR.";
        
    const char* prompt = "Pressione ENTER para aceitar a missão.";

    // Posicionamento do Texto
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
        
    // Caminhão em ASCII Art 
    int xTruck = 22; 
    int yTruck = 11; // Começa na linha 14

    telaSetColor(RED, BLACK); 
    telaDrawText(xTruck, yTruck++, "      ____________________");
    telaDrawText(xTruck, yTruck++, "     /                    |");
    
    // Lateral com nome do Cesar
    telaDrawText(xTruck, yTruck,   "    |   ");
    telaSetColor(RED, BLACK); 
    telaDrawText(xTruck + 8, yTruck, "CESAR  SCHOOL"); 
    telaSetColor(BLUE, BLACK);
    telaDrawText(xTruck + 22, yTruck++, "   |");
    
    // Parte de baixo
    telaDrawText(xTruck, yTruck++, "    |____________________ |__");
    telaDrawText(xTruck, yTruck++, "    |  ___   ___   ___  |  __|");
    telaDrawText(xTruck, yTruck++, "    | |   | |   | |   | | |  |"); 
    telaDrawText(xTruck, yTruck++, "    |_|___|_|___|_|___|_|_|__|");
    
    // Rodas
    telaSetColor(DARKGRAY, BLACK);
    telaDrawText(xTruck, yTruck++, "     (O) (O) (O)       (O) (O)");

    // Prompt Final
    telaSetColor(YELLOW, BLACK);
    telaDrawText((MAXX - strlen(prompt)) / 2, MAXY - 2, (char*)prompt);
    telaSetColor(WHITE, BLACK);
}

void telaDesenharFaseBomba(bool alvo[9], Proposicao* matriz[9], bool atomos[3], int selecionado, int tempoRestante, bool ajudaAtiva, const char* textoAjuda, const char* nome, int pontuacao) {
    
    // Borda Principal (Tela Cheia)
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 

    // Placar (Topo Direito)
    desenharPlacar(nome, pontuacao);
    
    // Timer (Topo Direito)
    char buffer[50];
    int minutos = tempoRestante / 60;
    int segundos = tempoRestante % 60;
    
    if (tempoRestante < 60) telaSetColor(RED, BLACK); 
    else telaSetColor(CYAN, BLACK);
    
    sprintf(buffer, "TEMPO: %02d:%02d", minutos, segundos);
    telaDrawText(3, 2, buffer);
    
    // Matriz ALVO (Pequeno, no topo central)
    telaSetColor(WHITE, BLACK);
    const char* tituloAlvo = "PADRAO ALVO:";
    telaDrawText((MAXX - strlen(tituloAlvo))/2, 3, (char*)tituloAlvo);
    
    int xAlvoBase = (MAXX - (3 * 4)) / 2; 
    int yAlvoBase = 4;

    // Desenha o gabarito simples
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

    // 4. MATRIZ DO JOGADOR
    // Configurações da Grade
    int larguraCelula = 22;
    int xInicioGrade = 6;
    int yInicioGrade = 8; 
    
    // Linha de topo da tabela
    telaSetColor(WHITE, BLACK);
    telaDrawText(xInicioGrade, yInicioGrade, " ______________________ ______________________ ______________________ ");

    for (int i = 0; i < 3; i++) { // 3 Linhas
        int yLinhaAtual = yInicioGrade + 1 + (i * 2); // Pula de 2 em 2 linhas
        
        for (int j = 0; j < 3; j++) { // 3 Colunas
            int idx = i * 3 + j;
            int xColunaAtual = xInicioGrade + (j * (larguraCelula + 1)); // +1 pelo caractere '|'

            // Desenha as bordas laterais da célula
            telaSetColor(WHITE, BLACK);
            telaDrawText(xColunaAtual, yLinhaAtual, "|");
            
            // Se for a última coluna, desenha a borda final
            if (j == 2) telaDrawText(xColunaAtual + larguraCelula + 1, yLinhaAtual, "|");

            // Avalia e define a cor do texto
            bool valorAtual = avaliar(matriz[idx]);
            
            if (valorAtual) telaSetColor(GREEN, BLACK); // V = Verde
            else telaSetColor(RED, BLACK);             // F = Vermelho

            // Prepara o texto da expressão
            char textoExpr[100] = "";
            imprimirExpressao(matriz[idx], textoExpr);
            
            // Centraliza o texto dentro da célula
            int lenExpr = strlen(textoExpr);
            int padding = (larguraCelula - lenExpr) / 2;
            if (padding < 0) padding = 0; 

            telaDrawText(xColunaAtual + 1 + padding, yLinhaAtual, textoExpr);
        }

        // Desenha a linha inferior da célula
        telaSetColor(WHITE, BLACK);
        char linhaFundo[100] = "";
        // Constrói a string da linha de baixo
        sprintf(linhaFundo, "|______________________|______________________|______________________|");
        telaDrawText(xInicioGrade, yLinhaAtual + 1, linhaFundo);
    }


    // Controles (Variáveis P, Q, R) - Abaixo da grade
    telaSetColor(WHITE, BLACK);
    int xVars = 20;
    int yVars = 18;
    const char* nomes[3] = {"P", "Q", "R"};
    
    for(int i=0; i<3; i++) {
        int xPos = xVars + (i * 15);
        
        // Seta de seleção
        if (i == selecionado) {
            telaSetColor(YELLOW, BLACK);
            telaDrawText(xPos, yVars - 1, "  v  ");
        } else {
            telaDrawText(xPos, yVars - 1, "     ");
        }
        
        // Valor
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

    // Rodapé
    telaSetColor(YELLOW, BLACK);
    const char* instrucoes = "ESPACO: Mudar Valor | SETAS: Selecionar | '0': Dica Prof. Guilherme";
    telaDrawText((MAXX - strlen(instrucoes))/2, MAXY - 3, (char*)instrucoes);

    // Pop-up de Ajuda
    if (ajudaAtiva) {
        telaSetColor(WHITE, BLUE); // Fundo Azul
        
        int yAjuda = 12;
        int xCaixa = 10;
        int larguraCaixa = 60; // Largura fixa da caixa azul
        
        // Cria uma linha de fundo azul vazia
        char fundo[100];
        memset(fundo, ' ', larguraCaixa);
        fundo[larguraCaixa] = '\0';

        // Desenha o quadrado azul (6 linhas de altura)
        for(int k = 0; k < 7; k++) {
            telaDrawText(xCaixa, yAjuda + k, fundo);
        }

        // Título Centralizado
        const char* titulo = "DICA DO PROF. GUILHERME";
        int xTitulo = xCaixa + (larguraCaixa - strlen(titulo)) / 2;
        telaDrawText(xTitulo, yAjuda + 1, (char*)titulo);
        
        // Linha decorativa
        char linhaSep[100];
        memset(linhaSep, '=', larguraCaixa - 4);
        linhaSep[larguraCaixa - 4] = '\0';
        telaDrawText(xCaixa + 2, yAjuda + 2, linhaSep);

        // Texto da Dica 
        telaDesenharTextoQuebrado(xCaixa + 4, yAjuda + 3, textoAjuda, larguraCaixa - 8);
        
        telaSetColor(WHITE, BLACK); // Volta ao normal
    }
}

void telaDesenharFaseGalaga(const char* nome, int pontuacao) {
    telaDesenharJanela(MINX, MINY, MAXX - 1, MAXY - 1, NULL); 
    desenharPlacar(nome, pontuacao);
    telaSetColor(WHITE, BLACK);
    telaDrawText(10, 5, "FASE 3 (Ainda nao implementado)");
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

    // Mostra pontuação final
    char buffer[50];
    sprintf(buffer, "Pontuacao Final: %04d", pontuacaoFinal);
    telaSetColor(CYAN, BLACK);
    telaDrawText((MAXX - strlen(buffer)) / 2, 15, buffer);
    telaSetColor(WHITE, BLACK);
}