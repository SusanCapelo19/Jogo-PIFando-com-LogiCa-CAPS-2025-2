# Sobrevivendo a PIF com LógiCa

**Disciplina:** Programação Imperativa e Funcional — 2025.2  
**Instituição:** CESAR School  
**Projeto:** Jogo em C utilizando a biblioteca `cli-lib`.

---

## Narrativa
O jogador assume o papel de um estudante de PIF que, ao executar um código misterioso deixado pelo professor em sua máquina, abre um portal de viagem no tempo. 

Para ser aprovado na disciplina (e sobreviver), ele deve vencer três desafios em três eras diferentes (Passado, Presente e Futuro). Em cada desafio, a **Lógica Proposicional** será sua única arma e aliada para resolver os problemas e retornar ao presente.

---

## Mecânicas do Jogo

O jogo é dividido em três fases distintas, cada uma testando uma habilidade diferente de lógica e programação:

### Fase 1: O Passado (Séc. XVIII) - Jogo da Forca
* **Cenário:** Inglaterra, corte do Rei Jorge II.
* **Objetivo:** Descobrir a palavra-chave lógica para escapar da sentença de forca.
* **Mecânica:** Jogo da forca clássico. O jogador tem 6 chances de erro.
* **Ajuda:** O matemático **Thomas Bayes** oferece ajuda. O jogador pode solicitar uma dica até 3 vezes; para receber a dica (uma letra revelada), deve-se acertar uma questão de lógica proposicional.

### Fase 2: O Presente - Desarmar a Bomba
* **Cenário:** Rodoanel Mário Covas, bloqueado por um caminhão contendo uma Bomba Lógica.
* **Objetivo:** Igualar a "Matriz de Expressões" do detonador ao "Padrão Alvo" antes que o tempo acabe.
* **Mecânica:** O jogador navega por uma matriz de expressões lógicas (ex: `P ^ Q`, `~R`) e deve alterar os valores verdade dos átomos (P, Q, R) para Verdadeiro ou Falso. O feedback visual (Verde/Vermelho) indica o status da expressão.
* **Ajuda:** Dicas do **Prof. Guilherme** podem ser solicitadas para entender melhor as portas lógicas.

### Fase 3: O Futuro - Galaga Lógico
* **Cenário:** Invasão Alienígena futurista.
* **Objetivo:** Destruir as naves inimigas e o Chefão final para salvar a Terra.
* **Mecânica:** *Shooter* vertical (navinha). O jogador move-se e atira. A cada rodada, o supercomputador **LogiteCh** oferece um desafio de tradução (Linguagem Natural -> Lógica). Acertar o desafio concede *Power-Ups* (tiros duplos, triplos ou master).
* **Boss:** Na rodada final, uma nave-mãe com alta resistência e inteligência de movimento desafia o jogador.

---

## Sistema de Pontuação

A pontuação é cumulativa e define a posição do jogador no **Ranking de Sobreviventes** (Top 5 salvo em arquivo binário).

| Fase | Ação | Pontuação |
| :--- | :--- | :--- |
| **Forca** | Acerto de Letra | **+10 pts** |
| | Vitória na Fase | **+100 pts** |
| | Erro de Letra | **-5 pts** |
| | Pedir Ajuda (Bayes) | **-5 pts** |
| **Bomba** | Bomba Desarmada | **+200 pts** |
| | Tempo Restante | **+1 pt por segundo** |
| | Pedir Dica (Prof.) | **-5 pts** |
| **Galaga** | Inimigo Destruído | **+50 pts** |
| | Bônus (Arma Master) | **+50 pts extras** |
| | Derrotar o Boss | **+2.000 pts** |

---

## Equipe de Desenvolvimento

| Nome | Usuário GitHub | Responsável por |
| :--- | :--- | :--- |
| **Arthur Cavalcanti** | `@arthurccavalcanti` | Implementação dos arquivos da fase 2 (Desarmar Bomba) e lógica de árvores binárias. |
| **Jardel Simplicio** | `@JDsoli` | Implementação *in pair* da fase 3 (Galaga), mecânicas de tiro e colisão. |
| **Manoel Nascimento** | `@Manoelhns` | Implementação da fase 1 (Jogo da Forca), manipulação de strings e arquivos de jogador. |
| **Ramón Taffarel** | `@Ramontaffa` | Implementação *in pair* da fase 3 (Galaga), inteligência dos inimigos e Boss. |
| **Susan Capelo** | `@SusanCapelo19` | Criação da história, interface visual (Wrapper da `cli-lib`), questões de lógica, integração do `main` e sistema de Ranking. |

---

## Instruções de Compilação e Execução

Este projeto foi desenvolvido em C e utiliza um `Makefile` para automatizar a compilação. Certifique-se de estar em um ambiente Linux ou WSL com `gcc` e `make` instalados.

### Compilar o Jogo
No terminal, dentro da pasta raiz do projeto, execute: make.

### Executar o Jogo
Após a compilação execute: ./sobrevivendo_pif.
