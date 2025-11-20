#include "fase1_forca.h"
#include "tela.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h> 

// Função auxiliar para remover o \n do final da string lida do arquivo
void trim(char* str) {
    str[strcspn(str, "\r\n")] = 0;
}

// Função auxiliar para sortear uma linha de um arquivo
char* sortearLinhaDoArquivo(const char* nomeArquivo) {
    FILE* file = fopen(nomeArquivo, "r");
    if (!file) {
        // Se não achar o arquivo, retorna um valor padrão para não travar
        char* backup = (char*)malloc(20);
        strcpy(backup, "ERRO_FILE");
        return backup;
    }

    // Conta quantas linhas tem o arquivo
    int linhas = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strlen(buffer) > 2) linhas++; // Ignora linhas vazias
    }
    rewind(file); // Volta para o inicio

    if (linhas == 0) {
        fclose(file);
        char* backup = (char*)malloc(20);
        strcpy(backup, "VAZIO");
        return backup;
    }

    // Sorteia uma linha
    int linhaSorteada = rand() % linhas;
    int atual = 0;
    char* resultado = NULL;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (strlen(buffer) <= 2) continue; // Pula vazias

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

// Seleciona a palavra secreta baseada no nível
char* getPalavraSecreta(NivelDificuldade nivel) {
    const char* arquivo = "palavras_facil.txt"; // Padrão
    
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
    
    // Separa Pergunta e Resposta
    char* pergunta = strtok(linhaCompleta, ";");
    char* respostaCorretaStr = strtok(NULL, ";");
    char respostaCorreta = 'A'; 

    if (respostaCorretaStr != NULL) {
        respostaCorreta = toupper(respostaCorretaStr[0]);
    }

    // --- DESENHO DA TELA DE AJUDA ---
    telaClear();
    
    telaSetColor(WHITE, BLACK);
    telaDrawText(5, 3, "--- AJUDA DE THOMAS BAYES ---");
    
    // Desenha a Pergunta (Quebrada em várias linhas)
    telaDesenharTextoQuebrado(5, 6, pergunta, 70);

    // Prompt de Resposta 
    telaDrawText(5, 15, "Digite sua resposta (A, B ou C): ");
    telaRefresh();
    
    int key = telaGetKey();
    char respostaUsuario = toupper(key);

    telaDrawText(5, 17, "Sua resposta: ");
    char display[2] = {respostaUsuario, '\0'};
    telaDrawText(20, 17, display);

    if (respostaUsuario == respostaCorreta) {
        telaSetColor(GREEN, BLACK);
        telaDrawText(5, 19, "CORRETO! O MISTERIO SE CLAREIA...");
        
        // Revela uma letra
        int len = strlen(palavraSecreta);
        int revelada = 0;
        // Tenta revelar uma letra ainda não descoberta
        for(int i=0; i<len; i++) {
            if(letrasDescobertas[i] == '_') {
                letrasDescobertas[i] = palavraSecreta[i];
                // Revela todas as ocorrências dessa mesma letra
                char letraRevelada = palavraSecreta[i];
                for(int j=0; j<len; j++) {
                     if(palavraSecreta[j] == letraRevelada) letrasDescobertas[j] = letraRevelada;
                }
                revelada = 1;
                break; 
            }
        }
        if(!revelada) { // Se já descobriu tudo, nada acontece
        }

    } else {
        telaSetColor(RED, BLACK);
        telaDrawText(5, 19, "ERRADO! A LOGICA É IMPLACÁVEL.");
    }
    
    telaSetColor(YELLOW, BLACK);
    telaDrawText(5, 22, "Pressione qualquer tecla para voltar...");
    telaRefresh();
    telaGetKey();

    free(linhaCompleta);
}

bool executarFaseForca(Jogador* jogador) {
    // Inicializa random
    srand(time(NULL));

    char* palavraSecreta = getPalavraSecreta(jogador->nivel);
    if (strcmp(palavraSecreta, "ERRO_FILE") == 0) {
        free(palavraSecreta);
        palavraSecreta = strdup("LOGICA"); 
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

        // Verifica Vitória
        if (strchr(letrasDescobertas, '_') == NULL) {
            vitoria = true;
            jogador->pontuacao += 100; // Bônus de Vitória
            break;
        }
        // Verifica Derrota
        if (erros >= 6) {
            vitoria = false;
            break;
        }

        int key = telaGetKey();

        // Ajuda
        if (key == '0') {
            if (ajudasRestantes > 0) {
                pedirAjudaLogica(letrasDescobertas, palavraSecreta, jogador->nivel);
                ajudasRestantes--;
                jogador->pontuacao -= 5; // Penalidade Ajuda
                if (jogador->pontuacao < 0) jogador->pontuacao = 0; // Não deixa negativo
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
            jogador->pontuacao += 10; // Pontos por acerto
        } else {
            erros++;
            letrasTentadas[numTentadas] = letraMaiuscula;
            numTentadas++;
            letrasTentadas[numTentadas] = ' '; 
            numTentadas++;
            letrasTentadas[numTentadas] = '\0';
            
            jogador->pontuacao -= 5; // Penalidade por erro
             if (jogador->pontuacao < 0) jogador->pontuacao = 0;
        }
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