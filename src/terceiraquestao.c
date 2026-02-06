#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PAISES 10
#define MAX_ESPORTES 100

struct EsporteCont {
    char nome[50];
    int cont;
};

struct Paises {
    char NOC[10]; 
    int medalhas;
    char esporte_destaque[50];
    struct EsporteCont esportes[MAX_ESPORTES];
    int total_esportes;
};

// Função simples para limpar \n, \r e espaços do início
void limpar(char *str) {
    str[strcspn(str, "\r\n")] = 0;
    if (str[0] == ' ') {
        for (int i = 0; i < strlen(str); i++) {
            str[i] = str[i + 1];
        }
    }
}

int main() {
    struct Paises paises[MAX_PAISES];
    int ano, total_digitado = 0;
    char edicao[20], str[2048];

    printf("Escolha o ano (ex: 1920):\n");
    scanf("%d", &ano);
    getchar(); 

    printf("Escolha a edicao (Summer ou Winter):\n");
    fgets(edicao, 20, stdin);
    limpar(edicao);

    for (int i = 0; i < MAX_PAISES; i++) {
        // O loop 'do-while' impede que o programa pule se você der um Enter vazio
        do {
            printf("Escolha o %d° pais (NOC) ou digite 'FIM' para processar: ", i + 1);
            fgets(paises[i].NOC, 10, stdin);
            limpar(paises[i].NOC);

            if (strlen(paises[i].NOC) == 0) {
                printf("Erro: Nome do pais nao pode ser vazio!\n");
            }
        } while (strlen(paises[i].NOC) == 0);

        // Se digitar FIM, para de pedir e vai para o arquivo
        if (strcmp(paises[i].NOC, "FIM") == 0) {
            break;
        }

        paises[i].medalhas = 0;
        paises[i].total_esportes = 0;
        strcpy(paises[i].esporte_destaque, "");
        total_digitado++;
    }

    FILE *ptr = fopen("data/results.csv", "r");
    if (ptr == NULL) {
        printf("Erro: Arquivo data/results.csv nao encontrado!\n");
        return 1;
    }

    fgets(str, sizeof(str), ptr); // Pula cabeçalho

    while (fgets(str, sizeof(str), ptr) != NULL) {
        int coluna = 0, dentro_aspas = 0, k = 0;
        char token[200] = "";
        int ano_csv = 0;
        char edicao_csv[20] = "", esporte_csv[50] = "", noc_csv[10] = "", medalha_csv[20] = "";

        for (int j = 0; j <= strlen(str); j++) {
            char c = str[j];
            if (c == '"') { dentro_aspas = !dentro_aspas; continue; }

            if ((c == ',' && !dentro_aspas) || c == '\0' || c == '\n') {
                token[k] = '\0';
                limpar(token);

                if (coluna == 0) ano_csv = atoi(token);
                if (coluna == 1) strcpy(edicao_csv, token);
                if (coluna == 2) strcpy(esporte_csv, token);
                if (coluna == 6) strcpy(noc_csv, token);
                if (coluna == 10) strcpy(medalha_csv, token);

                coluna++; k = 0;
                if (c == '\0' || c == '\n') break;
            } else {
                token[k++] = c;
            }
        }

        if (ano_csv == ano && strcmp(edicao_csv, edicao) == 0) {
            if (strcmp(medalha_csv, "NA") != 0 && strlen(medalha_csv) > 1) {
                for (int i = 0; i < total_digitado; i++) {
                    if (strcmp(noc_csv, paises[i].NOC) == 0) {
                        paises[i].medalhas++;
                        int achou = 0;
                        for (int j = 0; j < paises[i].total_esportes; j++) {
                            if (strcmp(paises[i].esportes[j].nome, esporte_csv) == 0) {
                                paises[i].esportes[j].cont++;
                                achou = 1;
                                break;
                            }
                        }
                        if (!achou && paises[i].total_esportes < MAX_ESPORTES) {
                            strcpy(paises[i].esportes[paises[i].total_esportes].nome, esporte_csv);
                            paises[i].esportes[paises[i].total_esportes].cont = 1;
                            paises[i].total_esportes++;
                        }
                    }
                }
            }
        }
    }
    fclose(ptr);

    for (int i = 0; i < total_digitado; i++) {
        int maior = 0;
        for (int j = 0; j < paises[i].total_esportes; j++) {
            if (paises[i].esportes[j].cont > maior) {
                maior = paises[i].esportes[j].cont;
                strcpy(paises[i].esporte_destaque, paises[i].esportes[j].nome);
            }
        }
    }

    printf("\n--- RESULTADO FINAL ---\n");
    for (int i = 0; i < total_digitado; i++) {
        printf("%s: %d medalhas. Destaque: %s\n", 
               paises[i].NOC, paises[i].medalhas, 
               paises[i].esporte_destaque[0] ? paises[i].esporte_destaque : "Nenhum");
    }

    return 0;
}