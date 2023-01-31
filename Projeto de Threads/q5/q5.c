#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define PENSANDO 0
#define COM_FOME 1
#define COMENDO 2
#define ESQUERDA (filosofo + 5 - 1) % 5
#define DIREITA (filosofo + 1) % 5

int estado[5]; //array para salvar o estado da thread(filosofo)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //mutex que irá travar as threads(filosofos) quando forem pegar e devolver os garfos
pthread_cond_t cond_fork[5]; //array de cond para colocar as threads(filosofos) em espera; 

void teste(int filosofo) {
    //se o filosofo está com fome e seus vizinhos não estão comendo, faça
    if(estado[filosofo] == COM_FOME && estado[ESQUERDA] != COMENDO && estado[DIREITA] != COMENDO) { 
        estado[filosofo] = COMENDO; //altera o estado do filosofo para comendo 
        pthread_cond_signal(&cond_fork[filosofo]); //se houver alguma thread(filosofo) esperando, ele é despertado 
    }
}

//filosofo pensa por um tempo de 1 até 3 segundos
void think(int filosofo) { 
    printf("Filosofo %d esta pensando\n", filosofo);
    sleep((rand() % 3) + 1);
}

void get_forks(int filosofo) {
    pthread_mutex_lock(&mutex); //trava o mutex, nenhum filosofo pode mexer nos garfos
    estado[filosofo] = COM_FOME;
    printf("Filosofo %d esta com fome\n", filosofo);
    teste(filosofo); //verifica se o filosofo pode comer

    //enquanto o filosofo está com fome e pelo menos um de seus vizinhos está comendo, faça:
    while(estado[filosofo] == COM_FOME && (estado[ESQUERDA] == COMENDO || estado[DIREITA] == COMENDO)) {
        pthread_cond_wait(&cond_fork[filosofo], &mutex); //filosofo fica em espera
    }

    pthread_mutex_unlock(&mutex); //destrava o mutex, os filosofos podem mexer nos garfos
}

//filosofo come por um tempo de 1 até 3 segundos
void eat(int filosofo) { 
    printf("Filosofo %d esta comendo\n", filosofo);
    sleep((rand() % 3) + 1);
}

void put_forks(int filosofo) {
    pthread_mutex_lock(&mutex); //trava o mutex, nenhum filosofo pode mexer nos garfos
    estado[filosofo] = PENSANDO;
    printf("Filosofo %d acabou de comer\n", filosofo);
    teste(ESQUERDA); //verifica se o vizinho da esquerda está com fome e pode comer
    teste(DIREITA); //verifica se o vizinho da direita está com fome e pode comer
    pthread_mutex_unlock(&mutex); //destrava o mutex, os filosofos podem mexer nos garfos
}

void *rotina(void *thread_id) {
    int id = *(int *) thread_id;

    while(1) {
        think(id); //o filosofo está pensando
        get_forks(id); //filosofo parou de pensar e vai tentar pegar os garfos
        eat(id); //o filosofo está comendo
        put_forks(id); //filosofo acabou de comer e vai devolver os garfos
    }

    pthread_exit(NULL);
}

int main() {
    int *t_id;
    pthread_t filosofos[5];

    srand(time(NULL)); //rand é pseudo-randomica, para torna-la randomica multiplicamos o rand() pelo valor retornado da funçaõ time(NULL)

    for(int i = 0; i < 5; i++) {
        pthread_cond_init(&cond_fork[i], NULL);
    }
 
    for(int i = 0; i < 5; i++) {
        t_id = (int *) malloc(sizeof(int));
        *t_id = i;
        if(pthread_create(&filosofos[i], NULL, &rotina, (void *) t_id) != 0) {
            printf("Falha na criacao da thread\n");
        }
    }

    for(int i = 0; i < 5; i++) {
        if(pthread_join(filosofos[i], NULL) != 0) {
            printf("Falha na espera da thread\n");
        }
    }

    for(int i = 0; i < 5; i++) {
        pthread_cond_destroy(&cond_fork[i]);
    }

    pthread_mutex_destroy(&mutex);

    free(t_id);

    return 0;
}