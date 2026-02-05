#include <stdio.h>
#include <string.h>

struct Paises{
    char nome[50];
    int medalhas;
    char esporte[50];
};

int main() {
    int ano;
    char edicao[10];
    struct Paises paises[10];

    char str[1024]; // buffer maior pra CSV
    FILE* ptr = fopen("data/results.csv", "r");
    if(ptr == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return 1;
    }

    // Apenas imprime o CSV por enquanto
    while(fgets(str, sizeof(str), ptr) != NULL) {
        printf("%s", str);
    }

    fclose(ptr);

    // Agora perguntas pro usuário
    printf("\nEscolha a edição das Olimpíadas de seu interesse:\n"); 
    fgets(edicao, sizeof(edicao), stdin);
    edicao[strcspn(edicao, "\n")] = 0; // remove o \n

    printf("Escolha o ano:\n");
    scanf("%d", &ano);
    getchar(); // limpa o \n do buffer

    // Leitura dos 10 países
    for(int i = 0; i < 10; i++){
        printf("Escolha o %d° país:\n", i+1);
        fgets(paises[i].nome, sizeof(paises[i].nome), stdin);
        paises[i].nome[strcspn(paises[i].nome, "\n")] = 0; // remove \n
        paises[i].medalhas = 0; // inicializa
        strcpy(paises[i].esporte, ""); // inicializa
    }

    // Mostrar os países cadastrados
    printf("\nPaíses escolhidos:\n");
    for(int i = 0; i < 10; i++){
        printf("País: %s  -> Esporte: %s -> Medalhas: %d\n", 
               paises[i].nome, paises[i].esporte, paises[i].medalhas);
    }

    return 0;
}
