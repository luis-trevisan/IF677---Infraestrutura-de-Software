#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define TAM 3
#define LINHAS 3
#define COLUNAS 3
#define NUM_THREADS 3

// O "-1" é utilizado para sinalizar que nao exitstem mais valores nessa linha da matriz esparsa
// Por causa disso uma matriz 3x3 tem que ser transformada em 3x4 uma 4x4 em 4x5 etc
// Criando a matriz_esparsa1

int matriz_esparsa1_posicoes[TAM][TAM + 1] = {{0, 1, -1, -1}, {0, 1, 2, -1}, {1, 2, -1, -1}};
float matriz_esparsa1_valores[TAM][TAM] = {{2.0, -1.0}, {-1.0, 2.0, -1.0}, {-1.0, 2.0}};

// criando a matriz_esparsa2
int matriz_esparsa2_posicoes[TAM][TAM + 1] = {{0, -1, -1, -1}, {2, -1, -1, -1}, {0, 1, 2, -1}};
float matriz_esparsa2_valores[TAM][TAM] = {{3.0}, {5.0}, {2.0, 8.0, 1.0}};


//Matrizes alternativas para teste
/*
int matriz_esparsa1_posicoes[TAM][TAM + 1] = {{0, 1, -1, -1}, {0, 1, 2, -1}, {1, -1, -1, -1}};
float matriz_esparsa1_valores[TAM][TAM] = {{4.0, 8.0}, {9.0, 7.0, 1.0}, {3}};

int matriz_esparsa2_posicoes[TAM][TAM + 1] = {{0, 2, -1, -1}, {1, -1, -1, -1}, {0, 2, -1, -1}};
float matriz_esparsa2_valores[TAM][TAM] = {{2.0, 5.0}, {3.0}, {1.0, 1.0}};
*/


float vetor_denso[TAM] = {5, 8, 3};

float matriz_densa[TAM][TAM] = {{1, 2, 5}, {3, 4, 2}, {5, 6, 1}};

float resultado_esparsa_X_densa[TAM][TAM];
float resultado_esparsa_X_vetor[TAM][TAM];
float resultado_esparsa_X_esparsa[TAM][TAM];

// Decidimos que todas as threads deveriam fazer os 3 tipos de calculos pedidos pela questao
// e somente depois disso fazer o thread join, ao inves de fazer thread join para cada calculo necessario para questão

void *threadCode(void *tid)
{
    int i, j, k, x;
    int threadId = (*(int *)tid);
    int posicoes[TAM + 1];
    float valores[TAM + 1];

    // Multipiclando a matriz_esparsa1 por um vetor denso
    for (i = threadId; i < LINHAS; i = i + NUM_THREADS)
    {
        j = 0;
        resultado_esparsa_X_vetor[i][j] = 0;
        for (k = 0; matriz_esparsa1_posicoes[i][k] != -1; k++)
        {
            // "x" recebe a posicao em que deve procurar o elemento correto do "vetor_denso"
            x = matriz_esparsa1_posicoes[i][k];
            resultado_esparsa_X_vetor[i][j] = resultado_esparsa_X_vetor[i][j] + matriz_esparsa1_valores[i][k] * vetor_denso[x];
        }
    }

    // Multipiclando uma matriz_esparsa1 pela matriz_esparsa2;
    for (i = threadId; i < LINHAS; i = i + NUM_THREADS)
    {
        for (j = 0; j < COLUNAS; j++)
        {
            resultado_esparsa_X_esparsa[i][j] = 0;
            for (k = 0; matriz_esparsa1_posicoes[i][k] != -1; k++)
            {
                // "x" recebe a posicao em que deve procurar o elemento correto da matriz_esparsa2
                x = matriz_esparsa1_posicoes[i][k];
                
                for( int w = 0; matriz_esparsa2_posicoes[x][w] != -1; w++ )
                {
                    if( matriz_esparsa2_posicoes[x][w] == j)
                    {
                        resultado_esparsa_X_esparsa[i][j] = resultado_esparsa_X_esparsa[i][j] + matriz_esparsa1_valores[i][k] * matriz_esparsa2_valores[x][w];
                    }
                }
                 
            }
        }
    }

    // Multipiclando uma matriz esparsa por uma densa
    for (i = threadId; i < LINHAS; i = i + NUM_THREADS)
    {
        for (j = 0; j < COLUNAS; j++)
        {
            resultado_esparsa_X_densa[i][j] = 0;

            for (k = 0; matriz_esparsa1_posicoes[i][k] != -1; k++)
            {
                // "x" recebe a posicao em que deve procurar o elemento correto da matriz_densa
                x = matriz_esparsa1_posicoes[i][k];
                resultado_esparsa_X_densa[i][j] = resultado_esparsa_X_densa[i][j] + matriz_esparsa1_valores[i][k] * matriz_densa[x][j];
            }
        }
    }
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    int *taskids;

    // Criando threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        taskids = (int *)malloc(sizeof(int));
        *taskids = i;
        pthread_create(&threads[i], NULL, threadCode, (void *)taskids);
    }

    // Esperando todas as threads acabarem
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("Resultado da matriz esparsa vezes o vetor denso: \n");
    for (int i = 0; i < LINHAS; i++)
    {
        printf("%.2f\t", resultado_esparsa_X_vetor[i][0]);
        printf("\n");
    }

    printf("Resultado da matriz esparsa vezes outra matriz esparsa: \n");
    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            printf("%.2f\t", resultado_esparsa_X_esparsa[i][j]);
        }

        printf("\n");
    }

    printf("Resultado da matriz esparsa vezes a matriz densa: \n");
    for (int i = 0; i < LINHAS; i++)
    {
        for (int j = 0; j < COLUNAS; j++)
        {
            printf("%.2f\t", resultado_esparsa_X_densa[i][j]);
        }

        printf("\n");
    }

    //desalocando os recursos usados no programa
    free(taskids);

    return 0;
}
