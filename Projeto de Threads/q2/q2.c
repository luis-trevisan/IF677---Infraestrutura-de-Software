#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int *interseccao;
pthread_mutex_t mutex[5]; //um array de mutex é utilizado para travar somente a intersecção onde há disputa entre os trens
pthread_cond_t cheia[5]; //será utilizado um array de cond pois cada intersecção terá sua proŕia condição não afetando as outras intersecções

void *rotina(void *thread_id) {
    int id = *(int *) thread_id;
    
    for(int i = 0; i < 5; i++) {
        pthread_mutex_lock(&mutex[i]); //entra na região critica

        //para evitar falsa sinalização usamos um while que verifica se a interseccao está cheia ou não
        //se estiver, cond_wait é usada e bloqueia a thread na variável de condição cheia
        while(interseccao[i] == 2) {
            pthread_cond_wait(&cheia[i], &mutex[i]);
        }

        interseccao[i]++; //o trem sai da intersecção
        printf("Trem %d entrou na interseccao %d, tem %d trens na interseccao\n", id, i + 1, interseccao[i]);
        pthread_mutex_unlock(&mutex[i]); //sai da região critica

        usleep(500000); //o trem permanece na intersecção por 500ms;
        
        pthread_mutex_lock(&mutex[i]); //entra na região critica
        interseccao[i]--;              //o trem sai da intersecção
        printf("Trem %d saiu da interseccao %d, tem %d trens na interseccao\n", id, i + 1, interseccao[i]);

        //se houver espaço na intersecçao a thread que está bloquada na variavel de condição cheia é despertada
        if(interseccao[i] < 2) {
            pthread_cond_signal(&cheia[i]);
        }
        
        pthread_mutex_unlock(&mutex[i]); //sai da região critica
    }

    pthread_exit(NULL); // encerra a thread
}

int main() {
    int *t_id;
    interseccao = (int *) calloc(5, sizeof(int));
    pthread_t trem[10];
    
    if(interseccao == NULL) {
    	printf("Falha na alocação\n");
    	exit(1);
    }
    
    //Iniciando o array de mutex e o array de conds
    for(int i = 0; i < 5; i++) {
        pthread_mutex_init(&mutex[i], NULL);
        pthread_cond_init(&cheia[i], NULL);
    }

    //Criando as threads e enviando o indice do for como parametro atraves do ponteiro t_id,
    //para ser usado como id da thread na rotina
    for(int i = 0; i < 10; i++) {
        t_id = (int *) malloc(sizeof(int));
        *t_id = i;

        if(pthread_create(&trem[i], NULL, &rotina, (void *) t_id) != 0) {
            printf("Falha na criacao da thread\n");
            exit(1);
        }
    }

    //esperando o retorno das threads
    for(int i = 0; i < 10; i++) {
        if(pthread_join(trem[i], NULL) != 0) {
            printf("Falha na espera da thread\n");
            exit(1);
        }
    }

    //desalocando os recursos utilizados durante o programa
    for(int i = 0; i < 5; i++) {
        pthread_mutex_destroy(&mutex[i]);
        pthread_cond_destroy(&cheia[i]);
    }
    
    free(interseccao);
    free(t_id);

    return 0;
}
