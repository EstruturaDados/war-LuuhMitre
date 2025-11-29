// ============================================================================
//         PROJETO WAR ESTRUTURADO - VERSÃO FINAL (MESTRE)
// ============================================================================
// ATENDE AOS REQUISITOS:
// - NOVATO: Structs, Vetores, Entrada/Saída.
// - AVENTUREIRO: Ponteiros, Alocação Dinâmica (Territórios), Lógica de Ataque (Metade das tropas).
// - MESTRE: Modularização, Vetor de Strings (Missões), Alocação Dinâmica (Missão), Passagem por Referência.
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// --- Constantes Globais ---
#define NUM_TERRITORIOS 5
#define NUM_MISSOES 3 // Quantidade de missões disponíveis no vetor
#define MAX_NOME 30
#define MAX_COR 20
#define MAX_TXT_MISSAO 100

// --- Estrutura de Dados (Requisito Novato) ---
struct Territorio {
    char nome[MAX_NOME];
    char cor[MAX_COR];
    int tropas;
};

// --- Protótipos das Funções (Requisito Mestre: Modularização) ---

// Gerenciamento de Memória
struct Territorio* alocarMapa(int qtd);
void liberarMemoria(struct Territorio* mapa, char* missaoJogador);

// Setup e Entrada de Dados
void inicializarTerritorios(struct Territorio* mapa);
void atribuirMissao(char* destino, const char* missoes[], int totalMissoes, int* idInterno);

// Interface
void exibirMenuPrincipal();
void exibirMapa(const struct Territorio* mapa);
void exibirMissao(const char* missao); // Passagem por valor (ponteiro para const string)

// Lógica do Jogo
void faseDeAtaque(struct Territorio* mapa, const char* corJogador);
void atacar(struct Territorio* atacante, struct Territorio* defensor); // Requisito Aventureiro
int verificarMissao(const char* missao, const struct Territorio* mapa, int tamanho, const char* corJogador, int idInterno);
void limparBufferEntrada();

// --- Função Principal (main) ---
int main() {
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL)); 

    // 1. Alocação Dinâmica do Mapa (Requisito Aventureiro)
    struct Territorio* mapa = alocarMapa(NUM_TERRITORIOS);
    if (!mapa) return 1;

    // 2. Cadastro dos Territórios (Requisito Novato)
    inicializarTerritorios(mapa);

    // 3. Configuração do Jogador e Seleção de Cor
    char corJogador[MAX_COR];
    // Vetor auxiliar para listar cores disponíveis para escolha
    char coresDisponiveis[NUM_TERRITORIOS][MAX_COR]; 
    int qtdCoresUnicas = 0;
    int escolhaCor = 0;

    printf("\n>>> CONFIGURACAO DO JOGADOR <<<\n");
    printf("Com base nos territorios cadastrados, escolha sua cor:\n");

    // Lógica para listar cores únicas
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        int jaExiste = 0;
        for (int k = 0; k < qtdCoresUnicas; k++) {
            if (strcmp(mapa[i].cor, coresDisponiveis[k]) == 0) jaExiste = 1;
        }
        if (!jaExiste) {
            strcpy(coresDisponiveis[qtdCoresUnicas], mapa[i].cor);
            printf(" %d - %s\n", qtdCoresUnicas + 1, coresDisponiveis[qtdCoresUnicas]);
            qtdCoresUnicas++;
        }
    }

    int corValida = 0;
    while (!corValida) {
        printf("Digite o numero da cor desejada: ");
        scanf("%d", &escolhaCor);
        limparBufferEntrada();
        if (escolhaCor > 0 && escolhaCor <= qtdCoresUnicas) {
            strcpy(corJogador, coresDisponiveis[escolhaCor - 1]);
            corValida = 1;
        } else {
            printf("Opcao invalida.\n");
        }
    }

    // 4. Sistema de Missões (Requisito Mestre)
    // Vetor de Strings com descrições pré-definidas
    const char* listaMissoes[NUM_MISSOES] = {
        "Dominacao Total: Conquistar todos os 5 territorios.",
        "Estrategia Defensiva: Manter todos os seus territorios com pelo menos 3 tropas.",
        "Aniquilacao: Eliminar totalmente um exercito inimigo especifico."
    };

    // Alocação dinâmica para a missão do jogador
    char* missaoJogador = (char*) malloc(MAX_TXT_MISSAO * sizeof(char));
    int idMissaoInterno = 0; // Auxiliar para lógica de verificação
    
    // Passagem por referência para atribuição
    atribuirMissao(missaoJogador, listaMissoes, NUM_MISSOES, &idMissaoInterno);

    // 5. Loop do Jogo
    int opcao = 0;
    do {
        printf("\n============================================\n");
        printf("       TURNO DO JOGADOR (%s)\n", corJogador);
        printf("============================================\n");
        
        exibirMissao(missaoJogador);
        exibirMapa(mapa);
        exibirMenuPrincipal();

        printf(" Escolha uma opcao: ");
        scanf("%d", &opcao);
        limparBufferEntrada();

        switch(opcao) {
            case 1:
                faseDeAtaque(mapa, corJogador);
                break;
            case 2:
                // Verificação da missão
                if (verificarMissao(missaoJogador, mapa, NUM_TERRITORIOS, corJogador, idMissaoInterno)) {
                    printf("\n************************************************\n");
                    printf(" PARABENS! MISSÃO CUMPRIDA: %s\n", missaoJogador);
                    printf(" VOCE VENCEU O JOGO!\n");
                    printf("************************************************\n");
                    opcao = 0; 
                } else {
                    printf("\n A missao ainda nao foi cumprida.\n");
                    printf(" Pressione ENTER para continuar...");
                    getchar();
                }
                break;
            case 0:
                printf("\n Encerrando o jogo...\n");
                break;
            default:
                printf("\n Opcao invalida!\n");
                getchar();
        }
    } while (opcao != 0);

    // 6. Liberação de Memória (Requisito Aventureiro/Mestre)
    liberarMemoria(mapa, missaoJogador);

    return 0;
}

// --- Implementação das Funções ---

struct Territorio* alocarMapa(int qtd) {
    // Uso de calloc conforme requisito Aventureiro/Mestre
    return (struct Territorio*) calloc(qtd, sizeof(struct Territorio));
}

void inicializarTerritorios(struct Territorio* mapa) {
    printf("\n=== CADASTRO DOS %d TERRITORIOS ===\n", NUM_TERRITORIOS);
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        printf("\n--- Territorio %d ---\n", i + 1);
        printf("Nome: ");
        fgets(mapa[i].nome, MAX_NOME, stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';
        
        printf("Cor do Exercito: ");
        fgets(mapa[i].cor, MAX_COR, stdin);
        mapa[i].cor[strcspn(mapa[i].cor, "\n")] = '\0';
        
        printf("Numero de Tropas: ");
        scanf("%d", &mapa[i].tropas);
        limparBufferEntrada();
    }
    printf("\nCadastro concluido.\n");
}

// Requisito Mestre: Sorteia e copia string
void atribuirMissao(char* destino, const char* missoes[], int totalMissoes, int* idInterno) {
    int sorteio = rand() % totalMissoes;
    strcpy(destino, missoes[sorteio]); // Copia a descrição para a memória alocada do jogador
    *idInterno = sorteio; // Guarda o índice para facilitar a lógica de verificação
}

void liberarMemoria(struct Territorio* mapa, char* missaoJogador) {
    if (mapa) free(mapa);
    if (missaoJogador) free(missaoJogador);
}

void exibirMenuPrincipal() {
    printf("\n--- MENU DE ACOES ---\n");
    printf(" 1. Atacar\n");
    printf(" 2. Verificar Missao\n");
    printf(" 0. Sair\n");
    printf("---------------------\n");
}

void exibirMapa(const struct Territorio* mapa) {
    printf("\n--- MAPA ATUAL ---\n");
    printf(" ID | %-15s | %-10s | TROPAS \n", "NOME", "EXERCITO");
    for(int i = 0; i < NUM_TERRITORIOS; i++) {
        printf(" %02d | %-15s | %-10s | %d\n", i+1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

void exibirMissao(const char* missao) {
    printf(" [!] OBJETIVO: %s\n", missao);
}

void faseDeAtaque(struct Territorio* mapa, const char* corJogador) {
    int origem, destino;

    printf("\n>>> FASE DE ATAQUE <<<\n");
    printf(" ID Origem (Seu: %s): ", corJogador);
    scanf("%d", &origem);
    origem--; 

    // Validações básicas
    if (origem < 0 || origem >= NUM_TERRITORIOS) { printf("ID Invalido.\n"); return; }
    if (strcmp(mapa[origem].cor, corJogador) != 0) { printf("Este territorio nao eh seu.\n"); return; }
    if (mapa[origem].tropas <= 1) { printf("Tropas insuficientes (minimo 2).\n"); return; }

    printf(" ID Destino (Inimigo): ");
    scanf("%d", &destino);
    destino--; 

    if (destino < 0 || destino >= NUM_TERRITORIOS) { printf("ID Invalido.\n"); return; }
    if (origem == destino) { printf("Nao pode atacar a si mesmo.\n"); return; }
    if (strcmp(mapa[destino].cor, corJogador) == 0) { printf("Nao pode atacar aliado.\n"); return; }

    // Chama a função 'atacar' (Requisito Aventureiro/Mestre)
    atacar(&mapa[origem], &mapa[destino]);
}

// Requisito Aventureiro: Função atacar com ponteiros e lógica de metade das tropas
void atacar(struct Territorio* atacante, struct Territorio* defensor) {
    printf("\nBatalha: %s vs %s\n", atacante->nome, defensor->nome);
    
    int dadoAtaque = (rand() % 6) + 1;
    int dadoDefesa = (rand() % 6) + 1;

    printf("Dados: Atq [%d] x Def [%d]\n", dadoAtaque, dadoDefesa);

    // Empate favorece ataque (regra citada no nível Aventureiro)
    if (dadoAtaque >= dadoDefesa) {
        printf("VITORIA DO ATAQUE! Defensor perde 1 tropa.\n");
        defensor->tropas--;

        // Lógica de Conquista
        if (defensor->tropas <= 0) {
            printf(">>> CONQUISTA! %s agora pertence ao exercito %s <<<\n", defensor->nome, atacante->cor);
            
            strcpy(defensor->cor, atacante->cor);
            
            // Requisito Aventureiro: Transferir metade das tropas
            int tropasParaMover = atacante->tropas / 2;
            if (tropasParaMover < 1) tropasParaMover = 1; // Garante ao menos 1 tropa

            defensor->tropas = tropasParaMover;
            atacante->tropas -= tropasParaMover;
            
            printf("%d tropas movidas para o novo territorio.\n", tropasParaMover);
        }
    } else {
        printf("VITORIA DA DEFESA! Atacante perde 1 tropa.\n");
        atacante->tropas--;
    }
    limparBufferEntrada();
    printf("Enter para continuar...");
    getchar();
}

// Requisito Mestre: Verificar se a missão foi cumprida
int verificarMissao(const char* missao, const struct Territorio* mapa, int tamanho, const char* corJogador, int idInterno) {
    int meusTerritorios = 0;
    int inimigosVivos = 0;
    int territoriosFortes = 0; // Para missão de defesa

    // Análise do Mapa
    for (int i = 0; i < tamanho; i++) {
        if (strcmp(mapa[i].cor, corJogador) == 0) {
            meusTerritorios++;
            if (mapa[i].tropas >= 3) territoriosFortes++;
        } else {
            inimigosVivos++;
        }
    }

    // Verifica com base no ID interno sorteado em atribuirMissao
    // 0: Dominacao Total
    if (idInterno == 0) {
        if (meusTerritorios == tamanho) return 1;
    }
    // 1: Estrategia Defensiva (ter territórios fortes)
    else if (idInterno == 1) {
        // Exemplo: Ganha se tiver pelo menos 2 territórios e todos eles tiverem >= 3 tropas
        if (meusTerritorios > 0 && territoriosFortes == meusTerritorios) return 1;
    }
    // 2: Aniquilacao (Ser o único no mapa)
    else if (idInterno == 2) {
        if (inimigosVivos == 0) return 1;
    }

    return 0;
}

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}