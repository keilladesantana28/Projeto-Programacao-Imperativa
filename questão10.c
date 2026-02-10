#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Estrutura para organizar os dados
typedef struct {
    char noc[10];           //Código de 3 letras - identificador do país
    int medalhas;           //Total de medalhas
    int atletas_count;      //Contador de atletas únicos 
    int ids_vistos[2000];   //Array de IDs já vistos 
    float eficiencia;       //Razão final - (medalhas / atletas)
} Pais;

//Função para limpar aspas e quebras de linha
void extrair_coluna(char *linha, int indice_alvo, char *destino) {
    int col = 0;                //Conta em qual coluna o computador está no momento
    int i = 0;                  //É a posição do caractere atual na linha
    int j = 0;                  //É a posição onde vamos escrever no destino
    int entre_aspas = 0;        //Se for 1, o computador sabe que está dentro de um texto e deve ignorar vírgulas

    
    while (linha[i] != '\0' && linha[i] != '\n' && linha[i] != '\r') {
        
        if (linha[i] == '\"') {         //continar lendo até a linha acabar
            entre_aspas = !entre_aspas; //Toda vez que o código encontra uma aspa ("), ele inverte o valor de entre_aspas, fui descobrir depois de tempos que dá para escrever assim
            
        }else if (linha[i] == ',' && !entre_aspas) { //se passou em uma linha, tinha (,) e o entre_aspas estava desligado
            if (col == indice_alvo) {
                break;                  //verifica se é a coluna que eu quero. Se for, para
            }
            col++;                      //Se não era a coluna que eu queria, passo para a próxima
            j = 0;                      //Reseta para a próxima coluna
        } 
        else {

            if (col == indice_alvo) {   //Se for um caractere comum e eu estiver na coluna alvo, copio para o destino
                destino[j++] = linha[i];
            
        }
        i++;                            //Avanço para o próximo caractere da linha do CSV
    }
    destino[j] = '\0';                  //Fecha a string
}

}

int main() {
    Pais lista[10];
    char linha[4096]; 
    char game[100], medal[50], id_str[50], noc_str[20];

    //Entrada do usuário: escolha dos 10 países
    printf("--- PROJETO OLIMPIADAS: CALCULADORA DE EFICIENCIA ---\n");
    printf("Digite o codigo NOC (3 letras) de 10 paises (ex: BRA, USA, JPN):\n");
    
    for (int i = 0; i < 10; i++) {
        printf("Pais %d: ", i + 1);
        scanf("%s", lista[i].noc);      //O usuário digita o código que está no CSV
        lista[i].medalhas = 0;          
        lista[i].atletas_count = 0;
        lista[i].eficiencia = 0.0;
        
        //Limpar o array de IDs para evitar lixo de memória (correção nova)
        for(int k=0; k<2000; k++) {
            lista[i].ids_vistos[k] = 0;
        }
    }
    
    //Abertura do arquivo em modo leitura ("r")
    FILE *arquivo = fopen("results.csv", "r");
    if (arquivo == NULL) {
        printf("\n[ERRO CRITICO]: Arquivo 'results.csv' nao encontrado!\n");
        printf("Certifique-se que o arquivo esta na MESMA pasta do executavel.\n");
        return 1;
    }

    printf("\nLendo arquivo... (Isso pode demorar um pouco se o arquivo for grande)\n");
    
    fgets(linha, 4096, arquivo);  //Pula o cabeçalho

    int linhas_lidas = 0;

    //Processamento leitura linha por linha
    while (fgets(linha, 4096, arquivo)) {
        linhas_lidas++;
        

        extrair_coluna(linha, 0, game); //Assumindo coluna 0 = nome do jogo

        //Verifica se contém "2020" e "Summer"
        if (strstr(game, "2020") != NULL && strstr(game, "Summer") != NULL) {
            
            extrair_coluna(linha, 7, noc_str); //Coluna 7: código NOC do país

            for (int i = 0; i < 10; i++) {
                if (strcmp(noc_str, lista[i].noc) == 0) {
                    
                    extrair_coluna(linha, 4, medal);  //Coluna 4: medalha
                    extrair_coluna(linha, 6, id_str); //Coluna 6: ID
                    
                    int id_atual = atoi(id_str);

                    //Se houver texto na coluna medalha, incrementa o total
                    if (strlen(medal) > 0 && strcmp(medal, "NA") != 0) {
                        lista[i].medalhas++;
                    }

                    //Lógica de atleta único: verifica se o id_visto já existe antes de contar
                    int ja_visto = 0;
                    for (int k = 0; k < lista[i].atletas_count; k++) {
                        if (lista[i].ids_vistos[k] == id_atual) {
                            ja_visto = 1;
                            break;
                        }
                    }
                    
                    //Verifica bounds check 
                    if (!ja_visto && lista[i].atletas_count < 2000) {
                        lista[i].ids_vistos[lista[i].atletas_count] = id_atual;
                        lista[i].atletas_count++;
                    }
                }
            }
        }
    }
    fclose(arquivo);

    //Cálculo da eficiência (razão: medalhas / atletas)
    for (int i = 0; i < 10; i++) {
        if (lista[i].atletas_count > 0) {
            lista[i].eficiencia = (float)lista[i].medalhas / lista[i].atletas_count;
        }
    }

    //Ordenação 
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9 - i; j++) {
            if (lista[j].eficiencia < lista[j+1].eficiencia) {
                Pais temp = lista[j];
                lista[j] = lista[j+1];
                lista[j+1] = temp;
            }
        }
    }

    printf("\n%-10s | %-10s | %-10s | %-10s\n", "NOC", "MEDALHAS", "ATLETAS", "EFICIENCIA");
    printf("----------------------------------------------------\n");
    for (int i = 0; i < 10; i++) {
        printf("%-10s | %-10d | %-10d | %.4f\n", lista[i].noc, lista[i].medalhas, lista[i].atletas_count, lista[i].eficiencia);
    }
    
    printf("\nProcessamento concluido. Total de linhas lidas: %d\n", linhas_lidas);

    return 0;
}
