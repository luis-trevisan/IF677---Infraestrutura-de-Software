#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_ARQUIVOS 4
#define NUM_THREADS 3
#define NUM_CANDIDATOS 5

int *lista;
//como a posição 0 é dos votos nulos, então precisamos do +1 para que cada candidato tenha o seu mutex
pthread_mutex_t mutex[NUM_CANDIDATOS + 1];

void *rotina(void *thread_id) {
    int id = *(int *) thread_id;
    char buffer[20];
    int num_candidato = 0;

    for(int k = id; k < NUM_ARQUIVOS; k += NUM_THREADS) {
        //abrindo os arquivos para leitura em sequencia
        sprintf(buffer, "./arquivos/%d.in", k + 1);
        FILE *arq = fopen(buffer, "r");

        if (arq == NULL) {
            printf("Nao foi possivel abrir o arquivo %s\n", buffer);
        }

        //enquanto não chegar no final do arquivo, faça
        while(fscanf(arq, "%d", &num_candidato) != EOF){ //o valor lido é salvo em num_candidato
            //região crítica para contabilizar os votos, travando somendo o mutex do candidato,
            //deixando que outras threads somem se estiverem contadno votos de outro candidato.
            pthread_mutex_lock(&mutex[num_candidato]);
            //soma o voto na posição do candidato no array
            lista[num_candidato]++;
            pthread_mutex_unlock(&mutex[num_candidato]);
        }

        fclose(arq);
    }

    pthread_exit(NULL);
}

int main() {
    char aux = '%';
    int *t_id, votos = 0, vencedor = 0, percento = 0, maior = 0;
    pthread_t threads[NUM_THREADS];

    //como a posição 0 é dos votos nulos, então precisamos do +1 para que cada candidato tenha sua posição no array
    lista = (int*) calloc (NUM_CANDIDATOS + 1, sizeof(int));

    for(int i = 0; i < NUM_CANDIDATOS + 1; i++) {
        pthread_mutex_init(&mutex[i], NULL);
    }

    //Criando as threads e enviando o indice do for como parametro atraves do ponteiro t_id,
    //para ser usado como id da thread na rotina
    for(int i = 0; i < NUM_THREADS; i++) {
        t_id = (int *) malloc(sizeof(int));
        *t_id = i;

        if(pthread_create(&threads[i], NULL, &rotina, (void *) t_id) != 0) {
            printf("Falha na criacao da thread\n");
            exit(1);
        }
    }

    //esperando o retorno das threads
    for(int i = 0; i < NUM_THREADS; i++) {
        if(pthread_join(threads[i], NULL) != 0) {
            printf("Falha na espera da thread\n");
            exit(1);
        }
    }

    // Calculando o total de votos
    for(int i = 0; i < NUM_CANDIDATOS + 1; i++) {
        votos += lista[i];

        
    }
    //calculando as porcentagens e printando 
    for (int i = 0; i < NUM_CANDIDATOS + 1; i++)
    {
        percento = (lista[i]*100) / votos;

        if (i == 0)
        {
            printf("porcentagem de votos nulos = %d%c\n",percento,aux);
        }else
            printf("porcentagem de votos do candidato %d = %d%c\n", i,percento,aux);
    }

    //calculando o vencedor
    for(int i = 1; i < NUM_CANDIDATOS + 1; i++) {
        if(lista[i] > maior) {
            maior = lista[i];
            vencedor = i;
        }
    }
    
    printf("candidato %d venceu\n",vencedor);

    //desalocando os recursos utilizados durante o programa
    for(int i = 0; i < NUM_CANDIDATOS + 1; i++) {
        pthread_mutex_destroy(&mutex[i]);
    }

    free(t_id);
    free(lista);

    return 0;
}