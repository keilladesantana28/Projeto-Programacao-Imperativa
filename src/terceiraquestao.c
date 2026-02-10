#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PAISES 10    // definição fixa da quantidade máxima de países dentro da lista
#define MAX_ESPORTES 100 // definição fixa da quantidade máxima de esportes que fazem parte das olímpiadas

struct EsporteCont {
    char nome[50]; // estrutura da contagem dos esportes
    int cont;
};

struct Paises {
    char NOC[10]; // estrutura da configuração dos países
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
    int ano, total_digitado = 0;       // instanciação das variáveis
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

        paises[i].medalhas = 0;      // incialização das variáveis   
        paises[i].total_esportes = 0;
        strcpy(paises[i].esporte_destaque, "");
        total_digitado++;
    }

    FILE *ptr = fopen("data/results.csv", "r");  // escopo que serve para a abertura e leitura do arquivo
    if (ptr == NULL) {
        printf("Erro: Arquivo data/results.csv nao encontrado!\n");
        return 1;
    }

    fgets(str, sizeof(str), ptr); // Pula cabeçalho

    while (fgets(str, sizeof(str), ptr) != NULL) {
        int coluna = 0, dentro_aspas = 0, k = 0;      // evita a "leitura" de vírgulas dentro de aspas e lê apenas o conteúdo real
        char token[200] = ""; // buffer temporário para armazenar o dado da célula atual
        int ano_csv = 0;
        char edicao_csv[20] = "", esporte_csv[50] = "", noc_csv[10] = "", medalha_csv[20] = ""; // variáveis auxiliares para identificar cada elemento dentro do registro

        for (int j = 0; j <= strlen(str); j++) {
            char c = str[j];
            if (c == '"') { dentro_aspas = !dentro_aspas; continue; }  // alterna a flag para saber se estamos dentro de aspas

            if ((c == ',' && !dentro_aspas) || c == '\0' || c == '\n') {   // serve para identificar o fim de uma coluna (separador ,)
                token[k] = '\0';
                limpar(token); // token é finalizado e limpo de espaços extras

                if (coluna == 0) ano_csv = atoi(token); // atoi serve para converter a string do ano em um número inteiro
                if (coluna == 1) strcpy(edicao_csv, token);
                if (coluna == 2) strcpy(esporte_csv, token);  // copia o conteúdo do token para a variável correspondente ao esporte
                if (coluna == 6) strcpy(noc_csv, token);
                if (coluna == 10) strcpy(medalha_csv, token);

                coluna++; k = 0;   // incrementa a coluna para poder mapear o próximo elemento
                if (c == '\0' || c == '\n') break; // se encontrar final da linha ou nulo, para o loop da linha
            } else {
                token[k++] = c; // serve para construir o token caractere por caractere
            }
        }

        if (ano_csv == ano && strcmp(edicao_csv, edicao) == 0) {
            if (strcmp(medalha_csv, "NA") != 0 && strlen(medalha_csv) > 1) { // esse NA serve para filtrar e ignorar atletas que não ganharam medalhas
                for (int i = 0; i < total_digitado; i++) { // percorre a lista de países digitados pelo usuário
                    if (strcmp(noc_csv, paises[i].NOC) == 0) {
                        paises[i].medalhas++; // incrementa a quantidade de medalhas do país encontrado
                        int achou = 0; // flag para indicar se o esporte já está na lista do país
                        for (int j = 0; j < paises[i].total_esportes; j++) {
                            if (strcmp(paises[i].esportes[j].nome, esporte_csv) == 0) { // compara para saber se o esporte lido já existe na lista deste país
                                paises[i].esportes[j].cont++;
                                achou = 1;
                                break;
                            }
                        }
                        if (!achou && paises[i].total_esportes < MAX_ESPORTES) {
                            strcpy(paises[i].esportes[paises[i].total_esportes].nome, esporte_csv); // adiciona um novo esporte à lista se não foi encontrado
                            paises[i].esportes[paises[i].total_esportes].cont = 1;
                            paises[i].total_esportes++;
                        }
                    }
                }
            }
        }
    }
    fclose(ptr); // fecha o arquivo para liberar recursos do sistema

    for (int i = 0; i < total_digitado; i++) {
        int maior = 0;
        for (int j = 0; j < paises[i].total_esportes; j++) {
            if (paises[i].esportes[j].cont > maior) { // verifica qual esporte tem a maior contagem de medalhas
                maior = paises[i].esportes[j].cont; // atualiza a variável de maior quantidade
                strcpy(paises[i].esporte_destaque, paises[i].esportes[j].nome);
            }
        }
    }

    printf("\n--- RESULTADO FINAL ---\n");
    for (int i = 0; i < total_digitado; i++) {
        printf("%s: %d medalhas. Destaque: %s\n", // imprimir o resultado final formatado
               paises[i].NOC, paises[i].medalhas, 
               paises[i].esporte_destaque[0] ? paises[i].esporte_destaque : "Nenhum");
    }

    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
    
    if (gnuplotPipe) {
        printf("\nGerando grafico...\n");
        // Configurações visuais do gráfico
        fprintf(gnuplotPipe, "set title 'Medalhas por Pais - %d %s'\n", ano, edicao);
        fprintf(gnuplotPipe, "set style fill solid 0.7\n"); // Barras preenchidas
        fprintf(gnuplotPipe, "set boxwidth 0.5\n");         // Largura da barra
        fprintf(gnuplotPipe, "set ylabel 'Total de Medalhas'\n");
        fprintf(gnuplotPipe, "set xlabel 'Paises'\n");
        fprintf(gnuplotPipe, "set grid y\n"); // Linhas de grade apenas no eixo Y
        
        // Plota usando dados enviados via stream ('-')
        // xtic(1) usa a primeira coluna (nome do país) como rótulo do eixo X
        fprintf(gnuplotPipe, "plot '-' using 2:xtic(1) with boxes title 'Medalhas' lc rgb 'blue'\n");

        // Envia os dados processados para o GNUplot
        for (int i = 0; i < total_digitado; i++) {
            fprintf(gnuplotPipe, "%s %d\n", paises[i].NOC, paises[i].medalhas);
        }

        // 'e' sinaliza o fim da transmissão de dados
        fprintf(gnuplotPipe, "e\n");
        pclose(gnuplotPipe);
    } else {
        printf("\nAviso: GNUplot nao encontrado. O grafico nao pode ser gerado.\n");
    }

    return 0;
}