#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TAM_REGIAO_CRITICA 25
#define LENDO 0
#define NAO_LENDO 1

int regiao_critica[TAM_REGIAO_CRITICA];
int var_cond[TAM_REGIAO_CRITICA];           //array que salva se uma thread leitora está lendo ou não aquela posição do array "região_critica"

// um array de mutex será utilizado para travar
// somente a posição da região crítica onde
// uma thread escritora deseja escrever
pthread_mutex_t mutex[TAM_REGIAO_CRITICA];

// um array de cond será utilizado para colocar em espera 
// as threads escritoras que desejam escrever na posição
// onde uma thread leitora está lendo
pthread_cond_t cond[TAM_REGIAO_CRITICA];

//rotina da thread escritora
void *rotina_escritoras(void *thread_id) { 
    int id = *(int *) thread_id;

    for(int i = 0; i < TAM_REGIAO_CRITICA; i++) {
        pthread_mutex_lock(&mutex[i]); //trava o mutex e impede outras escritoras de escreverem na posição "i" da região critica

        //verifica se há alguma thread leitora lendo nesse momento, se sim a thread escritora espera
        while(var_cond[i] == LENDO) { 
            pthread_cond_wait(&cond[i], &mutex[i]);
        }

        regiao_critica[i] = id; //escreve o "id" da thread no array
        printf("Thread escritora %d: escreveu seu ID na posicao %d\n", id, i);
        pthread_mutex_unlock(&mutex[i]); //destrava o mutex e agora outras escritoras podem escrever na posiçaõ "i" da região critica
    }

    pthread_exit(NULL);
}

//rotina da thread escritora
void *rotina_leitoras(void *thread_id) {
    int id = *(int *) thread_id;

    for(int i = 0; i < TAM_REGIAO_CRITICA; i++) {
        var_cond[i] = LENDO; //salva em var_cond que a thread leitora irá ler naquela posição 
        printf("Thread leitora %d: leu %d posicao %d\n", id, regiao_critica[i], i);
        var_cond[i] = NAO_LENDO; //salva em var_cond que a thread leitora terminou de ler na posição 
        pthread_cond_signal(&cond[i]); //libera a thread escritora que estiver em espera
    }

    pthread_exit(NULL);
}

int main() {
    int M, N, *t_id;
    pthread_t *leitoras, *escritoras;

    //recebendo a entrada do usuario
    printf("Numero de threads escritoras: ");
    scanf("%d", &M);
    printf("Numero de threads leitoras: ");
    scanf("%d", &N);

    //alocando os arrays de threads
    escritoras = (pthread_t *) malloc(M * sizeof(pthread_t));
    leitoras = (pthread_t *) malloc(N * sizeof(pthread_t));

    if(escritoras == NULL || leitoras == NULL) {
        printf("Falha na alocacao\n");
        exit(1);
    }

    //iniciando o mutex e a variavel de condição
    for(int i = 0; i < TAM_REGIAO_CRITICA; i++) {
        var_cond[i] = 1;
        pthread_mutex_init(&mutex[i], NULL);
        pthread_cond_init(&cond[i], NULL);
    }

    //Criando as threads escritoras e enviando o indice do for como parametro atraves do ponteiro t_id,
    //para ser usado como id da thread na rotina
    for(int i = 0; i < M; i++) {
        t_id = (int *) malloc(sizeof(int));
        *t_id = i;
        if(pthread_create(&escritoras[i], NULL, &rotina_escritoras, (void *) t_id) != 0) {
            printf("Falha na criacao das threads escritoras\n");
            exit(1);
        }
    }
    
    //Criando as threads leitoras e enviando o indice do for como parametro atraves do ponteiro t_id,
    //para ser usado como id da thread na rotina
    for(int i = 0; i < N; i++) {
        t_id = (int *) malloc(sizeof(int));
        *t_id = i;
        if(pthread_create(&leitoras[i], NULL, &rotina_leitoras, (void *) t_id) != 0) {
            printf("Falha na criacao das threads leitoras\n");
            exit(1);
        }
    }

    //esperando o retorno das threads escritoras
    for(int i = 0; i < M; i++) {
        if(pthread_join(escritoras[i], NULL) != 0) {
            printf("Falha na espera das threads escritoras\n");
            exit(1);
        }
    }
    
    //esperando o retorno das threads leitoras
    for(int i = 0; i < N; i++) {
        if(pthread_join(leitoras[i], NULL) != 0) {
            printf("Falha na espera das threads leitoras\n");
            exit(1);
        }
    }

    //desalocando os recursos utilizados no programa 
    for(int i = 0; i < TAM_REGIAO_CRITICA; i++) {
        pthread_mutex_destroy(&mutex[i]);
        pthread_cond_destroy(&cond[i]);
    }

    free(escritoras);
    free(leitoras);
    free(t_id);

    return 0;
}