#include "fase1_forca.h"
#include "tela.h" 
#include "timer.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h> 
#include <unistd.h> 

// --- FUNÇÕES AUXILIARES ---

void trim(char* str) {
    str[strcspn(str, "\r\n")] = 0;
}

char* sortearLinhaDoArquivo(const char* nomeArquivo) {
    FILE* file = fopen(nomeArquivo, "r");
    if (!file) {
        char* backup = (char*)malloc(20);
        strcpy(backup, "ERRO_FILE");
        return backup;
    }

    int linhas = 0;
    char buffer[512]; 
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strlen(buffer) > 2) linhas++; 
    }
    rewind(file); 

    if (linhas == 0) {
        fclose(file);
        char* backup = (char*)malloc(20);
        strcpy(backup, "VAZIO");
        return backup;
    }

    int linhaSorteada = rand() % linhas;
    int atual = 0;
    char* resultado = NULL;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (strlen(buffer) <= 2) continue; 

        if (atual == linhaSorteada) {
            trim(buffer);
            resultado = (char*) malloc(strlen(buffer) + 1);
            strcpy(resultado, buffer);
            break;
        }
        atual++;
    }

    fclose(file);
    return resultado;
}

char* getPalavraSecreta(NivelDificuldade nivel) {
    const char* arquivo = "palavras_facil.txt"; 
    if (nivel == MEDIO) arquivo = "palavras_medio.txt";
    else if (nivel == DIFICIL) arquivo = "palavras_dificil.txt";
    return sortearLinhaDoArquivo(arquivo);
}

// --- FUNÇÃO PEDIR AJUDA ---
void pedirAjudaLogica(char* letrasDescobertas, const char* palavraSecreta, NivelDificuldade nivel) {
    
    const char* arquivo = "perguntas_facil.txt";
    if (nivel == MEDIO) arquivo = "perguntas_medio.txt";
    else if (nivel == DIFICIL) arquivo = "perguntas_dificil.txt";

    char* linhaCompleta = sortearLinhaDoArquivo(arquivo);
    if (!linhaCompleta) return;

    // Separa a Pergunta+Opções do Gabarito (Delimitador)
    char* textoBase = strtok(linhaCompleta, ";");
    char* gabaritoStr = strtok(NULL, ";");
    char respostaCorreta = (gabaritoStr) ? toupper(gabaritoStr[0]) : 'A'; 

    // Tenta separar as opções (A), (B), (C) dentro do texto base
    char* ptrA = strstr(textoBase, "(A)");
    char* ptrB = (ptrA) ? strstr(ptrA, "(B)") : NULL;
    char* ptrC = (ptrB) ? strstr(ptrB, "(C)") : NULL;

    char pergunta[300] = "";
    char optA[100] = "";
    char optB[100] = "";
    char optC[100] = "";

    // Se conseguiu achar as 3 opções, separa as strings
    if (ptrA && ptrB && ptrC) {
        // Copia a pergunta (do inicio até o (A))
        int lenP = ptrA - textoBase;
        strncpy(pergunta, textoBase, lenP);
        pergunta[lenP] = '\0';

        // Copia A (do (A) até o (B))
        int lenA = ptrB - ptrA;
        strncpy(optA, ptrA, lenA);
        optA[lenA] = '\0';

        // Copia B (do (B) até o (C))
        int lenB = ptrC - ptrB;
        strncpy(optB, ptrB, lenB);
        optB[lenB] = '\0';

        // Copia C (do (C) até o fim)
        strcpy(optC, ptrC);
    } else {
        // se não achar a formatação, imprime tudo na pergunta
        strcpy(pergunta, textoBase);
    }

    // --- DESENHO DA TELA ---
    telaClear();
    
    // Cabeçalho 
    telaSetColor(CYAN, BLACK);
    telaDrawText(10, 5, "=== AJUDA DE THOMAS BAYES ===");
    telaDrawText(10, 7, "Responda corretamente para revelar uma letra:");
    
    // Pergunta 
    telaSetColor(WHITE, BLACK);
    // Desenha a pergunta, quebra linha se for longa
    telaDesenharTextoQuebrado(10, 10, pergunta, 60);
    
    // Opções na vertical
    telaSetColor(YELLOW, BLACK);
    if (strlen(optA) > 0) {
        telaDrawText(10, 14, optA);
        telaDrawText(10, 15, optB);
        telaDrawText(10, 16, optC);
    } else {
        // Se caiu no fallback, as opções já estão no texto da pergunta
    }

    // Prompt
    telaSetColor(WHITE, BLACK);
    telaDrawText(10, 19, "Digite sua resposta (A, B ou C): ");
    telaRefresh();
    
    // Input
    int key = telaGetKey();
    char respostaUsuario = toupper(key);

    // Mostra o que digitou
    // Limpa a linha antes
    telaDrawText(10, 19, "                                 "); 
    telaDrawText(10, 19, "Sua resposta: ");
    char display[2] = {respostaUsuario, '\0'};
    telaSetColor(YELLOW, BLACK);
    telaDrawText(25, 19, display);

    // Validação
    if (respostaUsuario == respostaCorreta) {
        telaSetColor(GREEN, BLACK);
        telaDrawText(10, 21, "CORRETO! O MISTÉRIO SE CLAREIA...");
        
        // Revela letra
        int len = strlen(palavraSecreta);
        int revelada = 0;
        for(int i=0; i<len; i++) {
            if(letrasDescobertas[i] == '_') {
                char letraAlvo = palavraSecreta[i];
                // Revela todas as ocorrências dessa letra
                for(int j=0; j<len; j++) {
                     if(palavraSecreta[j] == letraAlvo) letrasDescobertas[j] = letraAlvo;
                }
                revelada = 1;
                break; 
            }
        }
    } else {
        telaSetColor(RED, BLACK);
        telaDrawText(10, 21, "ERRADO! A LÓGICA É IMPLACÁVEL.");
    }
    
    telaRefresh();
    
    // Pausa para ler
    #ifdef _WIN32
        _sleep(1500);
    #else
        usleep(1500000); 
    #endif
    
    free(linhaCompleta);
}

bool executarFaseForca(Jogador* jogador) {
    srand(time(NULL));

    char* palavraSecreta = getPalavraSecreta(jogador->nivel);
    
    // Tratamento de string manual
    if (strcmp(palavraSecreta, "ERRO_FILE") == 0) {
        free(palavraSecreta);
        palavraSecreta = (char*)malloc(7);
        strcpy(palavraSecreta, "LOGICA");
    }

    int tamPalavra = strlen(palavraSecreta);
    char* letrasDescobertas = (char*) malloc(tamPalavra + 1);
    
    memset(letrasDescobertas, '_', tamPalavra);
    letrasDescobertas[tamPalavra] = '\0';

    char letrasTentadas[30] = "";
    int numTentadas = 0;
    int erros = 0;
    int ajudasRestantes = 3;
    bool vitoria = false;

    while (true) {
        telaClear();
        
        telaDesenharFaseForca(erros, letrasDescobertas, letrasTentadas, jogador->nome, jogador->pontuacao);
        
        telaRefresh();

        if (strchr(letrasDescobertas, '_') == NULL) {
            vitoria = true;
            jogador->pontuacao += 100; 
            break;
        }
        if (erros >= 6) {
            vitoria = false;
            break;
        }

        int key = telaGetKey();

        if (key == '0') {
            if (ajudasRestantes > 0) {
                pedirAjudaLogica(letrasDescobertas, palavraSecreta, jogador->nivel);
                ajudasRestantes--;
                jogador->pontuacao -= 5; 
                if (jogador->pontuacao < 0) jogador->pontuacao = 0; 
            }
            continue;
        }

        if (key < 0 || !isalpha(key)) continue;

        char letraMaiuscula = toupper(key);

        if (strchr(letrasTentadas, letraMaiuscula) != NULL || strchr(letrasDescobertas, letraMaiuscula) != NULL) {
            continue; 
        }

        bool acertou = false;
        for (int i = 0; i < tamPalavra; i++) {
            if (palavraSecreta[i] == letraMaiuscula) {
                letrasDescobertas[i] = letraMaiuscula;
                acertou = true;
            }
        }

        if (acertou) {
            jogador->pontuacao += 10; 
        } else {
            erros++;
            letrasTentadas[numTentadas] = letraMaiuscula;
            numTentadas++;
            letrasTentadas[numTentadas] = ' '; 
            numTentadas++;
            letrasTentadas[numTentadas] = '\0';
            
            jogador->pontuacao -= 5; 
             if (jogador->pontuacao < 0) jogador->pontuacao = 0;
        }
    }

    if (vitoria) {
        telaSetColor(GREEN, BLACK);
        telaDrawText(30, 12, "VOCÊ SE SAFOU!");
        telaRefresh();
        #ifdef _WIN32
            _sleep(2000);
        #else
            usleep(2000000);
        #endif
    }

    if (vitoria) {
        telaClear();
        telaDesenharVitoriaFase1(jogador->pontuacao);
        telaRefresh();
        while(true) {
            int k = telaGetKey();
            if (k == 13 || k == 10) break;
        }
    } 

    free(palavraSecreta);
    free(letrasDescobertas);
    return vitoria;
}