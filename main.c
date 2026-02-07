#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char linha[1024];
    FILE *arquivo = fopen("dados/results.csv", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo results.csv!\n");
        return 1;
    }
    printf("Iniciando Leitura dos Dados \n\n");

    while (fgets(linha, 1024, arquivo)) {
        char *coluna = strtok(linha, ",");
        while (coluna != NULL) {
            printf("%s | ", coluna);
            coluna = strtok(NULL, ",");
        }
        printf("\n");
    }
    fclose(arquivo);
    printf("\n Leitura Finalizada com Sucesso \n");
    return 0;
}