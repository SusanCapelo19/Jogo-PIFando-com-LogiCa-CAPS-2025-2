#include "fase2_bomba.h"
#include "tela.h"
#include "logica.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// INCLUI O TIMER DA BIBLIOTECA
#include "timer.h" 

// Variáveis Globais da Fase
static Proposicao* matrizExpressoes[9];
static bool gabaritoAlvo[9];
static bool valoresAtomos[3]; // P=0, Q=1, R=2
static const char nomesAtomos[3] = {'P', 'Q', 'R'};

// --- FUNÇÕES AUXILIARES ---

// Função duplicar string
char* duplicarString(const char* s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char* novo = (char*)malloc(len);
    if (novo) {
        memcpy(novo, s, len);
    }
    return novo;
}

void trimBomba(char* str) {
    str[strcspn(str, "\r\n")] = 0;
}

char* sortearDicaDoArquivo(NivelDificuldade nivel) {
    const char* nomeArquivo = "dicas_facil.txt";
    if (nivel == MEDIO) nomeArquivo = "dicas_medio.txt";
    else if (nivel == DIFICIL) nomeArquivo = "dicas_dificil.txt";

    FILE* file = fopen(nomeArquivo, "r");
    if (!file) return duplicarString("DICA: Use a logica das tabelas-verdade!");

    int linhas = 0;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strlen(buffer) > 2) linhas++;
    }
    rewind(file);

    if (linhas == 0) { fclose(file); return duplicarString("DICA: Sem dicas."); }

    int linhaSorteada = rand() % linhas;
    int atual = 0;
    char* resultado = NULL;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (strlen(buffer) <= 2) continue;
        if (atual == linhaSorteada) {
            trimBomba(buffer);
            resultado = duplicarString(buffer);
            break;
        }
        atual++;
    }
    fclose(file);
    if (resultado == NULL) return duplicarString("DICA: Use a logica!");
    return resultado;
}

// --- LÓGICA DE GERAÇÃO ---
Proposicao* gerarExpressaoAleatoria(NivelDificuldade nivel) {
    
    int chanceAtomo, chanceNegacao;
    
    if (nivel == FACIL) {
        // FÁCIL
        chanceAtomo = 30;    // 30% de chance de ser simples (P)
        chanceNegacao = 30;  // 30% de chance de ser negação (~P)
        // Sobra 40% para ser Binário simples (P v Q)
    } else { 
        // MÉDIO E DIFÍCIL: expressões composta e negação.
        chanceAtomo = 0;    
        chanceNegacao = 10;  
    }

    int roll = rand() % 100;

    if (roll < chanceAtomo) { 
        int idx = rand() % 3;
        return criarAtomo(nomesAtomos[idx], false);
    } 
    else if (roll < chanceAtomo + chanceNegacao) {
        int idx = rand() % 3;
        return criarNegacao(criarAtomo(nomesAtomos[idx], false));
    } 
    else {
        // Operador Binário (Obrigatório para Médio/Difícil)
        int op;
        
        if (nivel == FACIL) {
             // Fácil: Só E(2) ou OU(3)
             op = (rand() % 2) + 2; 
        }
        else if (nivel == MEDIO) {
            // Médio: E, OU, IMPLICA, XOR
            op = (rand() % 4) + 2; 
        } 
        else { 
            // Dificil: Tudo (incluindo BICONDICIONAL)
            op = (rand() % 5) + 2; 
        }
        
        int idx1 = rand() % 3;
        int idx2 = rand() % 3;
        
        // Gera os operadores
        return criarOperador(
            (TipoOperador)op,
            criarAtomo(nomesAtomos[idx1], false),
            criarAtomo(nomesAtomos[idx2], false)
        );
    }
}

void atualizarValoresNaArvore() {
    for (int i = 0; i < 9; i++) {
        atualizarValorAtomo(matrizExpressoes[i], 'P', valoresAtomos[0]);
        atualizarValorAtomo(matrizExpressoes[i], 'Q', valoresAtomos[1]);
        atualizarValorAtomo(matrizExpressoes[i], 'R', valoresAtomos[2]);
    }
}

bool verificarVitoria() {
    atualizarValoresNaArvore();
    for (int i = 0; i < 9; i++) {
        if (avaliar(matrizExpressoes[i]) != gabaritoAlvo[i]) return false;
    }
    return true;
}

// --- FUNÇÃO PRINCIPAL DA FASE 2 ---
bool executarFaseBomba(Jogador* jogador) {
    
    // Inicialização
    valoresAtomos[0] = false; valoresAtomos[1] = false; valoresAtomos[2] = false;

    for (int i = 0; i < 9; i++) matrizExpressoes[i] = gerarExpressaoAleatoria(jogador->nivel);

    valoresAtomos[0] = rand() % 2; valoresAtomos[1] = rand() % 2; valoresAtomos[2] = rand() % 2;
    atualizarValoresNaArvore();
    for (int i = 0; i < 9; i++) gabaritoAlvo[i] = avaliar(matrizExpressoes[i]);

    valoresAtomos[0] = false; valoresAtomos[1] = false; valoresAtomos[2] = false;
    atualizarValoresNaArvore();

    

    int selecionado = 0; 
    
    // CONFIGURAÇÃO DO TEMPO
    int tempoRestante = 240; // Segundos
    if (jogador->nivel == MEDIO) tempoRestante = 180;
    if (jogador->nivel == DIFICIL) tempoRestante = 120;

    // Inicializa o timer em milisegundos  
    timerInit(50); 
    int contadorTicks = 0; // Para contar os segundos

    bool vitoria = false;
    bool derrota = false;
    bool ajudaAtiva = false;
    char textoAjuda[256] = ""; 

    // --- GAME LOOP ---
    // O loop roda livremente para pegar input rápido,mas a tela e o tempo só atualizam quando o timer mandar.
    while (!vitoria && !derrota) {
        
        // Lógica do Timer da Biblioteca
        if (timerTimeOver()) {
            // Passaram-se 50ms
            contadorTicks++;
            
            // A cada 20 ticks (20 * 50ms = 1000ms), passou 1 segundo real
            if (contadorTicks >= 20) {
                tempoRestante--;
                contadorTicks = 0;
                
                if (tempoRestante <= 0) {
                    derrota = true;
                }
            }

            // Atualiza a tela a cada frame do timer
            telaClear();
            telaDesenharFaseBomba(gabaritoAlvo, matrizExpressoes, valoresAtomos, selecionado, tempoRestante, ajudaAtiva, textoAjuda, jogador->nome, jogador->pontuacao);
            telaRefresh();
        }

        // Input (Verifica teclado)
        if (keyhit()) {
            int c = readch();
            
            if (c == 'C' || c == 'd' || c == 'D') { 
                selecionado++; if (selecionado > 2) selecionado = 0;
            }
            else if (c == 'D' || c == 'a' || c == 'A') { 
                selecionado--; if (selecionado < 0) selecionado = 2;
            }
            else if (c == ' ') { 
                valoresAtomos[selecionado] = !valoresAtomos[selecionado];
                atualizarValoresNaArvore(); 
                if (verificarVitoria()) vitoria = true;
            }
            else if (c == '0') { 
                ajudaAtiva = !ajudaAtiva; 
                if (ajudaAtiva) {
                    char* dica = sortearDicaDoArquivo(jogador->nivel);
                    strncpy(textoAjuda, dica, 255);
                    free(dica);
                    jogador->pontuacao -= 5;
                    if (jogador->pontuacao < 0) jogador->pontuacao = 0;
                }
            }
        }
        
        // Se já perdeu/ganhou, sai do loop
        if (derrota || vitoria) break;
    }

    // Limpeza
    for(int i=0; i<9; i++) destruirProposicao(matrizExpressoes[i]);
    
    // Importante: Desliga o timer ao sair da fase
    timerDestroy(); 

    if (vitoria) {
        telaClear();
        telaSetColor(GREEN, BLACK);
        telaDrawText(30, 12, "BOMBA DESARMADA!");
        
        // Adiciona pontos
        jogador->pontuacao += 200;
        // Bônus por tempo (1 ponto por segundo sobrando)
        jogador->pontuacao += tempoRestante;
        
        telaRefresh();
        // Pequena espera
        timerInit(2000); // 2 segundos
        while (!timerTimeOver());
        timerDestroy();
    }

    return vitoria;
}   