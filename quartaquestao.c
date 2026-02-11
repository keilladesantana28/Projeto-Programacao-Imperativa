#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Eu defini esses limites altos para evitar erros de memória, já que os arquivos 
   do Keith Galli são enormes e podem travar o programa se o espaço for insuficiente. */
#define MAX_MULHERES 200000
#define MAX_RANKING 20000
#define TAM_LINHA 4096

typedef struct { 
    int id_referencia; 
} AtletaFeminina;

typedef struct { 
    char nome_completo[150]; 
    int contador_medalhas; 
} RegistroRanking;

/* Eu usei variáveis globais para não estourar a stack do sistema durante o processamento. */
AtletaFeminina lista_mulheres[MAX_MULHERES];
int total_mulheres = 0;
RegistroRanking ranking_final[MAX_RANKING];
int total_atletas_ranking = 0;

/* Eu desenvolvi esta função de comparação com critério duplo:
   1. Maior número de medalhas (ordem decrescente).
   2. Em caso de empate, eu uso a ordem alfabética (A-Z) para desempatar. */
int ordenar_por_medalha_e_nome(const void *a, const void *b) {
    RegistroRanking *atletaA = (RegistroRanking *)a;
    RegistroRanking *atletaB = (RegistroRanking *)b;

    if (atletaB->contador_medalhas != atletaA->contador_medalhas) {
        return atletaB->contador_medalhas - atletaA->contador_medalhas;
    }
    /* Se chegou aqui, é porque empataram em medalhas. Eu uso strcmp para o desempate. */
    return strcmp(atletaA->nome_completo, atletaB->nome_completo);
}

void carregar_bios() {
    FILE *arquivo_bios = fopen("bios.csv", "r");
    if (!arquivo_bios) return;
    
    char buffer[TAM_LINHA];
    /* Eu usei este fgets para pular o cabeçalho do bios.csv. */
    fgets(buffer, TAM_LINHA, arquivo_bios); 
    
    while (fgets(buffer, TAM_LINHA, arquivo_bios)) {
        char *ponteiro_caractere = buffer;
        int indice_coluna = 0, dentro_aspas = 0;
        char *colunas[20];
        
        /* Eu fiz este parser manual para ignorar vírgulas que aparecem dentro de aspas. */
        colunas[indice_coluna++] = ponteiro_caractere;
        for (; *ponteiro_caractere; ponteiro_caractere++) {
            if (*ponteiro_caractere == '"') dentro_aspas = !dentro_aspas;
            else if (*ponteiro_caractere == ',' && !dentro_aspas) {
                *ponteiro_caractere = '\0';
                colunas[indice_coluna++] = ponteiro_caractere + 1;
            }
        }
        /* Eu filtrei apenas atletas do sexo feminino e guardei o ID único da coluna 7. */
        if (indice_coluna >= 8 && strstr(colunas[1], "Female")) {
            lista_mulheres[total_mulheres++].id_referencia = atoi(colunas[7]);
        }
    }
    fclose(arquivo_bios);
}

void processar_results() {
    FILE *arquivo_results = fopen("results.csv", "r");
    if (!arquivo_results) return;
    
    char buffer[TAM_LINHA];
    /* Eu também pulei o cabeçalho do arquivo de resultados aqui. */
    fgets(buffer, TAM_LINHA, arquivo_results);

    while (fgets(buffer, TAM_LINHA, arquivo_results)) {
        char *colunas[20];
        int c_idx = 0, dentro_aspas = 0;
        char *ptr_aux = buffer;

        /* Eu apliquei o mesmo parser para manter a integridade das colunas de ID e Nome. */
        colunas[c_idx++] = ptr_aux;
        for (; *ptr_aux; ptr_aux++) {
            if (*ptr_aux == '"') dentro_aspas = !dentro_aspas;
            else if (*ptr_aux == ',' && !dentro_aspas) {
                *ptr_aux = '\0';
                colunas[c_idx++] = ptr_aux + 1;
            }
        }

        if (c_idx >= 7) {
            int ano_competicao = atoi(colunas[0]);
            
            /* Eu filtrei o ano >= 2004 e usei strcmp para garantir que 'na' (sem medalha) seja ignorado. */
            if (ano_competicao >= 2004 && strlen(colunas[4]) > 1 && strcmp(colunas[4], "na") != 0) {
                int id_atleta = atoi(colunas[6]);
                
                /* Eu busquei o ID no meu vetor de mulheres para validar o gênero. */
                int eh_feminina = 0;
                for(int i=0; i < total_mulheres; i++) {
                    if(lista_mulheres[i].id_referencia == id_atleta) { eh_feminina = 1; break; }
                }

                if (eh_feminina) {
                    int ja_registrada = 0;
                    for (int i = 0; i < total_atletas_ranking; i++) {
                        if (strcmp(ranking_final[i].nome_completo, colunas[5]) == 0) {
                            ranking_final[i].contador_medalhas++;
                            ja_registrada = 1; break;
                        }
                    }
                    /* Eu insiro o nome no ranking se for a primeira medalha processada dessa atleta. */
                    if (!ja_registrada && total_atletas_ranking < MAX_RANKING) {
                        strcpy(ranking_final[total_atletas_ranking].nome_completo, colunas[5]);
                        ranking_final[total_atletas_ranking].contador_medalhas = 1;
                        total_atletas_ranking++;
                    }
                }
            }
        }
    }
    fclose(arquivo_results);
}

int main() {
    carregar_bios();
    processar_results();
    
    /* Eu chamei o qsort aqui com a lógica de desempate alfabético que eu implementei. */
    qsort(ranking_final, total_atletas_ranking, sizeof(RegistroRanking), ordenar_por_medalha_e_nome);

    int limite_top = (total_atletas_ranking < 10) ? total_atletas_ranking : 10;
    
    /* Eu montei esta tabela formatada para que a auditoria no terminal fique alinhada. */
    printf("\n==================================================================\n");
    printf("     AUDITORIA: TOP 10 MEDALHISTAS FEMININAS (POS-2004)\n");
    printf("==================================================================\n");
    printf(" POS | ATLETA                                   | MEDALHAS \n");
    printf("------------------------------------------------------------------\n");
    for (int i = 0; i < limite_top; i++) {
        printf(" %02d  | %-40s | %d\n", i+1, ranking_final[i].nome_completo, ranking_final[i].contador_medalhas);
    }
    printf("==================================================================\n");

    /* Eu exportei os dados para o grafico.dat. */
    FILE *arquivo_temporario = fopen("grafico.dat", "w");
    for (int i = 0; i < limite_top; i++) {
        fprintf(arquivo_temporario, "\"%s\" %d\n", ranking_final[i].nome_completo, ranking_final[i].contador_medalhas);
    }
    fclose(arquivo_temporario);

    /* Eu usei o popen para abrir o Gnuplot e configurei o gráfico para ser legível. */
    FILE *pipe_gnuplot = popen("gnuplot -persist", "w");
    if (pipe_gnuplot) {
        fprintf(pipe_gnuplot, "set title 'Top 10 Medalhistas Femininas (Desde 2004)' font ',12'\n");
        fprintf(pipe_gnuplot, "set style fill solid 0.8 border -1\n");
        fprintf(pipe_gnuplot, "set grid y\n");
        fprintf(pipe_gnuplot, "set bmargin 12\n"); 
        fprintf(pipe_gnuplot, "set yrange [0:*]\n"); 
        fprintf(pipe_gnuplot, "set xtics rotate by -45 font ',8'\n"); 
        fprintf(pipe_gnuplot, "plot 'grafico.dat' using 2:xtic(1) with boxes notitle lc rgb 'blue'\n");
        pclose(pipe_gnuplot);
    }

    return 0;
}
