#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h> 
#include "fase3_galaga.h"
#include "tela.h"
#include "timer.h"
#include "keyboard.h"

#define _DEFAULT_SOURCE 



// --- VARIÁVEIS GLOBAIS DA FASE ---
static NaveJogador nave;
static Inimigo inimigos[MAX_INIMIGOS];
static Tiro tiros[MAX_TIROS];
static Tiro tirosInimigos[MAX_TIROS_INIMIGOS]; 
static Estrela estrelas[MAX_ESTRELAS];

// Vida do Boss
static int bossVidaAtual = 0;

// --- FUNÇÕES AUXILIARES ---

void trimLine(char* str) {
    str[strcspn(str, "\r\n")] = 0;
}

static char* duplicarString(const char* src) {
    if (src == NULL) return NULL;
    char* dest = (char*) malloc(strlen(src) + 1);
    if (dest != NULL) {
        strcpy(dest, src);
    }
    return dest;
}

char* sortearLinhaLogitech(const char* nomeArquivo) {
    FILE* file = fopen(nomeArquivo, "r");
    if (!file) return NULL;

    int linhas = 0;
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strlen(buffer) > 10) linhas++; 
    }
    rewind(file);

    if (linhas == 0) { fclose(file); return NULL; }

    int sorteio = rand() % linhas;
    int atual = 0;
    char* resultado = NULL;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (strlen(buffer) <= 10) continue;
        if (atual == sorteio) {
            trimLine(buffer);
            resultado = duplicarString(buffer); 
            break;
        }
        atual++;
    }
    fclose(file);
    return resultado;
}

bool parsePergunta(char* linha, char* p, char* a, char* b, char* c, char* resp) {
    if (linha == NULL) return false;
    char* ptr = linha;
    char* fim;

    ptr = strchr(ptr, '"'); if(!ptr) return false; ptr++;
    fim = strchr(ptr, '"'); if(!fim) return false;
    *fim = '\0'; strncpy(p, ptr, 199); p[199] = '\0'; ptr = fim + 1;

    ptr = strchr(ptr, '"'); if(!ptr) return false; ptr++;
    fim = strchr(ptr, '"'); if(!fim) return false;
    *fim = '\0'; strncpy(a, ptr, 99); a[99] = '\0'; ptr = fim + 1;

    ptr = strchr(ptr, '"'); if(!ptr) return false; ptr++;
    fim = strchr(ptr, '"'); if(!fim) return false;
    *fim = '\0'; strncpy(b, ptr, 99); b[99] = '\0'; ptr = fim + 1;

    ptr = strchr(ptr, '"'); if(!ptr) return false; ptr++;
    fim = strchr(ptr, '"'); if(!fim) return false;
    *fim = '\0'; strncpy(c, ptr, 99); c[99] = '\0'; ptr = fim + 1;

    ptr = strchr(ptr, '\''); 
    if(ptr) *resp = *(ptr+1);
    else {
        for(int i=strlen(ptr)-1; i>=0; i--) {
            if(isalpha(ptr[i])) { *resp = ptr[i]; break; }
        }
    }
    return true;
}

void initEstrelas() {
    for(int i=0; i<MAX_ESTRELAS; i++) {
        estrelas[i].x = (rand() % (MAXX - 5)) + 3; 
        estrelas[i].y = (rand() % (MAXY - 4)) + 2;
        int r = rand() % 5;
        estrelas[i].cor = (r==0)? WHITE : (r==1)? CYAN : (r==2)? MAGENTA : (r==3)? YELLOW : RED;
    }
}

void initInimigos(int rodada, NivelDificuldade nivel) {
    // Limpa inimigos e Tiros
    for(int i=0; i<MAX_INIMIGOS; i++) inimigos[i].vivo = false;
    for(int i=0; i<MAX_TIROS; i++) tiros[i].ativo = false;
    for(int i=0; i<MAX_TIROS_INIMIGOS; i++) tirosInimigos[i].ativo = false;

    // Configura Boss, na Rodada 5
    if (rodada == 5) {
        inimigos[0].vivo = true;
        inimigos[0].tipo = 1; // Boss
        inimigos[0].x = MAXX / 2;
        inimigos[0].y = 3; 
        inimigos[0].direcao = 1;
        inimigos[0].linha = 0; 
        
        // Vida do Boss
        if (nivel == FACIL) bossVidaAtual = 30;
        else if (nivel == MEDIO) bossVidaAtual = 60;
        else bossVidaAtual = 100; 

        return;
    }

    // Configura Inimigos Comuns
    int numFileiras = 0;
    int navesPorFileira[5] = {0}; 

    if (nivel == FACIL) {
        numFileiras = 2; 
        navesPorFileira[0] = (rodada % 2 != 0) ? 3 : 4; 
        navesPorFileira[1] = (rodada % 2 != 0) ? 3 : 4;
    } 
    else if (nivel == MEDIO) {
        numFileiras = 2; 
        navesPorFileira[0] = (rodada % 2 != 0) ? 4 : 5; 
        navesPorFileira[1] = (rodada % 2 != 0) ? 4 : 5;
    } 
    else { 
        if (rodada % 2 != 0) { numFileiras = 2; navesPorFileira[0]=5; navesPorFileira[1]=5; }
        else { numFileiras = 3; navesPorFileira[0]=5; navesPorFileira[1]=5; navesPorFileira[2]=3; }
    }

    int count = 0;
    int startY = 3;
    
    for (int l = 0; l < numFileiras; l++) {
        int qtd = navesPorFileira[l];
        int espacoTotal = qtd * 6; 
        int startX = (MAXX - espacoTotal) / 2;
        if (startX < 2) startX = 2;

        for (int c = 0; c < qtd; c++) {
            if (count >= MAX_INIMIGOS) break;
            
            inimigos[count].vivo = true;
            inimigos[count].tipo = 0;
            inimigos[count].linha = l;
            inimigos[count].y = startY + (l * 3);
            inimigos[count].x = startX + (c * 8); 
            inimigos[count].direcao = (l % 2 == 0) ? 1 : -1;
            
            count++;
        }
    }
}

void spawnMinionsBoss() {
    int nascidos = 0;
    for(int i=1; i<MAX_INIMIGOS; i++) {
        if(!inimigos[i].vivo && nascidos < 2) {
            inimigos[i].vivo = true;
            inimigos[i].tipo = 0;
            inimigos[i].y = 8; 
            inimigos[i].x = (nascidos == 0) ? 20 : MAXX - 20; 
            inimigos[i].linha = 1;
            inimigos[i].direcao = (rand()%2==0)? 1 : -1;
            nascidos++;
        }
    }
}

bool desafioLogitech(int rodada, NivelDificuldade nivel) {
    if (rodada >= 5) return true; 

    const char* nomeArq = (nivel == MEDIO) ? "logitch_medio.txt" : (nivel == DIFICIL) ? "logitch_dificil.txt" : "logitch_facil.txt";
    char* linhaRaw = sortearLinhaLogitech(nomeArq);
    char p[200] = "", a[100] = "", b[100] = "", c[100] = "", r = 'A';
    
    if (!linhaRaw || !parsePergunta(linhaRaw, p, a, b, c, &r)) {
        strcpy(p, "SISTEMA LOGITECH: DADOS CORROMPIDOS.");
        strcpy(a, "(A) IGNORAR"); strcpy(b, "(B) REINICIAR"); strcpy(c, "(C) CONTINUAR"); r = 'C';
    }

    telaClear();
    telaSetColor(CYAN, BLACK);
    telaDrawText(10, 5, "=== SISTEMA LOGITECH ATIVADO ===");
    telaDrawText(10, 7, "Traduza a sentenca para ganhar o Power-Up:");
    telaSetColor(WHITE, BLACK);
    telaDesenharTextoQuebrado(10, 10, p, 60);
    telaSetColor(YELLOW, BLACK);
    telaDrawText(10, 14, a); telaDrawText(10, 15, b); telaDrawText(10, 16, c);
    telaSetColor(WHITE, BLACK);
    telaDrawText(10, 19, "Digite sua resposta (A, B ou C): ");
    telaRefresh();

    bool resultado = false;
    while(true) {
        int k = telaGetKey();
        char respUser = toupper(k);
        if (respUser == 'A' || respUser == 'B' || respUser == 'C') {
            if (respUser == toupper(r)) {
                telaSetColor(GREEN, BLACK); telaDrawText(10, 21, "CORRETO! UPLOAD DE ARMA CONCLUIDO."); resultado = true;
            } else {
                telaSetColor(RED, BLACK); telaDrawText(10, 21, "ERRO DE SINTAXE. POWER-UP FALHOU."); resultado = false;
            }
            telaRefresh();
            usleep(1500000); 
            break;
        }
    }
    if(linhaRaw != NULL) free(linhaRaw); 
    return resultado;
}

void atirar() {
    int qtdTiros = 1;
    int posicoes[3] = {0, 0, 0}; 

    if (nave.armaAtual == TIRO_DUPLO) { 
        qtdTiros = 2; posicoes[0] = -2; posicoes[1] = 2; 
    } else if (nave.armaAtual >= TIRO_TRIPLO) { 
        qtdTiros = 3; posicoes[0] = 0; posicoes[1] = -2; posicoes[2] = 2; 
    } else {
        qtdTiros = 1; posicoes[0] = 0;
    }
    
    for(int t=0; t<qtdTiros; t++) {
        for(int i=0; i<MAX_TIROS; i++) {
            if (!tiros[i].ativo) {
                tiros[i].ativo = true;
                tiros[i].ehMaster = (nave.armaAtual == TIRO_MASTER);
                tiros[i].x = nave.x + posicoes[t];
                tiros[i].y = nave.y - 2;
                break; 
            }
        }
    }
}

void processarTiroInimigoSequencial(NivelDificuldade nivel, int rodada) {
    int intervalo = (nivel == FACIL) ? 100 : (nivel == MEDIO) ? 80 : 60;
    bool todosAtiram = (rodada >= 3);
    static int frameCounter = 0;
    static int turno = 0; 

    frameCounter++;
    if (frameCounter < intervalo) return;
    frameCounter = 0; 

    int idxCima[MAX_INIMIGOS], qtdCima = 0;
    int idxBaixo[MAX_INIMIGOS], qtdBaixo = 0;

    for(int i=0; i<MAX_INIMIGOS; i++) {
        if (inimigos[i].vivo && inimigos[i].tipo == 0) { 
            if (inimigos[i].linha <= 1) idxCima[qtdCima++] = i; else idxBaixo[qtdBaixo++] = i;
        }
    }
    
    // Boss atira sempre
    if (rodada == 5 && inimigos[0].vivo) { 
        for(int k=0; k<3; k++) { 
             for(int i=0; i<MAX_TIROS_INIMIGOS; i++) {
                if(!tirosInimigos[i].ativo) {
                    tirosInimigos[i].ativo = true;
                    tirosInimigos[i].x = inimigos[0].x + (k*2 - 2);
                    tirosInimigos[i].y = inimigos[0].y + 2;
                    break;
                }
             }
        }
        return;
    }

    int tc = (turno==0)?2:1; int tb = (turno==0)?1:2;
    if (!todosAtiram) { tc = (tc>1)?1:tc; tb = (tb>1)?1:tb; }

    for (int k=0; k<tc && qtdCima > 0; k++) {
        int id = idxCima[rand() % qtdCima];
        for(int t=0; t<MAX_TIROS_INIMIGOS; t++) { if(!tirosInimigos[t].ativo) { tirosInimigos[t].ativo=true; tirosInimigos[t].x=inimigos[id].x; tirosInimigos[t].y=inimigos[id].y+1; break; } }
    }
    for (int k=0; k<tb && qtdBaixo > 0; k++) {
        int id = idxBaixo[rand() % qtdBaixo];
        for(int t=0; t<MAX_TIROS_INIMIGOS; t++) { if(!tirosInimigos[t].ativo) { tirosInimigos[t].ativo=true; tirosInimigos[t].x=inimigos[id].x; tirosInimigos[t].y=inimigos[id].y+1; break; } }
    }
    turno = !turno; 
}

void verificarColisoes(Jogador* jogador) {
    for (int t = 0; t < MAX_TIROS; t++) {
        if (!tiros[t].ativo) continue;

        for (int i = 0; i < MAX_INIMIGOS; i++) {
            if (!inimigos[i].vivo) continue;

            int ex = inimigos[i].x;
            int ey = inimigos[i].y;
            int tx = tiros[t].x;
            int ty = tiros[t].y;

            bool hit = false;
            
            // Caixa de Colisão do Boss
            if (inimigos[i].tipo == 1) {
                if (tx >= ex - 14 && tx <= ex + 14 && ty >= ey && ty <= ey + 10) {
                    hit = true;
                }
            } 
            else { 
                if (tx >= ex - 2 && tx <= ex + 2 && ty >= ey && ty <= ey + 2) {
                    hit = true;
                }
            }

            if (hit) {
                tiros[t].ativo = false;
                
                if (inimigos[i].tipo == 1) { // BOSS
                    
                    int dano = 0;
                    if (jogador->nivel == FACIL) {
                        dano = (tiros[t].ehMaster) ? 3 : 2; 
                    } else if (jogador->nivel == MEDIO) {
                        dano = (tiros[t].ehMaster) ? 4 : 3; 
                    } else { 
                        dano = (tiros[t].ehMaster) ? 5 : 4; 
                    }
                    
                    bossVidaAtual -= dano;

                    if (bossVidaAtual <= 0) {
                        inimigos[i].vivo = false;
                        jogador->pontuacao += 2000;
                    }
                } else {
                    inimigos[i].vivo = false;
                    jogador->pontuacao += 50;
                    if(tiros[t].ehMaster) jogador->pontuacao += 50;
                }
                break; 
            }
        }
    }
}

bool verificarColisaoNaveOuTiroInimigo() {
    int nx = nave.x;
    int ny = nave.y;

    int pLeft = nx - 3;
    int pRight = nx + 3;
    int pTop = ny;
    int pBottom = ny + 2;

    for (int i = 0; i < MAX_INIMIGOS; i++) {
        if (!inimigos[i].vivo) continue;
        
        int ex = inimigos[i].x;
        int ey = inimigos[i].y;
        
        int eLeft, eRight, eTop, eBottom;

        if (inimigos[i].tipo == 1) { // BOSS
            eLeft = ex - 14; eRight = ex + 14;
            eTop = ey; eBottom = ey + 9;
        } else { // COMUM
            eLeft = ex - 2; eRight = ex + 2;
            eTop = ey; eBottom = ey + 2;
        }

        if (pLeft <= eRight && pRight >= eLeft && pTop <= eBottom && pBottom >= eTop) {
            if (inimigos[i].tipo == 1) return true; 

            inimigos[i].vivo = false; 
            if (nave.escudoAtivo) {
                nave.escudoAtivo = false; 
                return false; 
            } else return true; 
        }
    }

    for (int i = 0; i < MAX_TIROS_INIMIGOS; i++) {
        if (tirosInimigos[i].ativo) {
            int tx = tirosInimigos[i].x;
            int ty = tirosInimigos[i].y;

            if (tx >= pLeft && tx <= pRight && ty >= pTop && ty <= pBottom) {
                tirosInimigos[i].ativo = false; 
                
                if (nave.escudoAtivo) {
                    nave.escudoAtivo = false; 
                    return false;
                } else return true; 
            }
        }
    }
    return false; 
}

int contarInimigosVivos() {
    int vivos = 0;
    for(int i=0; i<MAX_INIMIGOS; i++) if(inimigos[i].vivo) vivos++;
    return vivos;
}

bool executarFaseGalaga(Jogador* jogador) {
    srand(time(NULL));
    nave.x = MAXX / 2; nave.y = MAXY - 5; nave.hp = 1; nave.escudoAtivo = false; nave.armaAtual = TIRO_SIMPLES;

    int rodada = 1;
    initEstrelas();
    initInimigos(rodada, jogador->nivel);
    
    // Limpa tiros iniciais
    for(int i=0; i<MAX_TIROS; i++) tiros[i].ativo = false;
    for(int i=0; i<MAX_TIROS_INIMIGOS; i++) tirosInimigos[i].ativo = false;

    timerInit(50); 
    bool vitoria = false; bool derrota = false;
    static int contadorDescida = 0; static int contadorHorizontal = 0; static int contadorSpawnBoss = 0;
    
    // Variável de Direção do Boss
    int bossDirY = 1; 

    while (!vitoria && !derrota) {
        
        if (contarInimigosVivos() == 0) {
            if (rodada >= 5) {
                vitoria = true; 
            } else {
                bool ganhouBonus = desafioLogitech(rodada, jogador->nivel);
                rodada++;
                if (ganhouBonus) {
                    if (rodada == 2) nave.escudoAtivo = true;
                    else if (rodada == 3) nave.armaAtual = TIRO_DUPLO;
                    else if (rodada == 4) nave.armaAtual = TIRO_TRIPLO;
                    else if (rodada == 5) nave.armaAtual = TIRO_MASTER;
                }
                initInimigos(rodada, jogador->nivel);
                telaClear(); 
            }
        }

        int base = (jogador->nivel == FACIL) ? 45 : (jogador->nivel == MEDIO) ? 35 : 25;
        if (rodada == 3 || rodada == 4) base -= 10;
        if (rodada == 5) base = (jogador->nivel == FACIL) ? 30 : (jogador->nivel == MEDIO) ? 20 : 15;
        int framesParaDescer = (base < 5) ? 5 : base;

        if (timerTimeOver()) {
            for(int i=0; i<MAX_TIROS; i++) {
                if (tiros[i].ativo) { tiros[i].y--; if (tiros[i].y < 2) tiros[i].ativo = false; }
            }
            for(int i=0; i<MAX_TIROS_INIMIGOS; i++) {
                if (tirosInimigos[i].ativo) { tirosInimigos[i].y++; if (tirosInimigos[i].y > MAXY - 2) tirosInimigos[i].ativo = false; }
            }

            contadorHorizontal++;
            if (contadorHorizontal > 5) { 
                int LIMITE_ESQ = 6; int LIMITE_DIR = MAXX - 6;
                bool inverterPares = false, inverterImpares = false;

                for(int i=0; i<MAX_INIMIGOS; i++) {
                    if (!inimigos[i].vivo) continue;
                    bool par = (inimigos[i].linha % 2 == 0);
                    if (par && inimigos[i].direcao == 1 && inimigos[i].x >= LIMITE_DIR) inverterPares = true;
                    else if (par && inimigos[i].direcao == -1 && inimigos[i].x <= LIMITE_ESQ) inverterPares = true;
                    if (!par && inimigos[i].direcao == 1 && inimigos[i].x >= LIMITE_DIR) inverterImpares = true;
                    else if (!par && inimigos[i].direcao == -1 && inimigos[i].x <= LIMITE_ESQ) inverterImpares = true;
                }
                for(int i=0; i<MAX_INIMIGOS; i++) {
                    if (!inimigos[i].vivo) continue;
                    if (inimigos[i].linha % 2 == 0) { if (inverterPares) inimigos[i].direcao *= -1; } 
                    else { if (inverterImpares) inimigos[i].direcao *= -1; }
                    inimigos[i].x += inimigos[i].direcao;
                }
                contadorHorizontal = 0;
            }

            contadorDescida++;
            if (contadorDescida > framesParaDescer) { 
                for(int i=0; i<MAX_INIMIGOS; i++) {
                    if(inimigos[i].vivo) {
                        
                        // *** LOGICA DE MOVIMENTO DO BOSS ***
                        if (inimigos[i].tipo == 1) { 
                            inimigos[i].y += bossDirY;
                            if (inimigos[i].y >= 9) bossDirY = -1; 
                            if (inimigos[i].y <= 2) bossDirY = 1;
                        } else {
                            inimigos[i].y++;
                            if (inimigos[i].y >= MAXY - 3) derrota = true;
                        }
                    }
                }
                contadorDescida = 0;
            }
            
            processarTiroInimigoSequencial(jogador->nivel, rodada);
            if (jogador->nivel == DIFICIL && rodada == 5) {
                contadorSpawnBoss++; if (contadorSpawnBoss >= 200) { spawnMinionsBoss(); contadorSpawnBoss = 0; }
            }

            verificarColisoes(jogador);
            if (verificarColisaoNaveOuTiroInimigo()) derrota = true;

            telaClear();
            telaDesenharFaseGalaga(&nave, inimigos, tiros, tirosInimigos, estrelas, jogador->nome, jogador->pontuacao, rodada);
            telaRefresh();
        }

        if (keyhit()) {
            int c = readch();
            if ((c == 'a' || c == 'A' || c == 'D') && nave.x > 6) nave.x--;
            if ((c == 'd' || c == 'D' || c == 'C') && nave.x < MAXX - 6) nave.x++;
            if (c == ' ' || c == 127 || c == 8) atirar();
        }
    }
    
    timerDestroy();
    if (vitoria) {
        telaSetColor(GREEN, BLACK); 
        telaDrawText(30, 12, "UNIVERSO SALVO!"); 
        telaRefresh(); 
        sleep(2);
    }
    return vitoria;
}