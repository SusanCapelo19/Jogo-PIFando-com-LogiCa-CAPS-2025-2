#include "logica.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Proposicao* criarAtomo(char nome, bool valorInicial) {
    Proposicao* novo = (Proposicao*) malloc(sizeof(Proposicao));
    if (novo != NULL) {
        novo->tipo = ATOMO;
        novo->nomeAtomo = nome;
        novo->valorAtomo = valorInicial;
        novo->esq = NULL;
        novo->dir = NULL;
    }
    return novo;
}

Proposicao* criarOperador(TipoOperador tipo, Proposicao* esq, Proposicao* dir) {
    Proposicao* novo = (Proposicao*) malloc(sizeof(Proposicao));
    if (novo != NULL) {
        novo->tipo = tipo;
        novo->esq = esq;
        novo->dir = dir;
    }
    return novo;
}

Proposicao* criarNegacao(Proposicao* filho) {
    Proposicao* novo = (Proposicao*) malloc(sizeof(Proposicao));
    if (novo != NULL) {
        novo->tipo = NAO;
        novo->esq = filho;
        novo->dir = NULL;
    }
    return novo;
}

// --- O CERNE DA LÓGICA ---
bool avaliar(Proposicao* p) {
    if (p == NULL) return false;

    if (p->tipo == ATOMO) {
        return p->valorAtomo;
    }

    if (p->tipo == NAO) {
        return !avaliar(p->esq);
    }
    
    // Avalia os dois lados para os operadores binários
    bool valEsq = avaliar(p->esq);
    bool valDir = avaliar(p->dir);

    if (p->tipo == E) {
        return valEsq && valDir;
    }
    
    if (p->tipo == OU) {
        return valEsq || valDir;
    }
    
    if (p->tipo == CONDICIONAL) {
        return !valEsq || valDir; // Falso apenas se V -> F
    }

    if (p->tipo == XOU) {
        // Disjunção Exclusiva: Verdadeiro se forem DIFERENTES
        return valEsq != valDir;
    }

    if (p->tipo == BICONDICIONAL) {
        // Bicondicional: Verdadeiro se forem IGUAIS
        return valEsq == valDir;
    }

    return false;
}

void destruirProposicao(Proposicao* p) {
    if (p == NULL) return;
    destruirProposicao(p->esq);
    destruirProposicao(p->dir);
    free(p);
}

void atualizarValorAtomo(Proposicao* p, char nome, bool novoValor) {
    if (p == NULL) return;
    if (p->tipo == ATOMO && p->nomeAtomo == nome) {
        p->valorAtomo = novoValor;
    }
    atualizarValorAtomo(p->esq, nome, novoValor);
    atualizarValorAtomo(p->dir, nome, novoValor);
}

void imprimirExpressao(Proposicao* p, char* buffer) {
    if (p == NULL) return;

    char temp[100];

    if (p->tipo == ATOMO) {
        sprintf(temp, "%c", p->nomeAtomo);
        strcat(buffer, temp);
    } 
    else if (p->tipo == NAO) {
        strcat(buffer, "~");
        imprimirExpressao(p->esq, buffer);
    } 
    else {
        strcat(buffer, "(");
        imprimirExpressao(p->esq, buffer);
        
        switch (p->tipo) {
            case E: strcat(buffer, " ^ "); break;
            case OU: strcat(buffer, " v "); break;
            case CONDICIONAL: strcat(buffer, " -> "); break;
            case XOU: strcat(buffer, " (+) "); break;
            case BICONDICIONAL: strcat(buffer, " <-> "); break;
            default: break;
        }
        
        imprimirExpressao(p->dir, buffer);
        strcat(buffer, ")");
    }
}