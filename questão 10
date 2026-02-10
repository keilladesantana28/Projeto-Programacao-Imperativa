#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Estrutura para organizar os dados de cada país escolhido
typedef struct {
    char noc[4];          //Código de 3 letras (ex: BRA, USA, FRA)
    int medalhas;         //Total de medalhas acumuladas
    int atletas_count;    //Contador de atletas únicos (pessoas reais)
    int ids_vistos[2000]; //Array para não contar o mesmo ID de atleta duas vezes
    float eficiencia;     //Razão final: Medalhas / Atletas
} Pais;

// Função auxiliar para ler colunas do CSV ignorando vírgulas dentro de aspas
void extrair_coluna(char *linha, int indice_alvo, char *destino) {

    int col = 0;           //Conta em qual coluna o computador está no momento.
    int i = 0;             //É a posição do caractere atual na linha
    int j = 0;             //É a posição onde vamos escrever no destino
    int entre_aspas = 0;  //Se for 1, o computador sabe que está dentro de um texto e deve ignorar vírgulas.

    while (linha[i] != '\0' && col <= indice_alvo) {
        if (linha[i] == '\"') {//continar lendo até a linha não acabar
            entre_aspas = !entre_aspas; //Toda vez que o código encontra uma aspa ("), ele inverte o valor de entre_aspas, fui descobrir depois de tempos que dá para escrever assim
        } else if (linha[i] == ',' && !entre_aspas) {//se passou em uma linha, tinha (,) e o entre_aspas estava desligado
            if (col == indice_alvo){//verifica se é a coluna que eu quero. Se for, para
            break;
            }

            col++;//Se não era a coluna que eu queria, passo para a próxima
            j = 0;//Reseto o preenchimento do destino para começar do zero na nova coluna
            
        }else {
            //Se for um caractere comum e eu estiver na coluna alvo, copio para o destino
            if (col == indice_alvo) destino[j++] = linha[i];
        }
        i++;//Avanço para o próximo caractere da linha do CSV
    }
    destino[j] = '\0';//Coloco o "ponto final" da string em
}

int main() {
    Pais lista[10];
    char linha[2048];
    char game[100], medal[20], id_str[20], noc_str[10];

    //Entrada do Usuário: Escolha dos 10 países
    printf("--- PROJETO OLIMPIADAS: CALCULADORA DE EFICIENCIA ---\n");
    printf("Digite o codigo NOC (3 letras) de 10 paises para analisar em 2020:\n");
    for (int i = 0; i < 10; i++) {
        printf("Pais %d: ", i + 1);
        scanf("%s", lista[i].noc); //O usuário digita o código que está no CSV
        lista[i].medalhas = 0;
        lista[i].atletas_count = 0;
        lista[i].eficiencia = 0.0;
    }

    //Abertura do arquivo em modo leitura ("r")
    FILE *arquivo = fopen("results.csv", "r");
    if (arquivo == NULL) {
        printf("Erro: Arquivo results.csv nao encontrado!\n");
        return 1;
    }

    printf("\nProcessando dados... Isso pode levar alguns segundos.\n");
    fgets(linha, 2048, arquivo); //Pula o cabeçalho

    //Processamento leitura linha por linha
    while (fgets(linha, 2048, arquivo)) {
        extrair_coluna(linha, 0, game); //Coluna 0: Nome da Olimpíada
        
        //Filtramos apenas para a edição de 2020 (conforme o arquivo)
        if (strcmp(game, "2020 Summer Olympics") == 0) {
            extrair_coluna(linha, 7, noc_str); //Coluna 7: Código NOC do país
            
            for (int i = 0; i < 10; i++) {
                if (strcmp(noc_str, lista[i].noc) == 0) {
                    extrair_coluna(linha, 4, medal);  //Coluna 4: Medalha ganha (ou vazio)
                    extrair_coluna(linha, 6, id_str); //Coluna 6: ID do atleta
                    int id_atual = atoi(id_str);

                    //Se houver texto na coluna medalha, incrementa o total
                    if (strlen(medal) > 1) lista[i].medalhas++;

                    //Lógica de Atleta Único: verifica se o id_visto já existe antes de contar
                    int ja_foi_visto = 0;
                    for (int k = 0; k < lista[i].atletas_count; k++) {
                        if (lista[i].ids_vistos[k] == id_atual) {
                            ja_foi_visto = 1;
                            break;
                        }
                    }
                    if (!ja_foi_visto && lista[i].atletas_count < 2000) {
                        lista[i].ids_vistos[lista[i].atletas_count] = id_atual;
                        lista[i].atletas_count++;
                    }
                }
            }
        }
    }
    fclose(arquivo);

    //Cálculo da Eficiência (Razão Medalhas / Atletas)
    for (int i = 0; i < 10; i++) {
        if (lista[i].atletas_count > 0) {
            lista[i].eficiencia = (float)lista[i].medalhas / lista[i].atletas_count;
        }
    }

    //Ordenação (Bubble Sort) para identificar os mais eficientes
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9 - i; j++) {
            if (lista[j].eficiencia < lista[j+1].eficiencia) {
                Pais temp = lista[j];
                lista[j] = lista[j+1];
                lista[j+1] = temp;
            }
        }
    }

    //Exibição do Ranking Final
    printf("\n%-10s | %-10s | %-10s | %-10s\n", "NOC", "MEDALHAS", "ATLETAS", "EFICIENCIA");
    printf("----------------------------------------------\n");
    for (int i = 0; i < 10; i++) {
        printf("%-10s | %-10d | %-10d | %.4f\n", 
               lista[i].noc, lista[i].medalhas, lista[i].atletas_count, lista[i].eficiencia);
    }

    return 0;
}
