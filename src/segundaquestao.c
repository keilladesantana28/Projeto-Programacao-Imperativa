#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main() {
    int contagemMulheres[2026] = {0};
    char *token; // variável token do tipo ponteiro de caracteres. Serve para guardar o endereço exato do ano e do gênero contidos dentro do arquivo.
    FILE *arquivo; //O FILE é um ponteiro que estabelece a ponte entre o programa e o arquivo, tendo controle da posição da leitura para que o código processe uma linha após a outra até o final.
    
    char linha[2000]; //Um vetor que guarda de forma temporaria o conteudo de cada linha do arquivo.
    
    arquivo = fopen("results.csv", "r"); // O fopen cria a ponte de acesso que verifica se o arquivo existe e se tem permissão para ler os dados através do modo "r" (read).
     if (arquivo == NULL) { //condicional para tratamento de erro. Caso o variavel arquivo for nula (não existe ou não tenho acesso), o programa imprime um alerta e encerra a execução com status de erro.
        printf("X Erro ao abrir o arquivo!\n"); 
            return 1;
    }
    printf("\n  A evolução da quantidade de mulheres em cada olimpíada...\n\n");
    // Repete a leitura de cada linha do arquivo e a guarda na variável "linha" até que os dados cheguem ao fim.
    while (fgets(linha, 2000, arquivo)) {
        int colunas = 0;
    token = strtok(linha, ",");  // strtok é a função que serve para separar cada dado da linha ao encontrar uma virgula e isola para guardar so os dados que são de interesse para o programa.
        
        if (token != NULL) {  //Se o token não for nulo, significa que temos um dado válido para trabalhar. Se estiver vazio, o programa pula esse bloco para não travar.
            char anoLinha[20];
            strcpy(anoLinha, token); // O strcpy copia o texto que está no token e guarda na varriável anoLinha. 

          int ano = atoi(token); // A função atoi converte o texto do ano isolado pelo token em um número inteiro, permitindo que o programa o utilize para contar e organizar os dados.

       
        for (colunas = 0; colunas < 5 && token != NULL; colunas++) {
            token = strtok(NULL, ",");  // O laço percorre e descarta as 5 colunas seguintes da linha, usando o strtok para seguir o marcador até a coluna que contém o gênero.
        }

       
        if (token != NULL && (token[0] == 'F' || token[1] == 'F')) {
            if (ano >= 1896 && ano <= 2024) {  // Verifica o gênero  e  o ano que está dentro do intervalo olímpico. Se ambos forem verdadeiros, incrementa em 1 a contagem para aquele ano específico no vetor.
                contagemMulheres[ano]++;
          } 
        }
    }
} 

fclose(arquivo); // Encerra a leitura do arquivo CSV.

//Abre uma ponte de acesso (pipe) com o programa GNUplot.
// As mensagens são enviadas através do pipe para que o GNUplot receba os dados e desenhe o gráfico.
    FILE *gnuplotPipe = popen("gnuplot -persist", "w");
    
    if (gnuplotPipe) {
        fprintf(gnuplotPipe, "set terminal windows size 1000,600\n");
        fprintf(gnuplotPipe, "set encoding utf8\n");
        fprintf(gnuplotPipe, "set xtics rotate by 90 right\n");
        fprintf(gnuplotPipe, "set title 'Evolução da Participação das Mulheres nas Olímpiadas'\n");
        fprintf(gnuplotPipe, "set style fill solid 0.7\n");
        fprintf(gnuplotPipe, "set boxwidth 0.8\n");
        fprintf(gnuplotPipe, "set ylabel 'Quantidade de Mulheres'\n");
        fprintf(gnuplotPipe, "set xlabel 'Anos Olímpicos'\n");
        fprintf(gnuplotPipe, "set grid y\n");
        
        // Plota os dados: eixo X é o ano, eixo Y é a contagem
        fprintf(gnuplotPipe, "plot '-' using 1:2 with boxes title 'Mulheres' lc rgb 'purple'\n");

        for (int i = 1896; i <= 2024; i++) {
            if (contagemMulheres[i] > 0) {
                // Envia o ano e a quantidade para o GNUplot
                fprintf(gnuplotPipe, "%d %d\n", i, contagemMulheres[i]);
                printf("Na olímpiada de %d: %d mulheres participaram\n", i, contagemMulheres[i]);
            }
        }

        fprintf(gnuplotPipe, "e\n");
        pclose(gnuplotPipe);
    } else {
        printf("\nAviso: GNUplot não encontrado. Mostrando apenas no terminal.\n");
    }
}